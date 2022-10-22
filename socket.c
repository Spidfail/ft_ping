#include "ft_ping.h"
#include <asm-generic/socket.h>

static int     socket_set_options(const int *sockfd, t_opt_e opt_num, char *arg) {
    int     buff_int = 0;
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
            break;
        case _OPT_p :
            break;
        case _OPT_Q :
            break;
        case _OPT_S :
            if (!arg)
                error_handle(EX_USAGE, _HEADER_USAGE);
            buff_int = ft_atoi(arg);
            if (setsockopt(*sockfd, SOL_SOCKET, SO_SNDBUF, &buff_int, sizeof(buff_int)) == -1)
                error_handle(0, "Failed to set socket option");
            break;
        case _OPT_t :
            if (!arg)
                error_handle(EX_USAGE, _HEADER_USAGE);
            buff_int = ft_atoi(arg);
            if (setsockopt(*sockfd, IPPROTO_IP, IP_TTL, &buff_int, sizeof(buff_int)) == -1)
                error_handle(0, "Failed to set socket option");
            break;
        case _OPT_T :
            break;
    }
    return EXIT_SUCCESS;
}

void            socket_init(int *sockfd, t_opt_d *opt_data) {
    if (sockfd == NULL)
        error_handle(0, "BUG : Please give us a valid sockfd pointer...");
    if ((*sockfd = socket(_INET_FAM, SOCK_RAW, IPPROTO_ICMP)) == -1)
        error_handle(0, "Failed to open socket [AF_INET, SOCK_RAW, IPPROTO_ICMP]");
    for (int i = 0 ; i < _OPT_MAX_NB ; ++i) {
        if (opt_data->opt[i])
            socket_set_options(sockfd, i, opt_data->opt_arg[i]);
    }
}