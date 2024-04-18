#include "libft.h"
#include <ft_ping.h>
#include <math.h>

t_sum   *session_new(uint16_t pid, int sockfd, char *raw_addr, void (*datagram_generate)(t_packet *, uint16_t, uint16_t)) {
    t_sum   *new_session = ft_calloc(1, sizeof(t_sum));
    
    if (new_session == NULL)
        return NULL;
    new_session->pid = pid;
    new_session->sockfd = sockfd;
    new_session->seq_number = 0;
    new_session->dest.addr_orig = raw_addr;
    new_session->time.time_min = DBL_MAX;
    new_session->time.time_max = DBL_MIN;
    // Init icmp datagram only. The IP header is generated automatically when using `sendto()`, based on socket options.
    (*datagram_generate)(&new_session->packet, new_session->seq_number, new_session->pid);
    return new_session;
}

void    session_deinit_hosts(t_list **hosts) {
    if (*hosts == NULL)
        return ;
    session_deinit_hosts(&((*hosts)->next));
    ft_lstdelone_lst(*hosts, free);
}

t_list  *session_init_all(uint16_t pid, const t_list *hosts) {
    t_list *sessions = NULL;

    for (const t_list *tmp = hosts ; tmp ; tmp = tmp->next) {
        t_sum   *new = NULL;
        t_list  *link = NULL;

        // Initialise the sockfd in the main loop to avoid file descriptor shortage issue
        new = session_new(pid, -1, tmp->content, ping_datagram_generate);
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

void    session_print_begin(const t_sum *session, t_arg_d* const arg_data) {
    // If the resolution has been done (by default )
    unsigned long packet_size = (unsigned long)_PING_DATA_SIZE +_ICMP_HDR_SIZE + _IP_HDR_SIZE;

    if (!arg_data->interface) {
        if (session->dest.addr_info->ai_canonname) {
            __PRINT_HEADER_DATABYTE(session->dest.addr_info->ai_canonname, session->dest.addr_str,
                    (unsigned long)_PING_DATA_SIZE, packet_size);
        }
        else {
            __PRINT_HEADER_DATABYTE(session->dest.addr_orig, session->dest.addr_str,
                    (unsigned long)_PING_DATA_SIZE, packet_size);
        }
    }
    else {
        char                host_addr[INET_ADDRSTRLEN];
        struct ifaddrs      *interfaces = NULL;

        if (getifaddrs(&interfaces) == -1)
            error_handle(-1, "Fatal error: impossible to recover interfaces");
        for (struct ifaddrs *ifa = interfaces ; ifa != NULL ; ifa = ifa->ifa_next) {
            if (ft_strcmp(ifa->ifa_name, arg_data->interface) == 0 && ifa->ifa_addr->sa_family == AF_INET) {
                if (inet_ntop(AF_INET, &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr, host_addr, INET_ADDRSTRLEN) == NULL) {
                    freeifaddrs(interfaces);
                    error_handle(0, "Fatal error, host ip is not valide");
                }
            }
        }
        if (session->dest.addr_info->ai_canonname)
            __PRINT_HEADER_BEG_IF(session->dest.addr_info->ai_canonname, session->dest.addr_str,
                    (unsigned long)_PING_DATA_SIZE, packet_size, host_addr, arg_data->interface)
        else
            __PRINT_HEADER_BEG_IF(session->dest.addr_orig, session->dest.addr_str,
                    (unsigned long)_PING_DATA_SIZE, packet_size, host_addr, arg_data->interface)
        freeifaddrs(interfaces);

    }
    if (arg_data->verbose)
        __PRINT_HEADER_VERBOSE(session->pid);
    printf("\n");
}

void        session_time_update(t_sum *session, double enlapsed) {
    t_list  *new = NULL;
    double  *rtt = NULL;
    
    new = ft_calloc(1, sizeof(t_list));
    if (new == NULL)
        error_handle(-1, "Malloc error");
    rtt = ft_calloc(1, sizeof(double));
    if (rtt == NULL)
        error_handle(-1, "Malloc error");
    new->content = rtt;
    *rtt = enlapsed;
    ft_lstadd_back(&(session->time.rtt), new);
    if (session->time.time_max < enlapsed)
        session->time.time_max = enlapsed;
    if (session->time.time_min > enlapsed)
        session->time.time_min = enlapsed;
}

static double        session_sum_calculate_mean(t_list *rtt) {
    double  mean = 0;

    for (t_list *link = rtt ; link ; link = link->next)
        mean += *((double *)link->content);
    return mean / (double)ft_lstsize(rtt);
}

static double        session_sum_calculate_stddev(double mean, t_list *rtt) {
    double  buffer = 0;

    for (t_list *link = rtt ; link ; link = link->next)
        buffer += pow((*((double *)link->content) - mean), 2);
    return sqrt(buffer / (double)ft_lstsize(rtt));
}

static double        session_sum_calculate_loss(int nb_sent, int nb_recv) {
    double  loss = 0;

    loss = ((nb_sent) - nb_recv);
    loss /= ((double)nb_sent);
    return loss * 100;
}

void    session_print_sum(t_sum *session) {
    // Calculate the enlapsed time at the end to avoid
    // making a calcul at each turn.
    double  mean = 0;
    double  loss = 0;
    double  stddev = 0;

    if (session->dest.addr_info == NULL)
        return ;
    loss = session_sum_calculate_loss(session->seq_number + 1, session->recv_number);
    mean = session_sum_calculate_mean(session->time.rtt);
    stddev = session_sum_calculate_stddev(mean, session->time.rtt);
    if (session->dest.addr_info->ai_canonname) {
        __PRINT_SUM(session->dest.addr_info->ai_canonname,
            session->seq_number + 1,
            session->recv_number,
            (size_t)loss,
            session->err_number)
    }
    else {
        __PRINT_SUM(session->dest.addr_orig,
            session->seq_number + 1,
            session->recv_number,
            (size_t)loss,
            session->err_number)
    }

    if (session->recv_number > 0)
        __PRINT_RTT(session->time.time_min,
            mean,
            session->time.time_max,
            session->recv_number,
            stddev)

}

static void      session_deinit(void *data) {
    t_sum   *session = data;
    ft_lstclear(&(session->time.rtt), free);
    freeaddrinfo(session->dest.addr_info);
    free(session);
}

t_list      *session_end(t_list **sessions) {
    t_sum   *session = (*sessions)->content;
    t_list  *next = (*sessions)->next;
    
    if (gettimeofday(&(session->time.time_end), NULL) == -1)
        error_handle(0, "Error while gettin' end time");
    if (close(session->sockfd) == -1)
        error_handle(-1, "Cannot close fd");
    session_print_sum(session);

    ft_lstdelone(*sessions, &session_deinit);
    *sessions = next;
    return next;
}
