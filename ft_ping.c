#include "ft_ping.h"
#include "libft.h"
#include <bits/types/struct_iovec.h>
#include <sys/socket.h>

static void    check_input(int ac) {
    if (ac != 2)
        error_handle(EX_USAGE, NULL);
}

static void     infinite_loop_shit() {
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

void            message_rec_init(t_message *msg, t_icmp request) {
    // INIT
    msg->buffer_size = request.packet_size;
    msg->iov_len = 1;
    msg->buffer = ft_calloc(1, request.packet_size);
    msg->iov = ft_calloc(1, msg->iov_len);
    // IOV
    msg->iov[0].iov_base = msg->buffer;
    msg->iov[0].iov_len = msg->buffer_size;
    msg->header.msg_iov = msg->iov;
}


int main(int ac, char *av[]) {
    t_host          dest = {0};
    int             sockfd_raw = 0;
    t_icmp          echo_request = {0};

    check_input(ac);
    //// Build the address:
    // Should use SOCK_RAW because ICMP is a protocol with
    // no user interface. So it need special options.
    // AF_INET for IPV4 type addr
    if ((sockfd_raw = socket(_INET_FAM, SOCK_RAW, IPPROTO_UDP)) == -1)
        error_handle(0, "Failed to open socket [AF_INET, SOCK_RAW, IPPROTO_UDP]");
    host_lookup(av[1], &dest);
    signal(SIGINT, signal_handler);
    signal(SIGALRM, signal_handler);
    while (1) {
        // Make sure all buffers (especially checksum) are cleared.
        ft_bzero(&echo_request, sizeof(t_icmp));
        generate_data(&echo_request.data, &echo_request.data_size);
        fill_header(&echo_request.header, echo_request.data, echo_request.data_size);
        create_packet(&echo_request);
        
        // send()
        sendto(sockfd_raw, echo_request.packet, echo_request.packet_size, 0,
            dest.addr_info->ai_addr, dest.addr_info->ai_addrlen);
        
        alarm(1);
        recvmsg(sockfd_raw, &dest.message, );
        // loop();
        // recvmsg() -> non-blocking
        // ->processing returned packet or abscence

        if (echo_request.data)
            free(echo_request.data);
        if (echo_request.packet)
            free(echo_request.packet);
    }

    freeaddrinfo(dest.addr_info);
}