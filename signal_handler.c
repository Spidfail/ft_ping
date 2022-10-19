#include "ft_ping.h"
#include <sys/socket.h>

static void     interrupt() {
    freeaddrinfo(g_data.dest_spec.addr_info);
    free_data(&g_data.echo_request);
    exit(0);
}

void            handle_tick() {
    print_packet(&g_data.echo_request, &g_data.dest_spec);
    clean_data(&g_data.echo_request);
    ++g_data.session.seq_number;
    if (send_new_packet(g_data.sockfd,
            &g_data.echo_request,
            &g_data.dest_spec,
            &g_data.session) == -1)
      error_handle(0, "Error while sending packet");
}

void            signal_handler(int sig) {
    switch (sig) {
        case SIGINT :
            print_sum(&g_data.session, &g_data.dest_spec);
            interrupt();
        break;
        case SIGALRM :
            handle_tick();
            alarm(1);
        break;
    }
}