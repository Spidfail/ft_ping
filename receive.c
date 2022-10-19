#include "ft_ping.h"

int    receive_data(int sockfd, t_icmp *echo_request) {
    struct msghdr       header;
    struct iovec        iov[1];
    char                name_buff[100];
    
    bzero(&name_buff, 100);
    bzero(&header, sizeof(struct msghdr));
    bzero(iov, sizeof(struct iovec));
    if (echo_request->packet == NULL)
        error_handle(0, "Error : recv_buffer not set !");
    iov[0].iov_len = echo_request->packet_size;
    iov[0].iov_base = echo_request->packet;
    header.msg_iovlen = 1;
    header.msg_iov = iov;
    header.msg_name = name_buff;
    header.msg_namelen = 100;

    // Important for printing inforomations and record errors
    echo_request->received_size = recvmsg(sockfd, &header, 0);
    if (echo_request->received_size == -1)
        return EXIT_FAILURE;

    // Store the message in a proper packet struct
    ft_memcpy(echo_request->packet, header.msg_iov->iov_base, header.msg_iov->iov_len);

    if (verify_ip_header((struct ip *)echo_request->packet) == EXIT_FAILURE ||
        verify_icmp_header(echo_request->packet, echo_request) == EXIT_FAILURE) {
        dprintf(2, "!!!!!!!!!!!!! ERROR VERIFY !\n");
    }
    return EXIT_SUCCESS;
}