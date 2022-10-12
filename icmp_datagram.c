#include "ft_ping.h"
#include "libft.h"
#include <netinet/ip_icmp.h>
#include <stddef.h>

/// Allocate and fill the data pointer with the content of
/// a `struct timeval` upon gettimeofday() call.
void        generate_data(char **data, size_t *size) {
    if (data == NULL || size == NULL)
        return ;

    struct timeval  time_of_day = {0};

    *size = sizeof(struct timeval);
    if ((*data = ft_calloc(1, *size)) == NULL)
        error_handle(0, "Failed to allocate ICMP data");
    if (gettimeofday(&time_of_day, NULL) != 0)
        error_handle(0, "Error while recovering the time of day");
    ft_memcpy(*data, (char *)&time_of_day, *size);
}

/// Fill the header for an ECHO_REQUEST with sequence number equal to 1.
void        fill_header(struct icmphdr *header, char *data, size_t data_size) {
    header->type = ICMP_ECHO;
    header->code = 0;
    header->un.echo.id = getuid();
    header->un.echo.sequence = 1;
    header->checksum = calculate_checksum_icmp(*header, data, data_size);
}

void        create_packet(t_icmp *icmp_dtg) {
    const short       header_size = sizeof(struct icmphdr);
    t_packet          new_packet = NULL;

    icmp_dtg->packet_size = header_size + icmp_dtg->data_size;
    if ((new_packet = ft_calloc(1, icmp_dtg->packet_size)) == NULL)
        error_handle(0, "Failed to allocate new packet");
    ft_memcpy(new_packet, &icmp_dtg->header, header_size);
    ft_memcpy(new_packet + header_size, icmp_dtg->data, icmp_dtg->data_size);
}