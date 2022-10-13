#include "ft_ping.h"
#include "libft.h"
#include <asm-generic/socket.h>
#include <bits/types/struct_timeval.h>
#include <sys/socket.h>

bool    g_interrupt = false;
bool    g_tick = true;
bool    g_is_packet = false;

static void    check_input(int ac) {
    if (ac != 2)
        error_handle(EX_USAGE, NULL);
}

void            signal_handler(int sig) {
    switch (sig) {
        case SIGINT :
            g_interrupt = true;
        break;
        case SIGALRM :
            g_tick = true;
        break;
    }
}

void    init_data(t_icmp *echo_request) {
    echo_request->data_size = _PING_DATA_SIZE;
    echo_request->datagram_size = _ICMP_HDR_SIZE + echo_request->data_size;
    echo_request->packet_size = _IP_HDR_SIZE + echo_request->datagram_size;
    echo_request->seq_number = 1;
    
    echo_request->data = ft_calloc(1, echo_request->data_size);
    echo_request->datagram = ft_calloc(1, echo_request->datagram_size);
    echo_request->packet = ft_calloc(1, echo_request->packet_size);
    if (!echo_request->data || !echo_request->datagram || !echo_request->packet)
        error_handle(0, "Failed to allocate data");
}

void    clean_data(t_icmp *echo_request) {
    // Make sure all buffers (especially the header) are cleared.
    ft_bzero(echo_request->data, echo_request->data_size);
    ft_bzero(echo_request->datagram, echo_request->datagram_size);
    ft_bzero(echo_request->packet, echo_request->packet_size);
    ft_bzero(&echo_request->header, sizeof(echo_request->header));
}

void    free_data(t_icmp *echo_request) {
    // Make sure all buffers (especially the header) are cleared.
    free(echo_request->data);
    free(echo_request->datagram);
    free(echo_request->packet);
}

int     send_data(int sockfd, t_icmp *echo_request, t_host *dest) {
    ssize_t         rtn = 0;

    generate_datagram(echo_request);
    rtn = sendto(sockfd, echo_request->datagram, echo_request->datagram_size,
                 0, dest->addr_info->ai_addr, dest->addr_info->ai_addrlen);
    if (rtn == -1)
        return -1;

    dprintf(2, "CONTROL SEND --> CHECKSUM = %u | TYPE = %u\n",
            echo_request->header.checksum, echo_request->header.type);

    return rtn;
}

void     print_packet(t_icmp *echo_request, t_host *dest) {
    if (!g_is_packet)
        return ;

    struct icmphdr *icmphdr = &echo_request->packet->icmp_hdr;
    printf("-------------Print Packet !----------------\n");
    printf("type: %u\n", icmphdr->type);
    printf("code: %u\n", icmphdr->code);
    printf("checksum: %u\n", icmphdr->checksum);
    printf("id: %u\n", icmphdr->un.echo.id);
    printf("sequence: %u\n", icmphdr->un.echo.sequence);
    for (size_t i = 0; i < 14; i++) {
        printf("%x", *((unsigned int *)(echo_request->datagram + 8) + i));
    }
    printf("\n");
    printf("%u\n", ~icmphdr->checksum & 0xffff);
    printf("Host : %s\n", dest->addr_str);
    printf("-------------------------------------------\n");
}

int main(int ac, char *av[]) {
    int             sockfd_raw = 0;
    t_host          dest;
    t_icmp          echo_request;
    struct timeval  timeout = {1, 0};
    int             broadcast = true;
    // Clear the buffer in case of garbage

    ft_bzero(&dest, sizeof(t_host));
    ft_bzero(&echo_request, sizeof(t_icmp));

    check_input(ac);

    //// Build the address:
    // Should use SOCK_RAW because ICMP is a protocol with
    // no user interface. So it need special options.
    // AF_INET for IPV4 type addr
    if ((sockfd_raw = socket(_INET_FAM, SOCK_RAW, IPPROTO_ICMP)) == -1)
        error_handle(0, "Failed to open socket [AF_INET, SOCK_RAW, IPPROTO_ICMP]");
    setsockopt(sockfd_raw, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(int));
    setsockopt(sockfd_raw, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval));
    
    init_data(&echo_request);
    host_lookup(av[1], &dest);

    signal(SIGINT, signal_handler);
    signal(SIGALRM, signal_handler);

    while (true) {
        if (send_data(sockfd_raw, &echo_request, &dest) == -1)
            dprintf(2, "----------!!!!!ERROR SENDING DATA\n");
    dprintf(2, " HERRREEEE ?????\n");
        if (receive_data(sockfd_raw, &echo_request) == EXIT_FAILURE) {
            if (errno == EWOULDBLOCK) {
                dprintf(2, "----------!!!!!TIMEOUT\n");
                g_is_packet = false;
            }
            else
                dprintf(2, "----------!!!!!ERROR RECEIVING DATA\n");
        } else
            g_is_packet = true;
        while (!g_tick) { }
        print_packet(&echo_request, &dest);
        g_tick = false;
        alarm(1);
        clean_data(&echo_request);
        ++echo_request.seq_number;
    }

    freeaddrinfo(dest.addr_info);
    free_data(&echo_request);
}