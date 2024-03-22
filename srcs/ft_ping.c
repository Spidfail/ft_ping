#include <ft_ping.h>
#include <ft_opt.h>

t_global    g_data = {0};

static void    check_input(int ac) {
    if (ac < 2)
        error_handle(EX_USAGE, NULL);
}

static void            parse_input(char *av[], int ac, t_arg_d *arg_data) {
    struct argp argp;
    
    bzero(&argp, sizeof(struct argp));
    argp.options = options;
    argp.parser = opt_parsing;
    argp.args_doc = "HOST";
    argp_parse(&argp, ac, av, 0, 0, arg_data);
}

static void     arg_handle(t_global *data, int ac, char *av[]) {
    check_input(ac);
    parse_input(av, ac, &(data->args));
    if (data->args.timeout > 0)
        opt_fork_timeout(data->args.timeout);
}


int main(int ac, char *av[]) {
    ft_bzero(&g_data, sizeof(t_global));
    arg_handle(&g_data, ac, av);


    // If no address has been found in arg_handle()
    // since av[0] points to the bin name
    host_lookup(g_data.args.arg_raw, &g_data.dest_spec, !g_data.args.numeric);
    socket_init(&g_data.sockfd, &g_data.args);
    init_data(&g_data.echo_request, &g_data.session);
    print_header_begin(g_data.sockfd, &g_data.dest_spec, &g_data.echo_request, &g_data.args);
    // Prepare signals to be catch
    signal(SIGINT, signal_handler);
    signal(SIGALRM, signal_handler);
    // Launch timer
    if (gettimeofday(&g_data.session.time_start, NULL) == -1)
        error_handle(0, "Error while gettin' time");
    if (send_new_packet(g_data.sockfd, &g_data.echo_request, &g_data.dest_spec, &g_data.session) == -1)
        error_handle(0, "Error while sending data");
    // Launch the first tick with alarm
    if (!g_data.args.flood)
        alarm(1);
    if (g_data.args.flood)
        loop_flood();
    else
        loop_classic();
}