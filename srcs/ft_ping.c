#include <ft_ping.h>

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

void    sequence_init(t_seq *sequence, const t_packet *to_send) {
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

int     packet_receive(int sockfd, t_seq *sequence) {
    return recvfrom(sockfd,
        &sequence->recv,
         sizeof(t_packet),
         0, 
         sequence->sender.addr_info->ai_addr,
         &(sequence->sender.addr_info->ai_addrlen));
}

static int     packet_verify_headers(const t_seq *sequence, uint8_t type, uint8_t code) {
    if (sequence->recv.icmp_hdr.type != type && sequence->recv.icmp_hdr.code != code)
        return EXIT_FAILURE;

    struct icmphdr  hdr_tmp = sequence->send->icmp_hdr;
    struct ip       ip_tmp = sequence->recv.ip_hdr;
    unsigned short  ip_checksum = 0; 

    hdr_tmp.type = 0;
    hdr_tmp.checksum = 0;
    ip_tmp.ip_sum = 0;
    hdr_tmp.checksum = ping_datagram_checksum(&hdr_tmp, sequence->send->data, _ICMP_HDR_SIZE + _PING_DATA_SIZE);
    ip_checksum = packet_checksum_calculate((char*)&ip_tmp, _IP_HDR_SIZE);
    if (memcmp(&hdr_tmp, &sequence->recv.icmp_hdr, _ICMP_HDR_SIZE) != 0)
        return EXIT_FAILURE;
    if (ip_checksum != sequence->recv.ip_hdr.ip_sum)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

int     main(int ac, char *av[]) {
    ft_bzero(&g_ping, sizeof(t_ping));
    arg_handle(&(g_ping.args), ac, av);

    g_ping.pid = getpid();
    // if (signal(SIGINT, signal_handler) == SIG_ERR)
    //     return -1;
    g_ping.session = session_init_all(g_ping.pid, g_ping.args.args, &(g_ping.args));
    if (g_ping.session == NULL)
        error_handle(-1, "Error while initializing sessions");

    for (t_list *link = g_ping.session ; link ; link = link->next ) {
        t_sum   *session = link->content;
        printf("SESSION HOST = %s\n", session->dest.addr_orig);
        printf("SESSION PACKET TYPE = %i\n", session->packet.icmp_hdr.type);

        // New sequence
        t_seq   *sequence = &(session->sequence);
        sequence_init(sequence, &(session->packet));
        int rtn = packet_send(session->sockfd, &(session->dest), sequence->send);
        printf(" RTN = %i\n", rtn);
        rtn = packet_receive(session->sockfd, sequence);
        printf(" RTN RECV = %i\n", rtn);
        rtn = packet_verify_headers(sequence, ICMP_ECHOREPLY, 0);
        printf(" RTN VERIFY HEADERS = %i\n", rtn);
        if (rtn == EXIT_FAILURE)
            printf(" ERROR CODE = %i || TYPE = %i || MESSAGE = %s\n", sequence->recv.icmp_hdr.code, sequence->recv.icmp_hdr.type, error_icmp_mapping(sequence->recv.icmp_hdr.type, sequence->recv.icmp_hdr.code));
        else
            printf(" GOOD CODE = %i\n", sequence->recv.icmp_hdr.code);
        sequence_deinit(sequence);
        
        

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