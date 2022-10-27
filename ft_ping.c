#include "ft_ping.h"

t_global    g_data = {0};


static void    check_input(int ac) {
    if (ac < 2)
        error_handle(EX_USAGE, NULL);
    // Limit max : binary + targeted address + number of options + their arguments
    else if (ac > 2 + _OPT_MAX_NB + _OPT_ARG_MAX_NB)
        error_handle(EX_USAGE, _HEADER_USAGE);
}

int main(int ac, char *av[]) {
    int     addr_pos = 1;
    ft_bzero(&g_data, sizeof(t_global));
    check_input(ac);
    opt_init(ac, av, &g_data.opt, &addr_pos);
    opt_handle(&g_data.opt);

    //// Build the address:
    // Should use SOCK_RAW because ICMP is a protocol with
    // no user interface. So it need special options.
    // AF_INET for IPV4 type addr
    socket_init(&g_data.sockfd, &g_data.opt);

    init_data(&g_data.echo_request, &g_data.session);
    host_lookup(av[addr_pos], &g_data.dest_spec);
    print_header_begin(&g_data.dest_spec, &g_data.echo_request);
    // Prepare signals to be catch
    signal(SIGINT, signal_handler);
    signal(SIGALRM, signal_handler);
    // Launch timer
    if (gettimeofday(&g_data.session.time_start, NULL) == -1)
        error_handle(0, "Error while gettin' time");
    if (send_new_packet(g_data.sockfd, &g_data.echo_request, &g_data.dest_spec, &g_data.session) == -1)
        error_handle(0, "Error while sending data");

    if (g_data.opt.opt[_OPT_l])
        loop_preload();
    if (g_data.opt.opt[_OPT_f])
        loop_flood();
    else
        loop_classic();
}