#include <ft_ping.h>

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

void    packet_update_timestamp(t_packet *packet) {
    struct timeval now = {0};
    
    timer_get(&now);
    packet_modify_data(packet, (char*)&now, sizeof(struct timeval));
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

static unsigned short     packet_print_ip_flag(unsigned short ip_off) {
     return (ip_off & 0xF000) >> 12;
}

static void     packet_print_error_ip(const struct ip *ip) {
    char    *data = (char*)ip;
    char    ip_src[16] = {0};
    char    ip_dst[16] = {0};
    size_t  len = sizeof(struct ip);
    int     rtn = 0;

    for (size_t i = 0 ; i < len ; i++) {
        rtn += printf("%02hhx", data[i]);
        if (rtn == 4) {
            printf(" ");
            rtn = 0;
        }
    }
    inet_ntop(AF_INET, &ip->ip_src, ip_src, 16);
    inet_ntop(AF_INET, &ip->ip_dst, ip_dst, 16);
    /* Calculate ip flags:
     * Take the ip->ip_id + 1 or ip + 7 (7th byte of the start)
     * Since the flags are set over the 3 bits over 8,
     * we need to shift them to the begining to calculate their value:
     * value >> 5;
     * There is the value, between 0 and 3, we can cast it to int
    */

    printf("\n");
    printf("Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src\tDst\tData\n");
    printf("%2x %2x  %02hhx %04x %02hx   %01x %04x  %02hx  %02hx %04hx %-16s %-16s ",
        ip->ip_v,
        ip->ip_hl,
        ip->ip_tos,
        (ip->ip_len > 0x2000) ? ntohs (ip->ip_len) : ip->ip_len,
        ntohs(ip->ip_id),
        packet_print_ip_flag(ip->ip_off),
        (ip->ip_off & IP_OFFMASK),
        ip->ip_ttl,
        ip->ip_p,
        ntohs(ip->ip_sum),
        ip_src,
        ip_dst
        );

    size_t          hlen = ip->ip_hl << 2;
    unsigned char   *data_ptr = (unsigned char*)ip + sizeof(*ip);

    while (hlen-- > sizeof (*ip))
        printf("%02x", (*data_ptr)++);
    printf("\n");
}

// ICMP: type 8, code 0, size 64, id 0x06ef, seq 0x0000
static void     packet_print_error_icmp(const struct icmphdr *icmp, unsigned short ip_len) {
    printf("ICMP:");
    printf(" type %i,", icmp->type);
    printf(" code %i,", icmp->code);
    printf(" size %hu,", ip_len);
    printf(" id 0x%04hx,", icmp->un.echo.id);
    printf(" seq 0x%04x\n", icmp->un.echo.sequence);
}

void     packet_print(const t_seq *sequence, bool verbose) {
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
        if (verbose) {
            const struct ip         *ip = (struct ip*)sequence->recv.data;
            int                     hdr_len = ip->ip_hl << 2;
            const struct icmphdr    *icmp = (struct icmphdr*)((unsigned char*)ip + hdr_len);

            packet_print_error_ip(ip);
            packet_print_error_icmp(icmp, ntohs(ip->ip_len) - hdr_len);
        }
    }
    else
        __PRINT_PACKET(sequence->recv_size, addr_str, sequence->recv.icmp_hdr.un.echo.sequence, sequence->recv.ip_hdr.ip_ttl, sequence->time_enlapsed_ms);
}