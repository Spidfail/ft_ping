#include <ft_opt.h>
#include <ft_ping.h>

static int     socket_set_options(const int *sockfd, t_arg_d *arg_data) {
    if (arg_data->interface)
        if (setsockopt(*sockfd, SOL_SOCKET, SO_BINDTODEVICE,
                       arg_data->interface,
                       ft_strlen(arg_data->interface)) == -1)
          error_handle(0, "SO_BINDTODEVICE");

    if (arg_data->linger)
        if (setsockopt(*sockfd, SOL_SOCKET, SO_RCVTIMEO_NEW,
                       &arg_data->linger, sizeof(struct timeval)) == -1)
          error_handle(0, "SO_RCVTIMEO");
    if (arg_data->sndbuf)
          if (setsockopt(*sockfd, SOL_SOCKET, SO_SNDBUF, &arg_data->sndbuf,
                         sizeof(int)) == -1)
            error_handle(0, "SO_SNDBUF");
    if (arg_data->ttl)
          if (setsockopt(*sockfd, IPPROTO_IP, IP_TTL, &arg_data->ttl,
                         sizeof(int)) == -1)
            error_handle(0, "IP_TTL");
    return EXIT_SUCCESS;
}

void            socket_init(int *sockfd, t_arg_d *arg_data) {
    if (sockfd == NULL)
        error_handle(0, "BUG : Please give us a valid sockfd pointer...\n");
    //// Build the address:
    // Should use SOCK_RAW because ICMP is a protocol with
    // no user interface. So it need special options.
    // AF_INET for IPV4 type addr.
    if ((*sockfd = socket(_INET_FAM, SOCK_RAW, IPPROTO_ICMP)) == -1)
        error_handle(0, "Failed to open socket [AF_INET, SOCK_RAW, IPPROTO_ICMP]");
      socket_set_options(sockfd, arg_data);
}