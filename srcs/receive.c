#include <ft_ping.h>
#include <asm-generic/errno-base.h>
#include <stdlib.h>
#include <string.h>

int    receive_data(int sockfd, t_icmp *echo_request, t_sum *session) {
    struct msghdr       header;
    struct iovec        iov[1];
    char                name_buff[100];
    
    bzero(&header, sizeof(struct msghdr));
    bzero(iov, sizeof(struct iovec));
    bzero(&name_buff, 100);
    if (echo_request->packet == NULL)
        error_handle(0, "Error : recv_buffer not set !");
    iov[0].iov_len = echo_request->packet_size;
    iov[0].iov_base = echo_request->packet;
    header.msg_iovlen = 1;
    header.msg_iov = iov;
    header.msg_name = name_buff;
    header.msg_namelen = 100;

    echo_request->received_size = recvmsg(sockfd, &header, 0);
    //// Even if recvmsg() failed, we must save these data.
    // Store the last time a packet arrived, used as the received_time
    if (gettimeofday(&session->time_end, NULL) == -1)
        error_handle(0, "Fatal: Failed to recover time");
    if (echo_request->received_size == -1) {
        if (errno == EINTR)
            return EXIT_SUCCESS;
        update_time(session, (struct timeval *)echo_request->data,
                    &session->time_end);
        ++session->err_number;
        return EXIT_FAILURE;
    }
    // Update all values here instead of making calculations and attribution
    // anywhere else. The total enlapsed time is calculating at the end.
    update_time(session, (struct timeval *)echo_request->data, &session->time_end);

    // Store the message in a proper packet struct
    if (ft_memcpy(echo_request->packet, header.msg_iov->iov_base, header.msg_iov->iov_len) == NULL)
        error_handle(0, "Fatal: memcpy failed");
    // Verify checksum and ICMP header content
    else if (verify_ip_header((struct ip *)echo_request->packet) == EXIT_FAILURE) {
        ++session->err_number;
        return EXIT_FAILURE;
    }
    else if (verify_icmp_header(echo_request->packet, echo_request) == EXIT_FAILURE) {
        ++session->err_number;
        return EXIT_FAILURE;
    }
    ++session->recv_number;
    return EXIT_SUCCESS;
}