#include <ft_ping.h>

t_sum   *session_new(uint16_t pid, int sockfd, char *raw_addr, void (*datagram_generate)(t_packet *, uint16_t)) {
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

void        session_time_update(t_sum *session, double enlapsed) {
    session->time.time_delta += enlapsed;
    if (session->time.time_max < enlapsed)
        session->time.time_max = enlapsed;
    if (session->time.time_min > enlapsed)
        session->time.time_min = enlapsed;
}

void    session_print_sum(t_sum *session) {
    // Calculate the enlapsed time at the end to avoid
    // making a calcul at each turn.
    double  enlapsed = 0;
    double  loss = 0;
    
    if (!(session->time.time_start.tv_sec == 0 || session->time.time_start.tv_usec == 0
            || session->time.time_end.tv_sec == 0 || session->time.time_end.tv_usec == 0))
        enlapsed = timer_enlapsed_ms(&(session->time.time_start), &(session->time.time_end));
    if (session->dest.addr_info == NULL)
        return ;

    loss = ((session->seq_number+1) - session->recv_number);
    loss = (loss / ((double)session->seq_number+1));
    if (session->dest.addr_info->ai_canonname) {
        __PRINT_SUM(session->dest.addr_info->ai_canonname,
            session->seq_number + 1,
            session->recv_number,
            session->err_number,
            (size_t)loss,
            enlapsed)
    }
    else {
        __PRINT_SUM(session->dest.addr_orig,
            session->seq_number + 1,
            session->recv_number,
            session->err_number,
            (size_t)loss,
            enlapsed)
    }

    if (session->recv_number > 0)
        __PRINT_RTT(session->time.time_min,
            session->time.time_delta,
            session->time.time_max,
            session->recv_number)

}

static void      session_deinit(void *data) {
    t_sum   *session = data;
    freeaddrinfo(session->dest.addr_info);
    free(session);
}

t_list      *session_end(t_list **sessions) {
    t_sum   *session = (*sessions)->content;
    t_list  *next = (*sessions)->next;
    
    if (gettimeofday(&(session->time.time_end), NULL) == -1)
        error_handle(0, "Error while gettin' end time");
    session_print_sum(session);
    ft_lstdelone(*sessions, &session_deinit);
    *sessions = next;
    return next;
}
