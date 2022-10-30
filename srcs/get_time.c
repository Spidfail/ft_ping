#include <ft_ping.h>

double      get_enlapsed_ms(const struct timeval *start, const struct timeval *end) {
    return (((double)end->tv_sec - (double)start->tv_sec) * 1000)
        + (((double)end->tv_usec - (double)start->tv_usec) / 1000);
}

void        update_time(t_sum *session, const struct timeval *start, const struct timeval *end) {
    session->time_enlapsed = get_enlapsed_ms(start, end);
    session->time_delta += session->time_enlapsed;
    if (session->time_max < session->time_enlapsed)
        session->time_max = session->time_enlapsed;
    if (session->time_min > session->time_enlapsed)
        session->time_min = session->time_enlapsed;
}