#include <ft_ping.h>

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
