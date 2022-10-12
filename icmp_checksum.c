#include "ft_ping.h"

/// Create a checksum by adding 2 octets by 2 octets successively.
/// For this reaser, buffer has to be casted in `uint16_t *`
static uint16_t    calculate_checksum(uint16_t *buffer, size_t size) {
    uint16_t    checksum = 0;
    
    for (size_t i = 0 ; i < size ; ++i, ++buffer)
        checksum = checksum + *buffer;
    return ~checksum;
}

uint16_t    calculate_checksum_icmp(struct icmphdr header, char *data, size_t size) {
    uint16_t    checksum = 0;
    // Allocating to be sure to get the right size.
    char        *buff = ft_calloc(1, sizeof(struct icmphdr) + size);

    if (buff == NULL)
        error_handle(0, "Failed to allocate data buffer");
    // Fill the first part with the icmp header struct
    ft_memcpy(buff, (uint16_t *)&header, sizeof(struct icmphdr));
    // Fill the rest with the data buffer
    ft_memcpy(buff + sizeof(struct icmphdr), data, size);
    checksum = calculate_checksum((uint16_t*)buff, size);
    free(buff);
    return checksum;
}