#include <ft_ping.h>

static int     socket_set_options(const int *sockfd, int opt_num, t_opt_d *opt_data) {
    switch (opt_num) {
        case _OPT_I :
          if (setsockopt(*sockfd, SOL_SOCKET, SO_BINDTODEVICE,
                         opt_data->opt_arg[opt_num],
                         ft_strlen(opt_data->opt_arg[opt_num])) == -1)
            error_handle(0, "SO_BINDTODEVICE");
          break;
        case _OPT_W :
          if (setsockopt(*sockfd, SOL_SOCKET, SO_RCVTIMEO_NEW,
                         &opt_data->timeout, sizeof(struct timeval)) == -1)
            error_handle(0, "SO_RCVTIMEO");
          break;
        case _OPT_S :
          if (setsockopt(*sockfd, SOL_SOCKET, SO_SNDBUF, &opt_data->sndbuf,
                         sizeof(int)) == -1)
            error_handle(0, "SO_SNDBUF");
          break;
        case _OPT_t :
          if (setsockopt(*sockfd, IPPROTO_IP, IP_TTL, &opt_data->ttl,
                         sizeof(int)) == -1)
            error_handle(0, "IP_TTL");
          break;
    }
    return EXIT_SUCCESS;
}

void            socket_init(int *sockfd, t_opt_d *opt_data) {
    if (sockfd == NULL)
        error_handle(0, "BUG : Please give us a valid sockfd pointer...\n");
    //// Build the address:
    // Should use SOCK_RAW because ICMP is a protocol with
    // no user interface. So it need special options.
    // AF_INET for IPV4 type addr.
    if ((*sockfd = socket(_INET_FAM, SOCK_RAW, IPPROTO_ICMP)) == -1)
        error_handle(0, "Failed to open socket [AF_INET, SOCK_RAW, IPPROTO_ICMP]");
    for (int i = 0 ; i < _OPT_MAX_NB ; ++i) {
        if (opt_data->opt[i])
            socket_set_options(sockfd, i, opt_data);
    }
}