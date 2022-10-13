#include "ft_ping.h"

int    receive_data(int sockfd, t_icmp *echo_request) {
    struct msghdr       header;
    struct iovec        iov[1];
    ssize_t             rtn = 0;

    if (echo_request->packet == NULL)
        error_handle(0, "Error : recv_buffer not set !");
    iov[0].iov_len = echo_request->packet_size;
    iov[0].iov_base = echo_request->packet;
    header.msg_iovlen = 1;
    header.msg_iov = iov;

    rtn = recvmsg(sockfd, &header, 0);
    dprintf(2, "RECVMSG = %lu\n", rtn);
    if (rtn == -1)
        return EXIT_FAILURE;

    // Store the message in a proper packet struct
    ft_memcpy(echo_request->packet, header.msg_iov->iov_base, header.msg_iov->iov_len);

    if (verify_ip_header((struct ip *)echo_request->packet) == EXIT_FAILURE ||
        verify_icmp_header(echo_request->packet, echo_request) == EXIT_FAILURE) {
        dprintf(2, "!!!!!!!!!!!!! ERROR VERIFY !\n");
    }
    return EXIT_SUCCESS;
}