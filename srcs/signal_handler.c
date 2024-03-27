#include <ft_ping.h>

void            interrupt(int exit_nb) {
    freeaddrinfo(g_data.dest_spec.addr_info);
    free_data(&g_data.echo_request);
    close(g_data.sockfd);
    exit(exit_nb);
}

void            end_session(t_sum *session, t_host *dest) {
    if (session->time_end.tv_sec == 0 && session->time_end.tv_usec == 0)
        if (gettimeofday(&(session->time_end), NULL) == -1)
            error_handle(0, "Error while gettin' end time");
    print_sum(session, dest);
    interrupt(0);
}

void            new_load() {
    clean_data(&g_data.echo_request);
    ++g_data.session.seq_number;
    if (send_new_packet(g_data.sockfd, &g_data.echo_request, &g_data.dest_spec,
                        &g_data.session) == -1)
      error_handle(0, "Error while sending packet");
}

void            handle_tick() {
    if (!g_data.args.flood) {
      if ((g_data.echo_request.is_packet &&
           g_data.echo_request.packet->icmp_hdr.type == ICMP_ECHOREPLY) ||
          !g_data.args.verbose)
        print_packet(&g_data.echo_request, &g_data.session);
      else if (g_data.args.verbose) {
        print_packet_error(&g_data.echo_request,
                           g_data.echo_request.packet->icmp_hdr.type,
                           g_data.echo_request.packet->icmp_hdr.code);
      }
    }
    else {
        //fprintf(stdout, "\x1b[1D"); // Move to beginning of previous line
        //fprintf(stdout, "\x1b[2K"); // Clear entire line
        if (!g_data.echo_request.is_packet) {
            fprintf(stdout, "E");
        }
        else {
            fprintf(stdout, "\b");
            fprintf(stdout, ".");
        }
        fflush(stdout); // Will now print everything in the stdout buffer
    }
    new_load();
}

void            signal_handler(int sig) {
    switch (sig) {
        case SIGINT :
            end_session(&(g_data.session), &(g_data.dest_spec));
        break;
        case SIGALRM :
            handle_tick();
            alarm(1);
        break;
    }
}