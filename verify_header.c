#include "ft_ping.h"
#include "libft.h"
#include <netinet/ip.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static int    verify_checksum(const void *header, size_t header_size, unsigned short checksum) {
    if (header == NULL)
        return EXIT_FAILURE;
    return checksum == calculate_checksum(header, header_size) ? EXIT_SUCCESS : EXIT_FAILURE;
}

int     verify_ip_header(const struct ip *reception) {
    if (reception == NULL)
        return EXIT_FAILURE;
    struct ip   tmp = *reception;

    tmp.ip_sum = 0;
    dprintf(2, " Check IP VERIFY : %d\n", verify_checksum(&tmp, tmp.ip_len, reception->ip_sum));
    return verify_checksum(&tmp, tmp.ip_len, reception->ip_sum);
}

int     verify_icmp_header(const t_packet *packet, const t_icmp *src) {
    dprintf(2, " Check ICMP VERIFY : %d\n", memcmp(&packet->icmp_hdr, &src->datagram, src->datagram_size));
    if (memcmp(&packet->icmp_hdr, &src->datagram, src->datagram_size) == 0)
        return EXIT_SUCCESS;
    return EXIT_FAILURE;
}