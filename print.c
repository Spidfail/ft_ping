#include "ft_ping.h"

void     print_packet(const t_icmp *echo_request, const t_sum *session) {
    char    addr_str[INET_ADDRSTRLEN];
    
    bzero(addr_str, INET_ADDRSTRLEN);
    if (NULL == inet_ntop(_INET_FAM,
            &echo_request->packet->ip_hdr.ip_src,
            addr_str,
            INET_ADDRSTRLEN)) {
        fprintf(stderr, "%s Cannot resolve ip address of a received packet: %s\n",
                _ERROR_HEADER, strerror(errno));
    }
        
    __PRINT_PACKET(_ICMP_HDR_SIZE + echo_request->data_size,
        addr_str,
        echo_request->packet->icmp_hdr.un.echo.sequence,
        echo_request->packet->ip_hdr.ip_ttl,
        session->time_enlapsed);
}

void    print_packet_error(const t_icmp *echo_request, uint8_t et, uint8_t ec) {
    char    addr_str[INET_ADDRSTRLEN];

    bzero(addr_str, INET_ADDRSTRLEN);
    if (NULL == inet_ntop(_INET_FAM,
            &echo_request->packet->ip_hdr.ip_src,
            addr_str,
            INET_ADDRSTRLEN)) {
        fprintf(stderr, "%s Cannot resolve ip address of a received packet: %s\n",
                _ERROR_HEADER, strerror(errno));
    }
    switch (et) {
        case ICMP_TIME_EXCEEDED:
            if (ec == ICMP_EXC_TTL)
                printf("From %s: icmp_seq=%i Time to live exceeded\n", \
                    addr_str, echo_request->header.un.echo.sequence);
            break;
        default :
            printf("From %s: icmp_seq=%i TYPE=%i CODE=%i\n", \
                addr_str, echo_request->header.un.echo.sequence, echo_request->packet->icmp_hdr.type, echo_request->packet->icmp_hdr.code);
    }
}

void    print_header_begin(int sockfd, const t_host *dest, const t_icmp *request, t_opt_d* const opt_data) {
    // If the resolution has been done (by default )
    if (!opt_data->opt[_OPT_I]) {
        if (dest->addr_info->ai_canonname)
            __PRINT_HEADER_BEG(dest->addr_info->ai_canonname, dest->addr_str, request->data_size, request->packet_size)
        else
            __PRINT_HEADER_BEG(dest->addr_orig, dest->addr_str, request->data_size, request->packet_size)
    }
    else {
        char                host_addr[INET_ADDRSTRLEN];
        struct ifreq        ifr = {0};

        // The interface name is hardcoded for now since I don't know how the real ping
        // find the interface address other than specified as an option.
        ft_memcpy(ifr.ifr_ifrn.ifrn_name, "ens33\0", 6);
        if (ioctl(sockfd, SIOCGIFADDR, &ifr) == -1)
            error_handle(0, "Fatal error while recovering network interface IP");
        if (inet_ntop(AF_INET, &((struct sockaddr_in*)&ifr.ifr_ifru.ifru_addr)->sin_addr, host_addr, INET_ADDRSTRLEN) == NULL)
            error_handle(0, "Fatal error, host ip is not valide");
        if (dest->addr_info->ai_canonname)
            __PRINT_HEADER_BEG_IF(dest->addr_info->ai_canonname, dest->addr_str, request->data_size, request->packet_size, host_addr, opt_data->opt_arg[_OPT_I])
        else
            __PRINT_HEADER_BEG_IF(dest->addr_orig, dest->addr_str, request->data_size, request->packet_size, host_addr, opt_data->opt_arg[_OPT_I])
    }
}

void    print_sum(t_sum *sumup, t_host *dest) {
    // Calculate the enlapsed time at the end to avoid
    // making a calcul at each turn.
    if (dest == NULL || dest->addr_info == NULL)
        return ;
    if (dest->addr_info->ai_canonname) {
        __PRINT_SUM(dest->addr_info->ai_canonname,
            sumup->seq_number,
            sumup->recv_number,
            sumup->err_number,
            get_enlapsed_ms(&sumup->time_start, &sumup->time_end))
    }
    else {
        __PRINT_SUM(dest->addr_orig,
            sumup->seq_number,
            sumup->recv_number,
            sumup->err_number,
            get_enlapsed_ms(&sumup->time_start, &sumup->time_end))
    }
    if (sumup->recv_number > 3)
        __PRINT_RTT(sumup->time_min,
            sumup->time_delta,
            sumup->time_max,
            sumup->recv_number)
}