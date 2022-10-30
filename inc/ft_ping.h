
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
#include <float.h>

#include <signal.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include <net/if.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <errno.h>
#include <error.h>
#include <ifaddrs.h>

#include <libft.h>


/////////////////////// Constants
#ifndef DEBUG
# define DEBUG false
#endif

#define _INET_FAM AF_INET

#define _IP_HDR_SIZE 20
#define _ICMP_HDR_SIZE 8
#define _PING_DATA_SIZE 56

#define _OPT_MAX_NB 15
#define _OPT_ARG_MAX_NB 11
#define _OPT_DATA_LEN _OPT_MAX_NB + _OPT_ARG_MAX_NB

#define _EX_NOERRNO -1

#define _ERROR_HEADER "ping:"
#define _ERROR_RESOLVE "cannot resolve"
/// OPT list : < -h -v -f -l -I -m -M -n -w -W -p -Q -S -t -T >
/// Only the first two are mandatory
#define _ERROR_USAGE "Destination address required"
#define _HEADER_USAGE "Usage\n\
    ping [options] <destination>\n\n\
Options:\n\
    <destination>      dns name or ip address\n\
    -h                 print help and exit\n\
    -v                 verbose output\n\
    -f                 flood ping\n\
    -l <preload>       send <preload> number of packages while waiting replies\n\
    -I <interface>     either interface name or address\n\
    -m <mark>          tag the packets going out\n\
    -M <pmtud opt>     define mtu discovery, can be one of <do|dont|want>\n\
    -n                 no dns name resolution\n\
    -w <deadline>      reply wait <deadline> in seconds\n\
    -W <timeout>       time to wait for response\n\
    -p <pattern>       contents of padding byte\n\
    -Q <tclass>        use quality of service <tclass> bits\n\
    -S <size>          use <size> as SO_SNDBUF socket option value\n\
    -t <ttl>           define time to live\n\
IPv4 options:\n\
    -T <timestamp>     define timestamp, can be one of <tsonly|tsandaddr|tsprespec>\n\
"
#define _ERROR_INTERFACE_SRCADDR "Warning: source address might be selected on device other than: "
#define _ERROR_INTERFACE_IFACE "unknown iface: "
#define _ERROR_INTERFACE_NOSUCHDEVICE "SO_BINDTODEVICE "


/////////////////////// Macros
#define __PRINT_PACKET(recv_size, ip_addr, seq, ttl, ms) \
    printf("%lu bytes from %s: icmp_seq=%i ttl=%i time=%f ms\n", \
    recv_size, ip_addr, seq, ttl, ms); \

#define __PRINT_HEADER_BEG(name_can, name_ip, data_size, packet_size) \
    printf("PING %s (%s) %lu(%lu) bytes of data.\n", \
    name_can, name_ip, data_size, packet_size); \

#define __PRINT_HEADER_BEG_IF(name_can, name_ip, data_size, packet_size, ip_local, interface) \
    printf("PING %s (%s) from %s %s: %lu(%lu) bytes of data.\n", \
    name_can, name_ip,  ip_local, interface, data_size, packet_size); \

#define __PRINT_SUM(name_canon, nb_sent, nb_recv, nb_errors, overall_time) \
    write(1, "\n", 1); \
    printf("--- %s ping statistics ---\n%i packets transmitted, %lu received, ", name_canon, nb_sent, nb_recv); \
    if (nb_errors != 0) \
        printf("+%lu errors, ", nb_errors); \
    printf("%lu%% packet loss", ((nb_sent - nb_recv) / nb_sent) * 100); \
    if (overall_time > 0) \
        printf(", time %fms\n", overall_time); \
    else \
        printf("\n"); \
    
#define __PRINT_RTT(time_min, time_delta, time_max, recv_number) \
    printf("rtt min/avg/max = %f/%f/%f\n", time_min, (time_delta/(double)recv_number), time_max);


/////////////////////// Typedef

typedef uint    ifa_flag_t;
#define IFAFLAG_NULL 0


/////////////////////// Enum
/// OPT list : < -h -v -f -l -I -m -M -n -w -W -p -Q -S -t -T >
typedef enum    e_opt_list {
    _OPT_h,
    _OPT_v,
    _OPT_f,
    _OPT_l, // <arg>
    _OPT_I, // <arg>
    _OPT_m, // <arg>
    _OPT_M, // <arg>
    _OPT_n,
    _OPT_w, // <arg>
    _OPT_W, // <arg>
    _OPT_p, // <arg>
    _OPT_Q, // <arg>
    _OPT_S, // <arg>
    _OPT_t, // <arg>
    _OPT_T, // <arg>
}               t_opt_e;

