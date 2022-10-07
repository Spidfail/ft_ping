
#ifndef FT_PING_H /////////////////////////////////////////////////////////////
# define FT_PING_H
#include <sysexits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>


#include "./libft_extended/libft.h"

#define ERROR_HEADER "ping:"
#define ERROR_RESOLVE "cannot resolve"
#define ERROR_USAGE "ping [-AaDdfnoQqRrv] [-c count] [-G sweepmaxsize] \
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

typedef struct  s_icmp {
    char                type;
    char                code;
    short               checksum;
    short               identifier;
    short               seq_nb;
    char                *str_addr;
}               t_icmp;


#endif // FT_PING_H ///////////////////////////////////////////////////////////
