#include <ft_ping.h>

void    sequence_init(t_seq *sequence, const t_packet *to_send) {
    timer_get(&(sequence->time_sent));
    sequence->send = to_send;
    sequence->recv_size = 0;
    sequence->sender.addr_info = ft_calloc(1, sizeof(struct addrinfo));
    if (sequence->sender.addr_info == NULL)
        error_handle(0, "Fatal: Failed to allocate addrinfo");
    sequence->sender.addr_info->ai_addrlen = 100;
    sequence->sender.addr_info->ai_addr = ft_calloc(1, sequence->sender.addr_info->ai_addrlen);
    if (sequence->sender.addr_info->ai_addr == NULL)
        error_handle(0, "Fatal: Failed to allocate sockaddr");
}

void    sequence_deinit(t_seq *sequence) {
   free(sequence->sender.addr_info->ai_addr);
   free(sequence->sender.addr_info);
   bzero(sequence, sizeof(t_seq));
}

void    sequence_clean(t_seq *sequence) {
    struct addrinfo save = {0};
    memcpy(&save, sequence->sender.addr_info, sizeof(struct addrinfo));
    
    sequence->recv_size = 0;
    bzero(&sequence->recv, sizeof(t_packet));

    sequence->sender.addr_orig = NULL;
    bzero(sequence->sender.addr_str, INET_ADDRSTRLEN);
    bzero(sequence->sender.addr_info, sizeof(struct addrinfo));
    sequence->sender.addr_info->ai_addr = save.ai_addr;
    bzero(&sequence->time_enlapsed_ms, sizeof(time_t));
}
