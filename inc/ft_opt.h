
#ifndef FT_OPT_H
# define FT_OPT_H

#include <libft.h>
#include <stdbool.h>
#include <sys/time.h>
#include <stdio.h>
#include <argp.h>

/////////////////////// Enum
/// implemented (augmented): -I -S
/// -h -v -c -i -f -l -n -w -W -p -r -s -T --ttl --ip-timestamp
typedef enum    e_opt_list {
    // _OPT_h,
    _OPT_c,
    _OPT_i, // <arg>
    _OPT_n,
    _OPT_r,
    _OPT_TTL,
    _OPT_T, // <arg>
    _OPT_v,
    _OPT_w, // <arg>
    _OPT_W, // <arg>
    _OPT_f,
    _OPT_IPTIMESTAMP,
    _OPT_l, // <arg>
    _OPT_p, // <arg>
    _OPT_q,
    _OPT_s, // <arg>
    _OPT_I,
    _OPT_S, // <arg>
    _OPT_MAX_NB
}               t_opt_e;

// DEFINE
#define _OPT_ARG_MAX_NB 11
#define _OPT_DATA_LEN _OPT_MAX_NB + _OPT_ARG_MAX_NB

// STRUCT
typedef struct  s_opt_data {
    bool            opt[_OPT_MAX_NB];
    char            *opt_arg[_OPT_MAX_NB];
    char            *addr_raw;
    int             count;
    int             interval;
    bool            numeric;
    bool            ignore_routing;
    int             ttl;
    int             tos;
    int             timeout;
    struct timeval  timeout_timeval;
    int             linger;
    int             flood;
    int             iptimestamp;
    int             preload;
    int             pattern;
    int             quiet;
    int             size;
    char            *interface;
    int             sndbuf;
}               t_opt_d;

////////////////// Static Struct
/// implemented (inetutils-v2): -h -v -f -w -W -n -TTL
/// implemented (augmented): -I -S
/// -h -v -c -i -f -l -n -w -W -p -r -s -T --ttl --ip-timestamp
static struct argp_option  options[] = {
    // Already implemented by `argp`
    // { "help", '?', 0, 0, "give this help list", 0},
    // { "version", 'V', 0, 0, "print program version", 0},
    { "count", 'c', "NUMBER", 0, "stop after sending NUMBER packets", 0},
    { "interval", 'i', "NUMBER", 0, "wait NUMBER seconds between sending each packet", 0},
    { "numeric", 'n', 0, 0, "do not resolve host addresses", 0},
    { "ignore-routing", 'r',0, 0, "send directly to a host on an attached network", 0},
    { "ttl", _OPT_TTL, "NUMBER", 0, "specify N as time-to-live", 0},
    { "tos", 'T', "NUM", 0, "set type of service (TOS) to NUM", 0},
    { "verbose", 'v', 0, 0, "verbose output", 0},
    { "timeout", 'w', "N", 0, "stop after N seconds", 0},
    { "linger", 'W', "N", 0, "number of seconds to wait for response", 0},
    { "flood", 'f',0, 0, "flood ping (root only)", 0},
    { "ip-timestamp", _OPT_IPTIMESTAMP, "FLAG", 0, "IP timestamp of type FLAG, which is one of \"tsonly\" and \"tsaddr\"", 0},
    { "preload",'l', "NUMBER", 0, "send NUMBER packets as fast as possible before falling into normal mode of behavior (root only)", 0},
    { "pattern", 'p', "PATTERN", 0, "fill ICMP packet with given pattern (hex)", 0},
    { "quiet", 'q', 0, 0, "quiet output", 0},
    { "size", 's', "SIZE", 0, "send NUMBER data octets", 0},
    // Augmented, not available in inetutils-v2
    { "interface", 'I',0, 0, "either interface name or address", 0},
    { "sendbuf", 'S',0, 0, "use <size> as SO_SNDBUF socket option value", 0},
    { 0 }
};

// FUNCTION
void            opt_store(char *arr[], int arr_size, t_opt_d *opt_data);
bool            get_opt(t_opt_e opt_value, t_opt_d *data);
char            *get_opt_arg(t_opt_e opt_value, t_opt_d *data);
void            opt_handle(t_opt_d *data);

#endif