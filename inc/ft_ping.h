
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
#include <ft_opt.h>


/////////////////////// Constants
#ifndef DEBUG
# define DEBUG false
#endif

#define _INET_FAM AF_INET
#define _IO_DEBIAN "ens33\0"

#define _IP_HDR_SIZE 20
#define _ICMP_HDR_SIZE 8
#define _PING_DATA_SIZE 56

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
    -w <timeout>      reply wait <timeout> in seconds\n\
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

#define __PRINT_PACKET_ERROR(recv_size, ip_addr, error_description) \
    printf("%lu bytes from %s: %s\n", \
    recv_size, ip_addr, error_description); \

#define __PRINT_HEADER_DATABYTE(name_can, name_ip, data_size, packet_size) \
    printf("PING %s (%s) %lu(%lu) data bytes", \
    name_can, name_ip, data_size, packet_size); \

#define __PRINT_HEADER_VERBOSE(indent) \
    printf(", id 0x%04x = %u", indent, indent);

#define __PRINT_HEADER_BEG_IF(name_can, name_ip, data_size, packet_size, ip_local, interface) \
    printf("PING %s (%s) from %s %s: %lu(%lu) data bytes", \
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

/////////////////////// ERROR CODES

typedef struct   s_recv_error_type_code {
    int type;
    int code;
    char *description;
}               t_recv_error;

#define ICMP_ERROR_MAXNB 17
static t_recv_error     g_icmp_error[18] = {
    {ICMP_DEST_UNREACH, ICMP_NET_UNREACH, "Destination Net Unreachable"},
    {ICMP_DEST_UNREACH, ICMP_HOST_UNREACH, "Destination Host Unreachable"},
    {ICMP_DEST_UNREACH, ICMP_PROT_UNREACH, "Destination Protocol Unreachable"},
    {ICMP_DEST_UNREACH, ICMP_PORT_UNREACH, "Destination Port Unreachable"},
    {ICMP_DEST_UNREACH, ICMP_FRAG_NEEDED, "Fragmentation needed and DF set"},
    {ICMP_DEST_UNREACH, ICMP_SR_FAILED, "Source Route Failed"},
    {ICMP_DEST_UNREACH, ICMP_NET_UNKNOWN, "Network Unknown"},
    {ICMP_DEST_UNREACH, ICMP_HOST_UNKNOWN, "Host Unknown"},
    {ICMP_DEST_UNREACH, ICMP_HOST_ISOLATED, "Host Isolated"},
    {ICMP_DEST_UNREACH, ICMP_NET_UNR_TOS, "Destination Network Unreachable At This TOS"},
    {ICMP_DEST_UNREACH, ICMP_HOST_UNR_TOS, "Destination Host Unreachable At This TOS"},
    {ICMP_REDIRECT, ICMP_REDIR_NET, "Redirect Network"},
    {ICMP_REDIRECT, ICMP_REDIR_HOST, "Redirect Host"},
    {ICMP_REDIRECT, ICMP_REDIR_NETTOS, "Redirect Type of Service and Network"},
    {ICMP_REDIRECT, ICMP_REDIR_HOSTTOS, "Redirect Type of Service and Host"},
    {ICMP_TIME_EXCEEDED, ICMP_EXC_TTL, "Time to live exceeded"},
    {ICMP_TIME_EXCEEDED, ICMP_EXC_FRAGTIME, "Frag reassembly time exceeded"},
    { 0 }
};

/////////////////////// Typedef
typedef uint    ifa_flag_t;
#define IFAFLAG_NULL 0


/////////////////////// Structs
///

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

typedef struct  s_sequence_sum {
    const t_packet      *send;
    t_packet            recv;
    t_host              sender;
    ssize_t             recv_size;
    double              time_enlapsed_ms;
}               t_seq;

struct s_time_metrics {
    double          time_delta;
    double          time_min;
    double          time_max;
    struct timeval  time_start;
    struct timeval  time_end;
};

typedef struct  s_session_sum {
    bool                    active;
    t_host                  dest;
    t_packet                packet;
    t_seq                   sequence;
    int                     sockfd;
    uint16_t                pid;
    size_t                  recv_number;
    size_t                  err_number;
    uint16_t                seq_number;
    struct s_time_metrics   time;
}               t_sum;

typedef struct  s_ping {
    t_arg_d     args;
    t_list      *session;
    uint16_t    pid;
}               t_ping;


extern t_ping   g_ping;
extern bool     g_stop;


int     error_handle(int errnum, char *err_value);
void    error_gai_handle(char *input, int8_t ec);
char    *error_icmp_mapping(int type, int code);

int     interface_id(ifa_flag_t *flag, t_ifid target, t_ife type);
int     interface_lookup(struct sockaddr_in *addr, const char *raw_addr, uint16_t port);

// void    signal_handler(int sig);

t_sum       *session_new(uint16_t pid, int sockfd, char *raw_addr, void (*datagram_generate)(t_packet *, uint16_t));
t_list      *session_init_all(uint16_t pid, const t_list *hosts, const t_arg_d *args_data);
t_list      *session_end(t_list **sessions);
void        session_print_sum(t_sum *session);

int         packet_send(int sockfd, const t_host *dest, const t_packet *packet);
int         packet_receive(int sockfd, t_seq *sequence);
void        packet_print(const t_seq *sequence, float time_enlapsed);
void        packet_cpy(t_packet *target, const t_packet *source);
uint16_t    packet_checksum_calculate(const char *buffer, size_t size);
int         packet_verify_headers(const t_seq *sequence, uint8_t type, uint8_t code);

void        host_lookup(t_host *host, char *raw_addr, bool do_resolution);
void        host_get_ip(struct sockaddr *addr_buff);

void        ping_datagram_generate(t_packet *packet, uint16_t seq_number);
uint16_t    ping_datagram_checksum(struct icmphdr *header, const char *data, size_t size);

int     socket_init(int domain, int type, int protocol, const t_arg_d *arg_data);

void    interrupt(int exit_nb);


#endif // FT_PING_H ///////////////////////////////////////////////////////////