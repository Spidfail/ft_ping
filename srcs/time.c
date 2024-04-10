#include <ft_ping.h>

void        time_update(t_sum *session, double enlapsed) {
    printf("ENLAPSED = %f \n", enlapsed);
    printf("TMAX = %f \n", session->time.time_max);
    session->time.time_delta += enlapsed;
    if (session->time.time_max < enlapsed)
        session->time.time_max = enlapsed;
    if (session->time.time_min > enlapsed)
        session->time.time_min = enlapsed;
}

double      timer_enlapsed_ms(const struct timeval *start, const struct timeval *end) {
    return (((double)end->tv_sec - (double)start->tv_sec) * 1000)
        + (((double)end->tv_usec - (double)start->tv_usec) / 1000);
}