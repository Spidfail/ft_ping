#include "ft_ping.h"
#include <stdio.h>
#include <stdlib.h>
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

int    host_lookup(char *raw_addr, struct addrinfo **save_addrinfo) {
    struct  addrinfo hints = {0};
    int     rtn = 0;

    //// Build the hints for network address and service translation:
    // ai_socktype is equal to zero to accept addres of any type of sockets.
    hints.ai_flags = 0;
    hints.ai_family = AF_INET;
    hints.ai_socktype = 0;
    hints.ai_protocol = IPPROTO_UDP;
    
    if ((rtn = getaddrinfo(raw_addr, NULL, &hints, save_addrinfo)) != EXIT_SUCCESS)
        error_handle(EX_NOHOST, raw_addr);
    return rtn;
}

int main(int ac, char *av[]) {
    struct addrinfo *dest = {0};
    int             raw_sockfd = 0;
    t_icmp  icmp = {0};

    check_input(ac);
    //// Build the address:
    // Should use SOCK_RAW because ICMP is a protocol with
    // no user interface. So it need special options.
    // AF_INET for IPV4 type addr
    if ((raw_sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1)
        error_handle(0, "Failed to open socket [AF_INET, SOCK_RAW, IPPROTO_UDP]");
    host_lookup(av[1], &dest);
    icmp.str_addr = inet_ntop();
    dprintf(2, "TEST ADDR = %s\n", dest->ai_addr->sa_data);
    freeaddrinfo(dest);
}