#include "ft_ping.h"
#include "libft.h"

t_global    g_data = {0};

static bool     is_addr(char *addr) {
    if (addr == NULL || addr[0] == '-')
        return false;
    return true;
}

static void    check_input(int ac, char *av[]) {
    if (ac < 2 || !is_addr(av[1]))
        error_handle(EX_USAGE, NULL);
    // Limit max : binary + targeted address + number of options + their arguments
    else if (ac > 2 + _OPT_MAX_NB + _OPT_ARG_MAX_NB)
        error_handle(EX_USAGE, _HEADER_USAGE);
}

int main(int ac, char *av[]) {
    int             broadcast = true;
    bool            flood = false;
    bool            is_first = true;
    // Clear the buffer in case of garbage

    ft_bzero(&g_data, sizeof(t_global));
    check_input(ac, av);
    opt_init(ac, av, &g_data.opt);

    //// Build the address:
    // Should use SOCK_RAW because ICMP is a protocol with
    // no user interface. So it need special options.
    // AF_INET for IPV4 type addr
    if ((g_data.sockfd = socket(_INET_FAM, SOCK_RAW, IPPROTO_ICMP)) == -1)
        error_handle(0, "Failed to open socket [AF_INET, SOCK_RAW, IPPROTO_ICMP]");
    // Set option to 
    setsockopt(g_data.sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(int));

    init_data(&g_data.echo_request, &g_data.session);
    host_lookup(av[1], &g_data.dest_spec);
    print_header_begin(&g_data.dest_spec, &g_data.echo_request);
    // Prepare signals to be catch
    signal(SIGINT, signal_handler);
    signal(SIGALRM, signal_handler);
    // Launch timer
    if (gettimeofday(&g_data.session.time_start, NULL) == -1)
        error_handle(0, "Error while gettin' time");
    if (send_new_packet(g_data.sockfd, &g_data.echo_request, &g_data.dest_spec, &g_data.session) == -1)
        error_handle(0, "Error while sending data");
    while (true) {
        if (receive_data(g_data.sockfd, &g_data.echo_request, &g_data.session) == EXIT_SUCCESS)
            g_data.echo_request.is_packet = true;
        // The tick is forced in flood mod
        if (is_first || flood) {
            handle_tick();
            if (is_first) {
                is_first = false;
                alarm(1);
            }
        }
    }
}