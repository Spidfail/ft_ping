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

/* 
 * Calculate the mean distance of a list of values using the sum of these values
 * and the length of that list of values (aka the number a values inside this list).
 */
// double        timer_calculate_mean(int sum, int length) {
//     return (double)sum / (double)length;
// }

/* 
 * Calculate the square distance of the value x1 from the mean μ -> (x1 − μ)^2
 */
// void        timer_calculate_stddev(int x1, int mean) {
    
// }