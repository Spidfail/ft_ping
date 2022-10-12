#include "ft_ping.h"
#include "libft.h"
#include <stddef.h>
#include <stdio.h>

bool    g_interrupt = false;
bool    g_tick = false;
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

void            message_rec_init(t_recv_buff *msg, t_icmp request) {
    // INIT -> get the ip header max packet size.
    msg->buffer_size = request.datagram_size + 40;
    msg->buffer = ft_calloc(1, msg->buffer_size);
    // IOV
    msg->iov[0].iov_base = msg->buffer;
    msg->iov[0].iov_len = msg->buffer_size;
    msg->header.msg_iov = msg->iov;
}

int main(int ac, char *av[]) {
    t_host          dest = {0};
    int             sockfd_raw = 0;
    t_icmp          echo_request = {0};
    t_recv_buff     recv_buff;
    t_packet        recv_packet = {0};


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
        ft_bzero(&recv_buff, sizeof(t_recv_buff));
        generate_data(&echo_request.data, &echo_request.data_size);
        fill_header(&echo_request.header, echo_request.data, echo_request.data_size);
        create_datagram(&echo_request);
        
        message_rec_init(&recv_buff, echo_request);

        // send() --> TODO : change the way it's done
        sendto(sockfd_raw, echo_request.datagram, echo_request.datagram_size, 0,
            dest.addr_info->ai_addr, dest.addr_info->ai_addrlen);
        // Setup the alarm to triger the new `send()`.
        alarm(1);

        if (recvmsg(sockfd_raw, &recv_buff.header, 0) == -1) {
            dprintf(2, "Error recvmsg!");
            return -1;
        }
        
        ////////////////////////////////////////////
        // Problem -> the iov array may contain a `struct cmsghdr`
        // and not a raw [ip - icmp - data] datagram

        // SO : may not be valid :
        //// Store the message in a proper packet struct
        //// ft_memcpy(&recv_packet, recv_buff.iov->iov_base, recv_buff.iov->iov_len);
        

        if (verify_ip_header((struct ip*)recv_buff.buffer) == EXIT_FAILURE
            || verify_icmp_header(&recv_packet, &echo_request) == EXIT_FAILURE) {
            dprintf(2, "ERROR VERIFY !\n");
        }

        // loop();
        //while (g_tick == false) { }

        // ->processing returned packet or abscence

        if (echo_request.data)
            free(echo_request.data);
        if (echo_request.datagram)
            free(echo_request.datagram);
    }

    freeaddrinfo(dest.addr_info);
}