
#ifndef FT_PING_H /////////////////////////////////////////////////////////////
# define FT_PING_H
#include <bits/types/struct_timeval.h>
#include <stddef.h>
#include <stdint.h>
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
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <errno.h>
#include <error.h>

#include "./libft_extended/libft.h"


/////////////////////// Constants
#ifndef DEBUG
# define DEBUG false
#endif

#define _INET_FAM AF_INET

#define _IP_HDR_SIZE 20
#define _ICMP_HDR_SIZE 8
#define _PING_DATA_SIZE 56

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

/////////////////////// Macros
#define __PRINT_PACKET(recv_size, ip_addr, seq, ttl, ms) \
    printf("%lu bytes from %s: icmp_seq=%i ttl=%i time=%lu ms\n", \
    recv_size, ip_addr, seq, ttl, ms); \

#define __PRINT_HEADER_BEG(name_can, name_ip, data_size, packet_size) \
    printf("PING %s (%s) %lu(%lu) bytes of data.\n", \
    name_can, name_ip, data_size, packet_size); \

#define __PRINT_SUM(name_canon, nb_sent, nb_recv, nb_errors, overall_time) \
    write(1, "\n", 1); \
    printf("--- %s ping statistics ---\n%i pachets transmitted, %lu received, ", name_canon, nb_sent, nb_recv); \
    if (nb_errors != 0) \
        printf("+%lu errors", nb_errors); \
    printf("%lu%% packet loss, time %lims\n", (((nb_sent - nb_recv) / nb_sent) * 100), overall_time); \

typedef struct  s_recv_buff {
    struct msghdr       header;
    char                *buffer;
    size_t              buffer_size;
    struct iovec        iov[1];
}               t_recv_buff;

typedef struct s_packet {
    struct ip       ip_hdr;
    struct icmphdr  icmp_hdr;
    char            data[_PING_DATA_SIZE];
}               t_packet;

typedef struct  s_host {
    struct addrinfo     *addr_info;
    char                addr_str[INET_ADDRSTRLEN];
}               t_host;

typedef struct  s_icmp {
    struct icmphdr  header;
    char            *data;
    size_t          data_size;
    char            *datagram;
    size_t          datagram_size;
    t_packet        *packet;
    size_t          packet_size;
    ssize_t         received_size;
    struct timeval  received_time;
    bool            is_packet;
}               t_icmp;

typedef struct  s_session_sum {
    uint16_t        seq_number;
    size_t          recv_number;
    size_t          err_number;
    struct timeval  start;
}               t_sum;

typedef struct  s_global_data {
    t_icmp          echo_request;
    t_host          dest_spec;
    t_sum           session;
    int             sockfd;
}               t_global;

    
/////////////////////// Global Variables
extern t_global    g_data;

// ICMP_CHECKSUM
uint16_t    calculate_checksum_icmp(struct icmphdr header, const char *data, size_t size);
uint16_t    calculate_checksum(const char *buffer, size_t size);

// ERROR_HANDLING
int         error_handle(int errnum, char *err_value);

// HOST
void        host_lookup(char *raw_addr, t_host *host);

// VERIFY
int         verify_ip_header(const struct ip *reception);
int         verify_icmp_header(const t_packet *packet, const t_icmp *src);

// RECEIVE
int         receive_data(int sockfd, t_icmp *echo_request);

// ICMP_DATAGRAM
int         send_new_packet(int sockfd, t_icmp *echo_request, t_host *dest, const t_sum *session);

// PRINT
void        print_packet(t_icmp *echo_request);
void        print_header_begin(const t_host *dest, const t_icmp *request);
void        print_sum(t_sum *sumup, t_host *dest);
ssize_t     get_enlapsed_ms(const struct timeval *start, const struct timeval *end);

// DATA
void        init_data(t_icmp *echo_request, t_sum *session);
void        clean_data(t_icmp *echo_request);
void        free_data(t_icmp *echo_request);

// SIGNAL
void        signal_handler(int sig);
void        handle_tick();

#endif // FT_PING_H ///////////////////////////////////////////////////////////