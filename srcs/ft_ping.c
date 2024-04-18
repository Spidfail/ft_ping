#include <ft_ping.h>

t_ping      g_ping = {0};
bool        g_stop = false;
const char *argp_program_bug_address = _OPT_ARGP_BUG_ADDR;
const char *argp_program_version = _OPT_ARGP_VERSION;

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
    parse_input(av, ac, data);
    if (data->timeout > 0)
        opt_fork_timeout(data->timeout);
}

static void     signal_handler(int sig) {
    switch (sig) {
        case SIGINT :
            g_stop = true;
            break;
        default:
            error_handle(-1, "Error: unexpected signal\n");
    }
}

int     main(int ac, char *av[]) {
    ft_bzero(&g_ping, sizeof(t_ping));
    arg_handle(&(g_ping.args), ac, av);

    g_ping.pid = getpid();
    g_ping.session = session_init_all(g_ping.pid, g_ping.args.args);
    session_deinit_hosts(&(g_ping.args.args));
    if (g_ping.session == NULL)
        error_handle(-1, "Error while initializing sessions");
    if (signal(SIGINT, signal_handler) == SIG_ERR)
        error_handle(-1, "Error while using signal");

    t_list *link = g_ping.session;
    while (link) {
        t_sum           *session = link->content;
        t_seq           *sequence = &(session->sequence);
        fd_set          sequence_set;
        struct timeval  timeout = {0};
        time_t          wait_scd = 1;
        int             rtn = 0;
        

        // Initialise the sockfd here rather than in session_init_all to avoid file descriptor shortage issue
        session->sockfd = socket_init(_INET_FAM, SOCK_RAW, IPPROTO_ICMP, &g_ping.args);

        // Necessary: inetutils-v2.0 is doing the lookup at the begining of each session.
        // Otherwise, different behaviour when getaddrinfo can't find the host.
        host_lookup(&(session->dest), session->dest.addr_orig, !g_ping.args.numeric);

        if (g_ping.args.interval > 0)
            wait_scd = g_ping.args.interval;
        if (g_ping.args.flood || g_ping.args.preload)
            timer_set_timeout(&timeout, wait_scd, true);
        else
            timer_set_timeout(&timeout, wait_scd, false);
        sequence_init(sequence, &(session->packet));
        timer_get(&(session->time.time_start));

        session_print_begin(session, &g_ping.args);
        if (packet_send(session->sockfd, &(session->dest), &(sequence->send)) == -1)
            error_handle(-1, "Impossible to send the packet");

        while (!g_stop) {
            FD_ZERO(&sequence_set);
            FD_SET(session->sockfd, &sequence_set);

            rtn = select(4, &sequence_set, NULL, NULL, &timeout);

            // Interrupted select()
            if (rtn == -1) {
                if (errno != EINTR)
                    error_handle(-1, "Select stopped unexpectedely");
                break;
            }
            // Timeout reached
            else if (rtn == 0) {
                session->seq_number++;
                packet_modify_sequence_number(&(sequence->send), session->seq_number);
                if (packet_send(session->sockfd, &(session->dest), &(sequence->send)) == -1)
                    error_handle(-1, "Impossible to send the packet");
                timer_get(&sequence->time_sent);
                if (g_ping.args.flood || session->seq_number + 1 <= g_ping.args.preload)
                    timer_set_timeout(&timeout, wait_scd, true);
                else
                    timer_set_timeout(&timeout, wait_scd, false);
                bzero(&(session->time.time_end), sizeof(struct timeval));
                sequence_clean(sequence);
            }
            // Fd is ready
            else if (FD_ISSET(session->sockfd, &sequence_set)) {
                sequence->recv_size = packet_receive(session->sockfd, sequence);
                timer_get(&session->time.time_end);
                sequence->time_enlapsed_ms = timer_enlapsed_ms(&sequence->time_sent, &session->time.time_end);
                session_time_update(session, sequence->time_enlapsed_ms);
                if (packet_verify_headers(sequence, ICMP_ECHOREPLY, 0) == EXIT_FAILURE)
                    session->err_number++;
                else
                    session->recv_number++;
                if (!g_ping.args.flood)
                    packet_print(sequence, sequence->time_enlapsed_ms);
                // Option --count
                if (session->seq_number + 1 == g_ping.args.count)
                    break;
            }
        }
        sequence_deinit(sequence);
        link = session_end(&g_ping.session);
    }
}