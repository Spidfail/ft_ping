#include "ft_ping.h"
#include <stdio.h>
#include <sys/socket.h>

int error_handle(int errnum, char *err_value) {
    switch (errnum) {
        case EX_NOHOST :
            fprintf(stderr, "%s %s: %s: Unknown host\n", ERROR_HEADER, ERROR_RESOLVE, err_value);
            break;
        case EX_USAGE :
            fprintf(stderr, "usage: %s\n", ERROR_USAGE);
            break;
        default :
            perror(err_value);
    }
    exit(errnum);
}

void    check_input(int ac) {
    if (ac != 2)
        error_handle(EX_USAGE, NULL);
}

struct addrinfo    *host_lookup(char *raw_addr) {
    struct  addrinfo hints = {0};
    struct  addrinfo *dest = NULL;

    //// Build the hints for network address and service translation:
    // ai_socktype is equal to zero to accept addres of any type of sockets.
    hints.ai_flags = 0;
    hints.ai_family = AF_INET;
    hints.ai_socktype = 0;
    hints.ai_protocol = IPPROTO_UDP;
    
    if (getaddrinfo(raw_addr, NULL, &hints, &dest) == -1)
        error_handle(EX_NOHOST, raw_addr);
    return dest;
}

int main(int ac, char *av[]) {
    int     raw_sockfd = 0;
    struct  addrinfo *dest = NULL;
    //t_icmp  icmp = {0};

    check_input(ac);
    //// Build the address:
    // Should use SOCK_RAW because ICMP is a protocol with
    // no user interface. So it need special options.
    // AF_INET for IPV4 type addr
    if ((raw_sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1)
        error_handle(0, "Failed to open socket [AF_INET, SOCK_RAW, IPPROTO_UDP]");
    dest = host_lookup(av[1]);
}