#include "ft_opt.h"
#include "libft.h"
#include <ft_opt.h>
#include <errno.h>
#include <ft_ping.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

t_ping    g_ping = {0};
const char *argp_program_bug_address = _OPT_ARGP_BUG_ADDR;
const char *argp_program_version = _OPT_ARGP_VERSION;

static void    check_input(int ac) {
    if (ac < 2)
        error_handle(EX_USAGE, NULL);
}

static void     parse_input(char *av[], int ac, t_arg_d *arg_data) {
    struct argp argp;
    
    bzero(&argp, sizeof(struct argp));
    argp.options = options;
    argp.parser = opt_parsing;
    argp.doc = _OPT_ARGP_DOC;
    argp.args_doc = _OPT_ARGP_ARGS_DOC;
    argp_parse(&argp, ac, av, 0, 0, arg_data);
}

static void     arg_handle(t_arg_d *data, int ac, char *av[]) {
    check_input(ac);
    parse_input(av, ac, data);
    if (data->timeout > 0)
        opt_fork_timeout(data->timeout);
}

// static int      init_signals() {
//     return 0;
// }

// static int      start_timer() {
//     return gettimeofday(&g_data.session.time_start, NULL);
// }
//

void    packet_cpy(t_packet *target, const t_packet *source) {
    ft_memcpy(target->data, source->data, _PING_DATA_SIZE);
    ft_memcpy(&(target->icmp_hdr), &(source->icmp_hdr), sizeof(struct icmphdr));
    ft_memcpy(&(target->ip_hdr), &(source->ip_hdr), sizeof(struct iphdr));
}

void    icmp_init(t_icmp *template, uint16_t pid) {
    template->ident = pid & 0xFFFF;
    template->data_size = _PING_DATA_SIZE;
    template->datagram_size = _ICMP_HDR_SIZE + template->data_size;
    template->packet_size = _IP_HDR_SIZE + template->datagram_size;
    template->data = ft_calloc(1, template->data_size);
    if (!template->data)
        error_handle(0, "Failed to allocate data");
}


int main(int ac, char *av[]) {
    ft_bzero(&g_ping, sizeof(t_ping));
    arg_handle(&(g_ping.args), ac, av);

    g_ping.pid = getpid();
    icmp_init(&(g_ping.template), g_ping.pid);
    // if (signal(SIGINT, signal_handler) == SIG_ERR)
    //     return -1;
    g_ping.session = session_init_all(g_ping.pid, g_ping.args.args, &(g_ping.template), &(g_ping.args));
    if (g_ping.session == NULL)
        error_handle(-1, "Error while initializing sessions");

    for (t_list *link = g_ping.session ; link ; link = link->next ) {
        t_sum   *active_s = link->content;
        printf("SESSION HOST = %s\n", active_s->dest.addr_orig);

        // start_timer()
        // start_timeout()
        
        // loop !stop
        // // timeout init
        // // select init
        // // select()
        // // select rtn()
        // // check rtn()
        // // print if needed
        // // stop conditions
        //
        // // send_packet?
        // stop_time()
        // end session
    }
}