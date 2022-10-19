#include "ft_ping.h"

/// Allocate and fill the data pointer with the content of
/// a `struct timeval` upon gettimeofday() call.
static void        generate_data(char **data) {
    if (*data == NULL)
        return ;

    struct timeval  time_of_day = {0};

    if (gettimeofday(&time_of_day, NULL) != 0)
        error_handle(0, "Error while recovering the time of day");
    ft_memcpy(*data, &time_of_day, sizeof(struct timeval));
}

/// Fill the header for an ECHO_REQUEST with sequence number equal to 1.
static void        fill_header(struct icmphdr *header, char *data, size_t data_size, uint16_t seq_number) {
    header->type = ICMP_ECHO;
    header->code = 0;
    header->checksum = 0; // Making sure the checksum is not set
    header->un.echo.id = getuid();
    header->un.echo.sequence = seq_number;
    header->checksum = calculate_checksum_icmp(*header, data, data_size);
}

static void        fill_datagram(t_icmp *icmp_dtg) {
    ft_memcpy(icmp_dtg->datagram, &icmp_dtg->header, _ICMP_HDR_SIZE);
    ft_memcpy(icmp_dtg->datagram + _ICMP_HDR_SIZE, icmp_dtg->data, icmp_dtg->data_size);
}

static void    generate_datagram(t_icmp *echo_request, const t_sum *session) {
    generate_data(&echo_request->data);
    fill_header(&echo_request->header, echo_request->data, echo_request->data_size,
                session->seq_number);
    fill_datagram(echo_request);
}

int     send_new_packet(int sockfd, t_icmp *echo_request, t_host *dest, const t_sum *session) {
    ssize_t         rtn = 0;

    generate_datagram(echo_request, session);
    rtn = sendto(sockfd, echo_request->datagram, echo_request->datagram_size,
                 0, dest->addr_info->ai_addr, dest->addr_info->ai_addrlen);
    if (rtn == -1)
        return -1;
    return rtn;
}