#include <ft_ping.h>

double      timer_enlapsed_ms(const struct timeval *start, const struct timeval *end) {
    return (((double)end->tv_sec - (double)start->tv_sec) * 1000)
        + (((double)end->tv_usec - (double)start->tv_usec) / 1000);
}

void        timer_get(struct timeval *timer) {
    if (gettimeofday(timer, NULL) == -1)
        error_handle(0, "Error while gettin' start time");
}

void        timer_set_timeout(struct timeval *timeout, time_t value) {
    timeout->tv_usec = value * 1000000;
    timeout->tv_sec = 0;
}