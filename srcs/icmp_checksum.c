#include <ft_ping.h>

/// Create a checksum by adding 2 octets by 2 octets successively.
/// For this reaser, buffer has to be casted in `uint16_t *`
uint16_t    calculate_checksum(const char *buffer, size_t size) {
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

uint16_t    calculate_checksum_icmp(struct icmphdr header, const char *data, size_t size) {
    uint16_t    checksum = 0;
    // Allocating to be sure to get the right size.
    char        *buff = ft_calloc(1, _ICMP_HDR_SIZE + size);

    if (buff == NULL)
        error_handle(0, "Failed to allocate data buffer");
    // Fill the first part with the icmp header struct
    ft_memcpy(buff, &header, _ICMP_HDR_SIZE);
    // Fill the rest with the data buffer
    ft_memcpy(buff + _ICMP_HDR_SIZE, data, size);
    checksum = calculate_checksum(buff, size);
    free(buff);
    return checksum;
}