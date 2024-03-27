#include <ft_ping.h>

void    loop_flood() {
    while (true) {
        if (receive_data(g_data.sockfd, &g_data.echo_request, &g_data.session) == EXIT_SUCCESS)
            g_data.echo_request.is_packet = true;
        // The tick is forced in flood mod
        handle_tick();
    }
}

void    loop_classic() {
    bool     is_first = true;
    int     counter = g_data.args.count;
    
    if (counter > 0)
        counter += 1;

    while (true) {
        if (receive_data(g_data.sockfd, &g_data.echo_request, &g_data.session) == EXIT_SUCCESS)
            g_data.echo_request.is_packet = true;
        // The tick is forced in flood mod
        if (is_first) {
            handle_tick();
            is_first = false;
        }
        if (counter != 0) {
            if (counter == 1)
                end_session(&g_data.session, &g_data.dest_spec);
            counter--;
        }
    }
}