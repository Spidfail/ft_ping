
#ifndef FT_PING_H /////////////////////////////////////////////////////////////
# define FT_PING_H
#include <netinet/in.h>
#include <stddef.h>
#include <sysexits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include <signal.h>
#include <sys/time.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
// struct iphdr ip_hdr;
// struct icmphdr icmp_hdr;

#include "./libft_extended/libft.h"

#define _INET_FAM AF_INET

#define _ERROR_HEADER "ping:"
#define _ERROR_RESOLVE "cannot resolve"
#define _ERROR_USAGE "ping [-AaDdfnoQqRrv] [-c count] [-G sweepmaxsize] \
            [-g sweepminsize] [-h sweepincrsize] [-i wait] \
            [-l preload] [-M mask | time] [-m ttl] [-p pattern] \
            [-S src_addr] [-s packetsize] [-t timeout][-W waittime] \
            [-z tos] host \
       ping [-AaDdfLnoQqRrv] [-c count] [-I iface] [-i wait] \
            [-l preload] [-M mask | time] [-m ttl] [-p pattern] [-S src_addr] \
            [-s packetsize] [-T ttl] [-t timeout] [-W waittime] \
            [-z tos] mcast-group \
Apple specific options (to be specified before mcast-group or host like all options) \
            -b boundif           # bind the socket to the interface \
            -k traffic_class     # set traffic class socket option \
            -K net_service_type  # set traffic class socket options \
            --apple-connect       # call connect(2) in the socket \
            --apple-time          # display current time"

typedef char*   t_packet;

typedef struct  s_message {
    struct msghdr       header;
    char                *buffer;
    size_t              buffer_size;
    struct iovec        *iov;
    size_t              iov_len;
}               t_message;

typedef struct  s_host {
    struct addrinfo     *addr_info;
    struct sockaddr_in  *addr_netbo;
    char                addr_str[30];
    t_message           message;
}               t_host;

typedef struct  s_icmp {
    struct icmphdr  header;
    char            *data;
    size_t          data_size;
    t_packet        packet;
    size_t          packet_size;
}               t_icmp;

bool    g_interrupt = false;
bool    g_tick = false;

// ICMP_DATAGRAM
void        generate_data(char **data, size_t *size);
void        fill_header(struct icmphdr *header, char *data, size_t data_size);
void        create_packet(t_icmp *icmp_dtg);

// ICMP_CHECKSUM
uint16_t    calculate_checksum_icmp(struct icmphdr header, char *data, size_t size);

// ERROR_HANDLING
int         error_handle(int errnum, char *err_value);

// HOST
void        host_lookup(char *raw_addr, t_host *host);


#endif // FT_PING_H ///////////////////////////////////////////////////////////
