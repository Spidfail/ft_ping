
#ifndef FT_OPT_H
# define FT_OPT_H

#include <stdbool.h>
#include <sys/time.h>

/////////////////////// Enum
/// -h -v -c -i -f -l -n -w -W -p -r -s -T --ttl --ip-timestamp
typedef enum    e_opt_list {
    _OPT_h,
    _OPT_v,
    _OPT_q,
    _OPT_c,
    _OPT_i,
    _OPT_f,
    _OPT_l, // <arg>
    _OPT_r, // <arg>
    _OPT_n,
    _OPT_w, // <arg>
    _OPT_W, // <arg>
    _OPT_p, // <arg>
    _OPT_s, // <arg>
    _OPT_t, // <arg>
    _OPT_T, // <arg>
    _OPT_TTL,
    _OPT_IPTIMESTAMP
}               t_opt_e;

// DEFINE
#define _OPT_MAX_NB _OPT_IPTIMESTAMP + 1
#define _OPT_ARG_MAX_NB 11
#define _OPT_DATA_LEN _OPT_MAX_NB + _OPT_ARG_MAX_NB

// STRUCT
typedef struct  s_opt_data {
    bool            opt[_OPT_MAX_NB];
    char            *opt_arg[_OPT_MAX_NB];
    struct timeval  timeout;
    int             ttl;
    int             deadline;
}               t_opt_d;

////////////////// Static Struct
/// -h -v -c -i -f -l -n -w -W -p -r -s -T --ttl --ip-timestamp
static struct argp_option  options[_OPT_MAX_NB + 1] = {
    { "help", '?', 0, 0, "give this help list"},
    { "verbose", 'v', 0, 0, "verbose output"},
    { "quiet", 'q', 0, 0, "quiet output"},
    { "count", 'c',0, 0, "stop after sending NUMBER packets"},
    { "interval", 'i',0, 0, "wait NUMBER seconds between sending each packet"},
    { "flood", 'f',0, 0, "flood ping (root only)"},
    { "preload",'l', 0, 0, "send NUMBER packets as fast as possible before falling into normal mode of behavior (root only)"},
    { "ignore-routing", 'r',0, 0, "send directly to a host on an attached network"},
    { "numeric", 'n',0, 0, "do not resolve host addresses"},
    { "timeout", 'w',0, 0, "stop after N seconds"},
    { "linger", 'W',0, 0, "number of seconds to wait for response"},
    { "pattern", 'p',0, 0, "fill ICMP packet with given pattern (hex)"},
    { "size", 's',0, 0, "send NUMBER data octets"},
    { "type", 't',0, 0, "send TYPE packets"},
    { "tos", 'T',0, 0, "set type of service (TOS) to NUM"},
    { "ttl", 1,0, 0, "specify N as time-to-live"},
    { "ip-timestamp", 2,0, 0, "IP timestamp of type FLAG, which is one of \"tsonly\" and \"tsaddr\""},
    {0}
};

// FUNCTION
void            opt_store(char *arr[], int arr_size, t_opt_d *opt_data, int *addr_pos);
bool            get_opt(t_opt_e opt_value, t_opt_d *data);
char            *get_opt_arg(t_opt_e opt_value, t_opt_d *data);
void            opt_handle(t_opt_d *data);

#endif