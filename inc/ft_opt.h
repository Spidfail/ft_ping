
#ifndef FT_OPT_H
# define FT_OPT_H

#include <libft.h>
#include <stdbool.h>
#include <sys/time.h>
#include <stdio.h>
#include <argp.h>

// DEFINE
#define _ERROR_OPT_INVALID "invalide value"
#define _ERROR_OPT_TOOSMALL "option value too small"
#define _ERROR_OPT_TOOBIG "option value too big"

typedef enum  e_opt_error_num {
    OPT_ERR_INVALID,
    OPT_ERR_TOOSMALL,
    OPT_ERR_TOOBIG,
    OPT_ERR_PATTERN,
    OPT_ERR_IFA,
}               t_opt_err;

// STRUCT
typedef struct  s_arg_data {
    char            *arg_raw;
    bool            verbose;
    bool            help;
    int             count;
    int             interval;
    bool            numeric;
    int             ttl;
    int             tos;
    bool            version;
    int             timeout;
    struct timeval  timeout_timeval;
    int             linger;
    int             flood;
    // int             iptimestamp;
    int             preload;
    char            *pattern;
    bool            quiet;
    int             size;
    char            *interface;
    int             sndbuf;
}               t_arg_d;

////////////////// Static Struct
/// implemented (inetutils-v2): -h -v -f -w -W -n -TTL
/// implemented (augmented): -I -S
/// -h -v -c -i -f -l -n -w -W -p -r -s -T --ttl --ip-timestamp
#define _OPT_ARGPK_TTL 1
#define _OPT_ARGPK_IPTIMESTAMP 2

static struct argp_option  options[] = {
    // Already implemented by `argp`
    // { "help", '?', 0, 0, "give this help list", 0},
    // { "version", 'V', 0, 0, "print program version", 0},
    { "help", '?', 0, 0, "give this help list", 0},
    { "verbose", 'v', 0, 0, "verbose output", 0},
    { "count", 'c', "NUMBER", 0, "stop after sending NUMBER packets", 0},
    { "interval", 'i', "NUMBER", 0, "wait NUMBER seconds between sending each packet", 0},
    { "numeric", 'n', 0, 0, "do not resolve host addresses", 0},
    { "ttl", _OPT_ARGPK_TTL, "NUMBER", 0, "specify N as time-to-live", 0},
    { "tos", 'T', "NUM", 0, "set type of service (TOS) to NUM", 0},
    { "timeout", 'w', "N", 0, "stop after N seconds", 0},
    { "linger", 'W', "N", 0, "number of seconds to wait for response", 0},
    { "flood", 'f',0, 0, "flood ping (root only)", 0},
    // { "ip-timestamp", _OPT_ARGPK_IPTIMESTAMP, "FLAG", 0, "IP timestamp of type FLAG, which is one of \"tsonly\" and \"tsaddr\"", 0},
    { "preload",'l', "NUMBER", 0, "send NUMBER packets as fast as possible before falling into normal mode of behavior (root only)", 0},
    { "pattern", 'p', "PATTERN", 0, "fill ICMP packet with given pattern (hex)", 0},
    { "quiet", 'q', 0, 0, "quiet output", 0},
    { "size", 's', "SIZE", 0, "send NUMBER data octets", 0},
    // Augmented, not available in inetutils-v2
    { "interface", 'I', "IFACE", 0, "either interface name or address", 0},
    { "sendbuf", 'S', "SIZE", 0, "use <size> as SO_SNDBUF socket option value", 0},
    { 0 }
};

// FUNCTION
error_t         opt_parsing(int key, char *arg, struct argp_state *state);
void            opt_fork_timeout(int scd);

// Option checking
int             option_check_is_digit(char *argument);
int             option_check_is_alnum(char *argument);
int             option_check_is_hex(char *argument);

// Error handling
void            opt_error_handle(t_opt_err errnum, char *arg, int exnum);

#endif