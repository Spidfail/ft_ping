#include <ft_ping.h>

static int      verify_checksum(char *header, size_t header_size, unsigned short checksum) {
    if (header == NULL)
        return EXIT_FAILURE;
    return checksum == calculate_checksum(header, header_size) ? EXIT_SUCCESS : EXIT_FAILURE;
}

int             verify_ip_header(const struct ip *reception) {
    struct ip   tmp = {0};

    if (reception == NULL)
        return EXIT_FAILURE;
    tmp = *reception;
    tmp.ip_sum = 0;
    return verify_checksum((char *)&tmp, _IP_HDR_SIZE, reception->ip_sum);
}

int             verify_icmp_header(const t_packet *packet, const t_icmp *src) {
    struct icmphdr hdr_tmp = src->header;

    if (packet == NULL || (packet->icmp_hdr.type != ICMP_ECHOREPLY && packet->icmp_hdr.code != 0))
        return EXIT_FAILURE;
    hdr_tmp.type = 0;
    hdr_tmp.checksum = 0;
    hdr_tmp.checksum = calculate_checksum_icmp(hdr_tmp, src->data, src->data_size);
    if (memcmp(&hdr_tmp, &packet->icmp_hdr, _ICMP_HDR_SIZE) == 0)
        return EXIT_SUCCESS;
    return EXIT_FAILURE;
}