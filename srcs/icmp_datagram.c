#include <ft_ping.h>

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

uint16_t            ping_datagram_checksum(struct icmphdr *header, const char *data, size_t size) {
    uint16_t    checksum = 0;
    // Allocating to be sure to get the right size.
    char        *buff = ft_calloc(1, _ICMP_HDR_SIZE + size);

    if (buff == NULL)
        error_handle(0, "Failed to allocate data buffer");
    // Fill the first part with the icmp header struct
    ft_memcpy(buff, header, _ICMP_HDR_SIZE);
    // Fill the rest with the data buffer
    ft_memcpy(buff + _ICMP_HDR_SIZE, data, size);
    checksum = packet_checksum_calculate(buff, size);
    free(buff);
    return checksum;
}
/// Allocate and fill the data pointer with the content of
/// a `struct timeval` upon gettimeofday() call.
static void        ping_data_generate(char data[]) {
    struct timeval  time_of_day = {0};

    if (gettimeofday(&time_of_day, NULL) != 0)
        error_handle(0, "Error while recovering the time of day");
    ft_memcpy(data, &time_of_day, sizeof(struct timeval));
}

/// Fill the header for an ECHO_REQUEST with sequence number equal to 1.
static void        ping_fill_header_icmp(struct icmphdr *header, char *data, size_t data_size, uint16_t seq_number) {
    header->type = ICMP_ECHO;
    header->code = 0;
    header->checksum = 0; // Making sure the checksum is not set
    header->un.echo.id = getuid();
    header->un.echo.sequence = seq_number;
    header->checksum = ping_datagram_checksum(header, data, data_size);
}

void                ping_datagram_generate(t_packet *packet, uint16_t seq_number) {
    ping_data_generate(packet->data);
    ping_fill_header_icmp(&(packet->icmp_hdr), packet->data, _PING_DATA_SIZE, seq_number);
}
