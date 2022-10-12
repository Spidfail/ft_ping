#include "ft_ping.h"

void    host_lookup(char *raw_addr, t_host *host) {
    struct  addrinfo hints = {0};

    //// Build the hints for network address and service translation:
    // ai_socktype is equal to zero to accept addres of any type of sockets.
    hints.ai_flags = 0;
    hints.ai_family = AF_INET;
    hints.ai_socktype = 0;
    hints.ai_protocol = IPPROTO_UDP;
    
    if (getaddrinfo(raw_addr, NULL, &hints, &host->addr_info) != EXIT_SUCCESS)
        error_handle(EX_NOHOST, raw_addr);
    host->addr_netbo = (struct sockaddr_in *)host->addr_info;

    // Clear the buffer in case of garbage
    ft_bzero(host->addr_str, 30);

    // Get us the strange equivalent in `struct sockaddr_in`, only to have access
    // in an easier way of a `struct in_addr` format.
    if (inet_ntop(_INET_FAM, &host->addr_netbo->sin_addr, host->addr_str, 30) == NULL)
        error_handle(EX_NOHOST, raw_addr);
}