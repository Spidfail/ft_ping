#include "ft_ping.h"

// Transform the input in a network byte order form if it's a valid address
int     interface_lookup(struct sockaddr_in *addr, const char *raw_addr, uint16_t port) {
    if (!addr || !raw_addr)
        return EXIT_FAILURE;
    // If the address is valid
    if (inet_pton(AF_INET, raw_addr, &addr->sin_addr) == 0) {
        addr->sin_port = htons(port);
        addr->sin_family = AF_INET;
        bzero(addr->sin_zero, sizeof(struct sockaddr));
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

int     interface_id(ifa_flag_t *flag, t_ifid target, t_ife type) {
    struct ifaddrs  *if_list = NULL;

    if (getifaddrs(&if_list) == -1)
        error_handle(0, "Fatal error, cannot recover network interfaces");
    switch (type) {
        case _IFE_NAME:
            for (struct ifaddrs *it = if_list ; it != NULL ; it = it->ifa_next) {
                if (ft_strcmp(target.name_str, it->ifa_name) == 0) {
                    *flag = it->ifa_flags;
                    freeifaddrs(if_list);
                    return EXIT_SUCCESS;
                }
            }
            break;
        case _IFE_ADDR:
            for (struct ifaddrs *it = if_list ; it ; it = it->ifa_next) {
                if (ft_memcmp(target.address, it->ifa_addr, sizeof(struct sockaddr)) == 0) {
                    *flag = it->ifa_flags;
                    freeifaddrs(if_list);
                    return EXIT_SUCCESS;
                }
            }
            break;
        case _IFE_MASK:
            for (struct ifaddrs *it = if_list ; it ; it = it->ifa_next) {
                if (ft_memcmp(target.mask, it->ifa_netmask, sizeof(struct sockaddr)) == 0) {
                    *flag = it->ifa_flags;
                    freeifaddrs(if_list);
                    return EXIT_SUCCESS;
                }
            }
            break;
    }
    freeifaddrs(if_list);
    return UINT_MAX;
}