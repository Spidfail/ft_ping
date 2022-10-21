#include "ft_ping.h"

void    host_lookup(char *raw_addr, t_host *host) {
    struct  addrinfo hints = {0};
    int8_t  rtn_gai = 0;
    //// Build the hints for network address and service translation:
    // ai_socktype is equal to zero to accept addres of any type of sockets.
    hints.ai_flags = AI_CANONNAME;
    hints.ai_family = _INET_FAM;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_ICMP;
    
    rtn_gai = getaddrinfo(raw_addr, NULL, &hints, &host->addr_info);
    if (rtn_gai != EXIT_SUCCESS)
        error_gai_handle(raw_addr, rtn_gai);
    
    // Get us the strange equivalent in `struct sockaddr_in`, only to have access
    // in an easier way of a `struct in_addr` format.
    if (inet_ntop(_INET_FAM,
            &((struct sockaddr_in *)host->addr_info->ai_addr)->sin_addr,
            host->addr_str, INET_ADDRSTRLEN) == NULL)
        error_handle(EX_NOHOST, raw_addr);
}