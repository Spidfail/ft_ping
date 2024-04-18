#include "libft.h"
#include <ft_ping.h>
#include <stddef.h>

void    packet_copy(t_packet *target, const t_packet *source) {
    ft_memcpy(target->data, source->data, _PING_DATA_SIZE);
    ft_memcpy(&(target->icmp_hdr), &(source->icmp_hdr), sizeof(struct icmphdr));
    ft_memcpy(&(target->ip_hdr), &(source->ip_hdr), sizeof(struct iphdr));
}

void    packet_modify_sequence_number(t_packet *packet, uint16_t seq_num) {
    packet->icmp_hdr.un.echo.sequence = seq_num;
    packet->icmp_hdr.checksum = 0;
    packet->icmp_hdr.checksum = ping_datagram_checksum(&(packet->icmp_hdr), packet->data, _ICMP_HDR_SIZE + _PING_DATA_SIZE);
}

void    packet_modify_data(t_packet *packet, const char *data, size_t size) {
    ft_memcpy(packet->data, data, size);
    packet->icmp_hdr.checksum = 0;
    packet->icmp_hdr.checksum = ping_datagram_checksum(&(packet->icmp_hdr), packet->data, _ICMP_HDR_SIZE + _PING_DATA_SIZE);
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

    struct icmphdr  hdr_tmp = sequence->recv.icmp_hdr;
    struct ip       ip_tmp = sequence->recv.ip_hdr;
    unsigned short  ip_checksum = 0; 

    hdr_tmp.checksum = 0;
    ip_tmp.ip_sum = 0;
    hdr_tmp.checksum = ping_datagram_checksum(&hdr_tmp, sequence->send.data, _ICMP_HDR_SIZE + _PING_DATA_SIZE);
    ip_checksum = packet_checksum_calculate((char*)&ip_tmp, _IP_HDR_SIZE);
    if (memcmp(&hdr_tmp, &sequence->recv.icmp_hdr, _ICMP_HDR_SIZE) != 0)
        return EXIT_FAILURE;
    if (ip_checksum != sequence->recv.ip_hdr.ip_sum)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

/// Create a checksum by adding 2 octets by 2 octets successively.
/// For this reaser, buffer has to be casted in `uint16_t *`
uint16_t            packet_checksum_calculate(const char *buffer, size_t size) {
    uint32_t    sum = 0;
    uint16_t    *buffer_16 = (uint16_t*)buffer;

    while (size > 1) {
        sum += *buffer_16;
        size -= 2;
        ++buffer_16;
    }
    // If the number of byte is odd, add the remaining byte
    if (size > 0)
        sum += *buffer;
    while (sum >> 16)
       sum = (sum & 0xffff) + (sum >> 16);
    return ~sum;
}

int                 packet_send(int sockfd, const t_host *dest, const t_packet *packet) {
    ssize_t         rtn = 0;
    char            *to_send = NULL;
    size_t          size_hdr = sizeof(struct icmphdr);

    to_send = ft_calloc(1, size_hdr + _PING_DATA_SIZE);
    if (to_send == NULL)
        return -1;
    ft_memcpy(to_send, &(packet->icmp_hdr), size_hdr);
    ft_memcpy(to_send + size_hdr, &(packet->data), _PING_DATA_SIZE);

    rtn = sendto(sockfd, to_send, size_hdr + _PING_DATA_SIZE,
                 0, dest->addr_info->ai_addr, dest->addr_info->ai_addrlen);
    free(to_send);
    if (rtn == -1)
        return -1;
    return rtn;
}

// static void     packet_print_error_ip(const struct ip *ip) {
// }

// static void     packet_print_error_icmp(const struct icmphdr *icmp) {
// }

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
        __PRINT_PACKET_ERROR(sequence->recv_size, addr_str, error_desc);
        // packet_print_error_ip(&(sequence->recv.ip_hdr));
        // packet_print_error_icmp(&(sequence->recv.icmp_hdr));
    }
    else
        __PRINT_PACKET(sequence->recv_size, addr_str, sequence->recv.icmp_hdr.un.echo.sequence, sequence->recv.ip_hdr.ip_ttl, time_enlapsed);

}