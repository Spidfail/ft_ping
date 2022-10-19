#include "ft_ping.h"

void     print_packet(t_icmp *echo_request, t_host *dest) {
    char    addr_str[INET_ADDRSTRLEN];
    
    bzero(addr_str, INET_ADDRSTRLEN);
    if (NULL == inet_ntop(_INET_FAM,
            &echo_request->packet->ip_hdr.ip_src,
            addr_str,
            INET_ADDRSTRLEN)) {
        dprintf(2, "ERROR NTOP\n");
    }
    //printf("%lu bytes from %s (%s): icmp_seq=%i ttl=%i time=%i.%i ms",
    //        echo_request->received_size,
    //        inet_ntop(_INET_FAM,
    //            &echo_request->packet->ip_hdr.ip_src,
    //            addr_str,
    //            INET_ADDRSTRLEN)
    //        );

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
}

void     print_header_begin(const t_host *dest, const t_icmp *request) {
    printf("PING %s (%s) %lu(%lu) bytes of data.\n",
            dest->addr_info->ai_canonname,
            dest->addr_str,
            request->data_size,
            request->packet_size);
}