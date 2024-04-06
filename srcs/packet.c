#include <ft_ping.h>

void    packet_cpy(t_packet *target, const t_packet *source) {
    ft_memcpy(target->data, source->data, _PING_DATA_SIZE);
    ft_memcpy(&(target->icmp_hdr), &(source->icmp_hdr), sizeof(struct icmphdr));
    ft_memcpy(&(target->ip_hdr), &(source->ip_hdr), sizeof(struct iphdr));
}

int     packet_receive(int sockfd, t_seq *sequence) {
    return recvfrom(sockfd,
        &sequence->recv,
         sizeof(t_packet),
         0, 
         sequence->sender.addr_info->ai_addr,
         &(sequence->sender.addr_info->ai_addrlen));
}

int     packet_verify_headers(const t_seq *sequence, uint8_t type, uint8_t code) {
    if (sequence->recv.icmp_hdr.type != type && sequence->recv.icmp_hdr.code != code)
        return EXIT_FAILURE;

    struct icmphdr  hdr_tmp = sequence->send->icmp_hdr;
    struct ip       ip_tmp = sequence->recv.ip_hdr;
    unsigned short  ip_checksum = 0; 

    hdr_tmp.type = 0;
    hdr_tmp.checksum = 0;
    ip_tmp.ip_sum = 0;
    hdr_tmp.checksum = ping_datagram_checksum(&hdr_tmp, sequence->send->data, _ICMP_HDR_SIZE + _PING_DATA_SIZE);
    ip_checksum = packet_checksum_calculate((char*)&ip_tmp, _IP_HDR_SIZE);
    if (memcmp(&hdr_tmp, &sequence->recv.icmp_hdr, _ICMP_HDR_SIZE) != 0)
        return EXIT_FAILURE;
    if (ip_checksum != sequence->recv.ip_hdr.ip_sum)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

void     packet_print(const t_seq *sequence, float time_enlapsed) {
    char    addr_str[INET_ADDRSTRLEN];
    char    *error_desc = NULL;
    
    bzero(addr_str, INET_ADDRSTRLEN);
    if (NULL == inet_ntop(_INET_FAM,
            &(sequence->recv.ip_hdr.ip_src),
            addr_str,
            INET_ADDRSTRLEN)) {
        fprintf(stderr, "%s Cannot resolve ip address of a received packet: %s\n",
                _ERROR_HEADER, strerror(errno));
    }
    error_desc =  error_icmp_mapping(sequence->recv.icmp_hdr.type, sequence->recv.icmp_hdr.code);
    if (error_desc != NULL) {
        __PRINT_PACKET_ERROR(sequence->recv_size + _ICMP_HDR_SIZE, addr_str, error_desc);
    }
    else
        __PRINT_PACKET(sequence->recv_size + _ICMP_HDR_SIZE, addr_str, sequence->send->icmp_hdr.un.echo.sequence, sequence->recv.ip_hdr.ip_ttl, time_enlapsed);

}