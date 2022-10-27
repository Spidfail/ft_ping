#include "ft_ping.h"

void    loop_preload() {
    // The tick is forced in preload mod for a number of packets
    for (ushort i = 0 ; i < g_data.opt.preload ; ++i) {
        if (receive_data(g_data.sockfd, &g_data.echo_request, &g_data.session) == EXIT_SUCCESS)
            g_data.echo_request.is_packet = true;
        handle_tick();
    }
}

void    loop_flood() {
    while (true) {
        if (receive_data(g_data.sockfd, &g_data.echo_request, &g_data.session) == EXIT_SUCCESS)
            g_data.echo_request.is_packet = true;
        // The tick is forced in flood mod
        handle_tick();
    }
}

void    loop_classic() {
    bool            is_first = true;

    while (true) {
        if (receive_data(g_data.sockfd, &g_data.echo_request, &g_data.session) == EXIT_SUCCESS)
            g_data.echo_request.is_packet = true;
        // The tick is forced in flood mod
        if (is_first) {
            handle_tick();
            if (is_first) {
                is_first = false;
                alarm(1);
            }
        }
    }
}