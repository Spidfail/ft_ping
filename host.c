#include "ft_ping.h"

static bool     is_addr(char *addr) {
    if (addr == NULL || addr[0] == '-')
        return false;
    return true;
}

void    host_lookup(char *raw_addr, t_host *host, bool do_resolution) {
    struct  addrinfo hints = {0};
    int8_t  rtn_gai = 0;

    if (!is_addr(raw_addr))
        error_handle(EX_USAGE, NULL);
    //// Build the hints for network address and service translation:
    // ai_socktype is equal to zero to accept addres of any type of sockets.
    if (do_resolution)
        hints.ai_flags = AI_CANONNAME;
    else
        hints.ai_flags = 0;
    hints.ai_family = _INET_FAM;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_ICMP;
    
    // Store the user input to use it in case of '-n' option
    // (no DNS lookup, numerical only)
    host->addr_orig = raw_addr;
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

void     host_get_ip(struct sockaddr *addr_buff) {
    char                host_name[100];
    struct addrinfo     *host_addr = NULL;
    int                 rtn = 0;

    bzero(host_name, 100);
    if (gethostname(host_name, 100) == -1)
        error_handle(0, "Fatal error, failed to retrieve the host name");
    rtn = getaddrinfo(host_name, NULL, NULL, &host_addr);
    if (rtn != 0)
        error_gai_handle(host_name, rtn);
    ft_memcpy(addr_buff, host_addr->ai_addr, sizeof(struct sockaddr));
    freeaddrinfo(host_addr);
}