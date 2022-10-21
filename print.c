#include "ft_ping.h"
#include <stdint.h>
#include <stdio.h>

void     print_packet(const t_icmp *echo_request, const t_sum *session) {
    char    addr_str[INET_ADDRSTRLEN];
    
    bzero(addr_str, INET_ADDRSTRLEN);
    if (NULL == inet_ntop(_INET_FAM,
            &echo_request->packet->ip_hdr.ip_src,
            addr_str,
            INET_ADDRSTRLEN)) {
        dprintf(2, "ERROR NTOP\n");
    }
        
    __PRINT_PACKET(_ICMP_HDR_SIZE + echo_request->data_size,
        addr_str,
        echo_request->packet->icmp_hdr.un.echo.sequence,
        echo_request->packet->ip_hdr.ip_ttl,
        session->time_enlapsed);
}

void    print_packet_error(const t_icmp *echo_request, uint8_t et, uint8_t ec) {
    char    addr_str[INET_ADDRSTRLEN];

    bzero(addr_str, INET_ADDRSTRLEN);
    if (NULL == inet_ntop(_INET_FAM,
            &echo_request->packet->ip_hdr.ip_src,
            addr_str,
            INET_ADDRSTRLEN)) {
        dprintf(2, "ERROR NTOP\n");
    }
    switch (et) {
        case ICMP_TIME_EXCEEDED:
            if (ec == ICMP_EXC_TTL)
                printf("From %s: icmp_seq=%i Time to live exceeded\n", \
                    addr_str, echo_request->header.un.echo.sequence);
            break;
    }
}

void    print_header_begin(const t_host *dest, const t_icmp *request) {
    __PRINT_HEADER_BEG(dest->addr_info->ai_canonname,
            dest->addr_str,
            request->data_size,
            request->packet_size)
}

void    print_sum(t_sum *sumup, t_host *dest) {
    // Calculate the enlapsed time at the end to avoid
    // making a calcul at each turn.
    if (dest == NULL || dest->addr_info == NULL)
        return ;
    __PRINT_SUM(dest->addr_info->ai_canonname,
        sumup->seq_number,
        sumup->recv_number,
        sumup->err_number,
        get_enlapsed_ms(&sumup->time_start, &sumup->time_end))
    __PRINT_RTT(sumup->time_min,
        sumup->time_delta,
        sumup->time_max,
        sumup->recv_number)
}