#include "ft_opt.h"
#include <ft_ping.h>
#include <stdint.h>
#include <stdlib.h>
#include <strings.h>

t_sum    *session_new(uint16_t pid, int sockfd, char *raw_addr, void (*datagram_generate)(t_packet *, uint16_t)) {
    t_sum   *new_session = ft_calloc(1, sizeof(t_sum));
    
    if (new_session == NULL)
        return NULL;
    new_session->pid = pid;
    new_session->sockfd = sockfd;
    new_session->seq_number = 0;
    host_lookup(&(new_session->dest), raw_addr, true);
    new_session->time.time_min = DBL_MAX;
    new_session->time.time_max = DBL_MIN;
    // Init icmp datagram only. The IP header is generated automatically when using `sendto()`, based on socket options.
    (*datagram_generate)(&new_session->packet, new_session->seq_number);
    return new_session;
}

t_list  *session_init_all(uint16_t pid, const t_list *hosts, const t_arg_d *args_data) {
    t_list *sessions = NULL;

    for (const t_list *tmp = hosts ; tmp ; tmp = tmp->next) {
        t_sum   *new = NULL;
        t_list  *link = NULL;
        int     sockfd = 0;

        sockfd = socket_init(_INET_FAM, SOCK_RAW, IPPROTO_ICMP, args_data);
        new = session_new(pid, sockfd, tmp->content, ping_datagram_generate);
        if (new == NULL)
            return NULL;
        link = ft_lstnew(new);
        if (link == NULL) {
            free(new);
            return NULL;
        }
        ft_lstadd_back(&sessions, link);
    }
    return sessions;
}

// void    session_print_sum(t_sum *session) {
//     // Calculate the enlapsed time at the end to avoid
//     // making a calcul at each turn.
//     if (session->dest.addr_info == NULL)
//         return ;
//     if (session->dest.addr_info->ai_canonname) {
//         __PRINT_SUM(session->dest.addr_info->ai_canonname,
//             session->seq_number,
//             session->recv_number,
//             session->err_number,
//             get_enlapsed_ms(&session->time.time_start, &session->time.time_end))
//     }
//     else {
//         __PRINT_SUM(dest->addr_orig,
//             session->seq_number,
//             session->recv_number,
//             session->err_number,
//             get_enlapsed_ms(&session->time.time_start, &session->time.time_end))
//     }
//     if (session->recv_number > 3)
//         __PRINT_RTT(session->time.time_min,
//             session->time.time_delta,
//             session->time.time_max,
//             session->recv_number)
// }

// void    session_end(t_list **sessions) {
//     for (t_list *)
//     if (session->time.time_end.tv_sec == 0 && session->time.time_end.tv_usec == 0)
//         if (gettimeofday(&(session->time.time_end), NULL) == -1)
//             error_handle(0, "Error while gettin' end time");
//     session_print_sum(session);
//     interrupt(0);
// }