/////////////////////// Structs
typedef struct  s_opt_data {
    bool            opt[_OPT_MAX_NB];
    char            *opt_arg[_OPT_MAX_NB];
    struct timeval  timeout;
    int             deadline;
    int             ttl;
    int             sndbuf;
}               t_opt_d;

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
    char                *addr_orig;
    struct addrinfo     *addr_info;
    char                addr_str[INET_ADDRSTRLEN];
}               t_host;

typedef union   u_interfaces_id {
    const char            *name_str;
    struct sockaddr       *address;
    struct sockaddr       *mask;
}               t_ifid;

typedef enum   e_interface_enum {
    _IFE_NAME,
    _IFE_ADDR,
    _IFE_MASK
}               t_ife;

typedef struct  s_icmp {
    struct icmphdr  header;
    char            *data;
    size_t          data_size;
    char            *datagram;
    size_t          datagram_size;
    t_packet        *packet;
    size_t          packet_size;
    ssize_t         received_size;
    bool            is_packet;
}               t_icmp;

typedef struct  s_session_sum {
    uint16_t        seq_number;
    size_t          recv_number;
    size_t          err_number;
    double          time_delta;
    double          time_min;
    double          time_max;
    double          time_enlapsed;
    struct timeval  time_end;
    struct timeval  time_start;
}               t_sum;

typedef struct  s_global_data {
    t_icmp          echo_request;
    t_host          dest_spec;
    t_sum           session;
    int             sockfd;
    t_opt_d         opt;
}               t_global;


    
/////////////////////// Global Variables
extern t_global    g_data;

// ICMP_CHECKSUM
uint16_t    calculate_checksum_icmp(struct icmphdr header, const char *data, size_t size);
uint16_t    calculate_checksum(const char *buffer, size_t size);

// ERROR_HANDLING
int         error_handle(int errnum, char *err_value);
void        error_gai_handle(char *input, int8_t ec);

// HOST
void        host_lookup(char *raw_addr, t_host *host, bool do_resolution);
void        host_get_ip(struct sockaddr *addr_buff);

// INTERFACE
int         interface_lookup(struct sockaddr_in *addr, const char *raw_addr, uint16_t port);
int         interface_id(ifa_flag_t *flag, t_ifid target, t_ife type);

// VERIFY
int         verify_ip_header(const struct ip *reception);
int         verify_icmp_header(const t_packet *packet, const t_icmp *src);

// RECEIVE
int         receive_data(int sockfd, t_icmp *echo_request, t_sum *session);

// ICMP_DATAGRAM
int         send_new_packet(int sockfd, t_icmp *echo_request, t_host *dest, const t_sum *session);

// PRINT
void        print_packet(const t_icmp *echo_request, const t_sum *session);
void        print_packet_error(const t_icmp *echo_request, uint8_t et, uint8_t ec);
void        print_header_begin(int sockfd, const t_host *dest, const t_icmp *request, t_opt_d* const opt_data);
void        print_sum(t_sum *sumup, t_host *dest);

// DATA
void        init_data(t_icmp *echo_request, t_sum *session);
void        clean_data(t_icmp *echo_request);
void        free_data(t_icmp *echo_request, t_opt_d *opt_data);

// SIGNAL HANDLER
void        signal_handler(int sig);
void        handle_tick();
void        new_load();
void        interrupt(int exit_nb);

// TIME
double      get_enlapsed_ms(const struct timeval *start, const struct timeval *end);
void        update_time(t_sum *session, const struct timeval *start, const struct timeval *end);

// OPTIONS
void            opt_store(char *arr[], int arr_size, t_opt_d *opt_data, int *addr_pos);
bool            get_opt(t_opt_e opt_value, t_opt_d *data);
char            *get_opt_arg(t_opt_e opt_value, t_opt_d *data);
void            opt_handle(t_opt_d *data);

// SOCKET
void            socket_init(int *sockfd, t_opt_d *opt_data);

// LOOP
void            loop_flood();
void            loop_classic();

#endif // FT_PING_H ///////////////////////////////////////////////////////////