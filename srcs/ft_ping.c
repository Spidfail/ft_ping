#include <ft_ping.h>

t_ping      g_ping = {0};
bool        g_stop = false;
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

void    sequence_init(t_seq *sequence, const t_packet *to_send) {
    timer_get(&(sequence->time_sent));
    sequence->send = to_send;
    sequence->recv_size = 0;
    sequence->sender.addr_info = ft_calloc(1, sizeof(struct addrinfo));
    if (sequence->sender.addr_info == NULL)
        error_handle(0, "Fatal: Failed to allocate addrinfo");
    sequence->sender.addr_info->ai_addrlen = 100;
    sequence->sender.addr_info->ai_addr = ft_calloc(1, sequence->sender.addr_info->ai_addrlen);
    if (sequence->sender.addr_info->ai_addr == NULL)
        error_handle(0, "Fatal: Failed to allocate sockaddr");
}

void    sequence_deinit(t_seq *sequence) {
   free(sequence->sender.addr_info->ai_addr);
   free(sequence->sender.addr_info);
   bzero(sequence, sizeof(t_seq));
}

void    sequence_clean(t_seq *sequence) {
    struct addrinfo save = {0};
    memcpy(&save, sequence->sender.addr_info, sizeof(struct addrinfo));
    
    sequence->recv_size = 0;
    bzero(&sequence->recv, sizeof(t_packet));


    sequence->sender.addr_orig = NULL;
    bzero(sequence->sender.addr_str, INET_ADDRSTRLEN);
    bzero(sequence->sender.addr_info, sizeof(struct addrinfo));
    sequence->sender.addr_info->ai_addr = save.ai_addr;
    bzero(&sequence->time_enlapsed_ms, sizeof(time_t));
}

void            signal_handler(int sig) {
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
    g_ping.session = session_init_all(g_ping.pid, g_ping.args.args, &(g_ping.args));
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
        
        timer_set_timeout(&timeout, wait_scd);
        sequence_init(sequence, &(session->packet));
        timer_get(&(session->time.time_start));
        if (packet_send(session->sockfd, &(session->dest), sequence->send) == -1)
            error_handle(-1, "Impossible to send the packet");

        while (!g_stop) {
            FD_ZERO(&sequence_set);
            FD_SET(session->sockfd, &sequence_set);

            rtn = select(4, &sequence_set, NULL, NULL, &timeout);

            // Interrupted select()
            if (rtn == -1) {
                if (errno != EINTR)
                    error_handle(-1, "Select stopped unexpectedely");
                printf("GSTOP = %i\n", g_stop);
                break;
            }
            // Timeout reached
            else if (rtn == 0) {
                printf("Timeout reached\n");
                session->seq_number++;
                if (packet_send(session->sockfd, &(session->dest), sequence->send) == -1)
                    error_handle(-1, "Impossible to send the packet");
                timer_get(&sequence->time_sent);
                timer_set_timeout(&timeout, wait_scd);
                bzero(&(session->time.time_end), sizeof(struct timeval));
                sequence_clean(sequence);
            }
            // Fd is ready
            else if (FD_ISSET(session->sockfd, &sequence_set)) {
                printf("FD ready\n");
                sequence->recv_size = packet_receive(session->sockfd, sequence);
                timer_get(&session->time.time_end);
                sequence->time_enlapsed_ms = timer_enlapsed_ms(&sequence->time_sent, &session->time.time_end);
                session_time_update(session, sequence->time_enlapsed_ms);
                if (packet_verify_headers(sequence, ICMP_ECHOREPLY, 0) == EXIT_FAILURE)
                    session->err_number++;
                else
                    session->recv_number++;
                packet_print(sequence, sequence->time_enlapsed_ms, session->seq_number);
            }
            // TODO: Add a stop condition -> option --count
        }
        fprintf(stderr, "sequence deinit\n");
        sequence_deinit(sequence);
        link = session_end(&g_ping.session);
    }
}