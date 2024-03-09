#include <ft_ping.h>
#include <ft_opt.h>

t_global    g_data = {0};

static void    check_input(int ac) {
    if (ac < 2)
        error_handle(EX_USAGE, NULL);
    // Limit max : binary + targeted address + number of options + their arguments
    else if (ac > 2 + _OPT_MAX_NB + _OPT_ARG_MAX_NB)
        error_handle(EX_USAGE, _HEADER_USAGE);
}

static void            arg_handle(int ac, char *av[], t_opt_d *data) {
    opt_store(av, ac, data);
    opt_handle(data);
}

int main(int ac, char *av[]) {
    ft_bzero(&g_data, sizeof(t_global));
    check_input(ac);
    arg_handle(ac, av, &(g_data.opt));

    // If no address has been found in arg_handle()
    // since av[0] points to the bin name
    host_lookup(g_data.opt.addr_raw, &g_data.dest_spec, !g_data.opt.opt[_OPT_n]);
    socket_init(&g_data.sockfd, &g_data.opt);
    init_data(&g_data.echo_request, &g_data.session);
    print_header_begin(g_data.sockfd, &g_data.dest_spec, &g_data.echo_request, &g_data.opt);
    // Prepare signals to be catch
    signal(SIGINT, signal_handler);
    signal(SIGALRM, signal_handler);
    // Launch timer
    if (gettimeofday(&g_data.session.time_start, NULL) == -1)
        error_handle(0, "Error while gettin' time");
    if (send_new_packet(g_data.sockfd, &g_data.echo_request, &g_data.dest_spec, &g_data.session) == -1)
        error_handle(0, "Error while sending data");
    // Launch the first tick with alarm
    if (!g_data.opt.opt[_OPT_f])
        alarm(1);
    if (g_data.opt.opt[_OPT_f])
        loop_flood();
    else
        loop_classic();
}