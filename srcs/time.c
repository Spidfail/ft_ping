#include <ft_ping.h>

double      timer_enlapsed_ms(const t_packet *packet) {
    struct timeval start = {0};
    struct timeval end = {0};
    
    ft_memcpy(&start, packet->data, sizeof(struct timeval));
    timer_get(&end);
    return (((double)end.tv_sec - (double)start.tv_sec) * 1000)
        + (((double)end.tv_usec - (double)start.tv_usec) / 1000);
}

void        timer_get(struct timeval *timer) {
    if (gettimeofday(timer, NULL) == -1)
        error_handle(0, "Error while gettin' start time");
}

void        timer_set_timeout(struct timeval *timeout, time_t value, bool flood) {
    if (!flood)
        timeout->tv_usec = value * 1000000;
    else
        timeout->tv_usec = value * 10000;
    timeout->tv_sec = 0;
}