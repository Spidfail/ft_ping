#include "ft_ping.h"

void    init_data(t_icmp *echo_request, t_sum *session) {
    session->seq_number = 1;
    session->time_min = DBL_MAX;
    session->time_max = DBL_MIN;

    echo_request->data_size = _PING_DATA_SIZE;
    echo_request->datagram_size = _ICMP_HDR_SIZE + echo_request->data_size;
    echo_request->packet_size = _IP_HDR_SIZE + echo_request->datagram_size;
    
    echo_request->data = ft_calloc(1, echo_request->data_size);
    echo_request->datagram = ft_calloc(1, echo_request->datagram_size);
    echo_request->packet = ft_calloc(1, echo_request->packet_size);
    if (!echo_request->data || !echo_request->datagram || !echo_request->packet)
        error_handle(0, "Failed to allocate data");
}

void    clean_data(t_icmp *echo_request) {
    // Make sure all buffers (especially the header) are cleared.
    ft_bzero(echo_request->data, echo_request->data_size);
    ft_bzero(echo_request->datagram, echo_request->datagram_size);
    ft_bzero(echo_request->packet, echo_request->packet_size);
    ft_bzero(&echo_request->header, sizeof(echo_request->header));
    // Important to avoid errors because of expired informations
    echo_request->is_packet = false;
    echo_request->received_size = 0;
}

void    free_data(t_icmp *echo_request) {
    // Make sure all buffers (especially the header) are cleared.
    free(echo_request->data);
    free(echo_request->datagram);
    free(echo_request->packet);
}