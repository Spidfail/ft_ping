#include "ft_ping.h"
#include <asm-generic/socket.h>

static int     socket_set_options(const int *sockfd, t_opt_e opt_num, t_opt_d *opt_data) {
    switch (opt_num) {
        case _OPT_h :
            break;
        case _OPT_v :
            break;
        case _OPT_f :
            break;
        case _OPT_l :
            break;
        case _OPT_I :
            break;
        case _OPT_m :
            break;
        case _OPT_M :
            break;
        case _OPT_n :
            break;
        case _OPT_w :
            break;
        case _OPT_W :
            if (setsockopt(*sockfd, SOL_SOCKET, SO_RCVTIMEO, &opt_data->timeout, sizeof(int)) == -1
                    || setsockopt(*sockfd, SOL_SOCKET, SO_SNDTIMEO, &opt_data->timeout, sizeof(int)) == -1)
                error_handle(0, "Failed to set socket option");
            break;
        case _OPT_p :
            break;
        case _OPT_Q :
            break;
        case _OPT_S :
            if (setsockopt(*sockfd, SOL_SOCKET, SO_SNDBUF, &opt_data->sndbuf, sizeof(int)) == -1)
                error_handle(0, "Failed to set socket option");
            break;
        case _OPT_t :
            if (setsockopt(*sockfd, IPPROTO_IP, IP_TTL, &opt_data->ttl, sizeof(int)) == -1)
                error_handle(0, "Failed to set socket option");
            break;
        case _OPT_T :
            break;
    }
    return EXIT_SUCCESS;
}

void            socket_init(int *sockfd, t_opt_d *opt_data) {
    if (sockfd == NULL)
        error_handle(0, "BUG : Please give us a valid sockfd pointer...\n");
    if ((*sockfd = socket(_INET_FAM, SOCK_RAW, IPPROTO_ICMP)) == -1)
        error_handle(0, "Failed to open socket [AF_INET, SOCK_RAW, IPPROTO_ICMP]");
    for (int i = 0 ; i < _OPT_MAX_NB ; ++i) {
        if (opt_data->opt[i])
            socket_set_options(sockfd, i, opt_data);
    }
}