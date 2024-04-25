#include <ft_opt.h>
#include <ft_ping.h>

static int     socket_set_options(const int sockfd, const t_arg_d *arg_data) {
    if (arg_data->interface)
        if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE,
                       arg_data->interface,
                       ft_strlen(arg_data->interface)) == -1)
          error_handle(0, "SO_BINDTODEVICE");

    if (arg_data->linger)
        if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO_NEW,
                       &arg_data->linger, sizeof(struct timeval)) == -1)
          error_handle(0, "SO_RCVTIMEO");
    if (arg_data->sndbuf)
          if (setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &arg_data->sndbuf,
                         sizeof(int)) == -1)
            error_handle(0, "SO_SNDBUF");
    if (arg_data->ttl)
          if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &arg_data->ttl,
                         sizeof(int)) == -1)
            error_handle(0, "IP_TTL");
    return EXIT_SUCCESS;
}

int            socket_init(int domain, int type, int protocol, const t_arg_d *arg_data) {
    int fd = 0;
    //// Build the address:
    // Should use SOCK_RAW because ICMP is a protocol with
    // no user interface. So it need special options.
    // AF_INET for IPV4 type addr.
    if ((fd = socket(domain, type, protocol)) == -1)
        error_handle(0, "Failed to open socket [AF_INET, SOCK_RAW, IPPROTO_ICMP]");
    socket_set_options(fd, arg_data);
    return fd;
}