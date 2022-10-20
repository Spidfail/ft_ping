#include "ft_ping.h"
#include <limits.h>
#include <stddef.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

ssize_t     get_enlapsed_ms(const struct timeval *start, const struct timeval *end) {
    return ((end->tv_sec - start->tv_sec) * 1000)
        + ((end->tv_usec - start->tv_usec) / 1000);
}

void     print_packet(t_icmp *echo_request) {
    char                addr_str[INET_ADDRSTRLEN];
    
    bzero(addr_str, INET_ADDRSTRLEN);
    if (NULL == inet_ntop(_INET_FAM,
            &echo_request->packet->ip_hdr.ip_src,
            addr_str,
            INET_ADDRSTRLEN)) {
        dprintf(2, "ERROR NTOP\n");
    }
        
    __PRINT_PACKET(echo_request->received_size,
        addr_str,
        echo_request->packet->icmp_hdr.un.echo.sequence,
        echo_request->packet->ip_hdr.ip_ttl,
        get_enlapsed_ms((struct timeval *)echo_request->data, &echo_request->received_time));

#if DEBUG == true  ///////////////
    struct icmphdr *icmphdr = &echo_request->packet->icmp_hdr;
    printf("-------------Print Packet !----------------\n");
    printf("type: %u\n", icmphdr->type);
    printf("code: %u\n", icmphdr->code);
    printf("checksum: %u\n", icmphdr->checksum);
    printf("id: %u\n", icmphdr->un.echo.id);
    printf("sequence: %u\n", icmphdr->un.echo.sequence);
    for (size_t i = 0; i < 14; i++) {
        printf("%x", *((unsigned int *)(echo_request->datagram + 8) + i));
    }
    printf("\n");
    printf("%u\n", ~icmphdr->checksum & 0xffff);
    printf("Host : %s\n", dest->addr_str);
    printf("-------------------------------------------\n");
#endif //////////////////////////
}

void     print_header_begin(const t_host *dest, const t_icmp *request) {
    __PRINT_HEADER_BEG(dest->addr_info->ai_canonname,
            dest->addr_str,
            request->data_size,
            request->packet_size)
}

void            print_sum(t_sum *sumup, t_host *dest) {
    (void)sumup;
    (void)dest;
    struct timeval  end = {0};
    
    if (gettimeofday(&end, NULL) == -1)
        error_handle(0, "Error while gettin' time");
    __PRINT_SUM(dest->addr_info->ai_canonname,
        sumup->seq_number,
        sumup->recv_number,
        sumup->err_number,
        get_enlapsed_ms(&sumup->start, &end))
}