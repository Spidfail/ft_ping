#include "ft_ping.h"
#include <stdbool.h>

static void     opt_error_handle(char *arg, size_t limit) {
    size_t  size = 100 + ft_strlen(arg);
    char    *buff_err = ft_calloc(size, sizeof(char));
    if (buff_err == NULL)
        error_handle(0, buff_err);
    if (limit != 0)
        sprintf(buff_err, "ping: invalid argument: '%s': out of range: 0 <= value <= %lu", arg, limit);
    else
        sprintf(buff_err, "ping: invalid argument: '%s'", arg);
    error_handle(_EX_NOERRNO, buff_err);
}

static int     opt_set_num_value(void *buff, long limit, char *arg) {
    long      nb = 0;

    for (size_t i = 0 ; i < ft_strlen(arg) ; ++i)
        if (!ft_isdigit(arg[i]))
            opt_error_handle(arg, 0);
    nb = ft_atol(arg);
    if (nb > limit || nb < 0)
        return EXIT_FAILURE;
    *(long*)buff = nb;
    return EXIT_SUCCESS;
}

void            fork_timeout(int scd) {
    __pid_t parent_id = getpid();
    __pid_t pid = fork();
    if (pid == 0) {
        sleep(scd);
        kill(parent_id, SIGINT);
        exit(0);
    }
}

void            opt_handle(t_opt_d *data) {
    for (short i = 0 ; i < _OPT_MAX_NB ; ++i) {
        if (data->opt[i])
            switch(i) {
                case _OPT_l:
                    if (opt_set_num_value(&data->preload, USHRT_MAX, data->opt_arg[i]) == EXIT_FAILURE)
                        opt_error_handle(data->opt_arg[i], USHRT_MAX);
                    break;
                case _OPT_w:
                    if (opt_set_num_value(&data->deadline, INT_MAX, data->opt_arg[i]) == EXIT_FAILURE)
                        opt_error_handle(data->opt_arg[i], INT_MAX);
                    fork_timeout(data->deadline);
                    break;
                case _OPT_W:
                    if (opt_set_num_value(&data->timeout, INT_MAX, data->opt_arg[i]) == EXIT_FAILURE)
                        opt_error_handle(data->opt_arg[i], INT_MAX);
                    break;
                case _OPT_t:
                    if (opt_set_num_value(&data->ttl, CHAR_MAX, data->opt_arg[i]) == EXIT_FAILURE)
                        opt_error_handle(data->opt_arg[i], CHAR_MAX);
                    break;
                case _OPT_S:
                    if (opt_set_num_value(&data->sndbuf, INT_MAX, data->opt_arg[i]) == EXIT_FAILURE)
                        opt_error_handle(data->opt_arg[i], INT_MAX);
                    break;
            }
    }
}

/// OPT list : < -h -v -f -l -I -m -M -n -w -W -p -Q -S -t -T >
/// Only the first two are mandatory
void            opt_store(char *arr[], int arr_size, t_opt_d *opt_data, int *addr_pos) {
    static const char   *opt_values[_OPT_MAX_NB] = {
        "-h", "-v", "-f", "-l", "-I", "-m", "-M", "-n",
        "-w", "-W", "-p", "-Q", "-S", "-t", "-T",
    };

    // Iterate over arguments and check every arg if this is an option or not.
    for (int i = 1 ; i < arr_size ; ++i) {
        for (int j = 0 ; j < _OPT_MAX_NB ; ++j) {
            // If this is an option, store it
            if (ft_strcmp(arr[i], opt_values[j]) == 0) {
                opt_data->opt[j] = true;
                // If the option have an argument associated.
                if ((j >= _OPT_l && j <= _OPT_M) || (j >= _OPT_w && j <= _OPT_T)) {
                    // Then check if a correct arg is associated,
                    // like it's not an option.
                    // If it is -> store it / Either throw error.
                    if (i + 1 < _OPT_MAX_NB && arr[i + 1][0] != '-') {
                        opt_data->opt_arg[j] = ft_strdup(arr[i + 1]);
                    }
                    else
                        error_handle(EX_USAGE, _HEADER_USAGE);
                }
                break;
            }
            if (j + 1 == _OPT_MAX_NB)
                *addr_pos = i;
        }
    }
}

bool            get_opt(t_opt_e opt_value, t_opt_d *data) {
    return data->opt[opt_value];
}

char            *get_opt_arg(t_opt_e opt_value, t_opt_d *data) {
    return data->opt_arg[opt_value];
}

void            opt_init(int ac, char *av[], t_opt_d *data, int *addr_pos) {
    for (int i = 0 ; i < _OPT_MAX_NB ; ++i)
        data->opt_arg[i] = NULL;
    bzero(data->opt, _OPT_MAX_NB);
    bzero(&data->timeout , sizeof(struct timeval));
    bzero(&data->deadline, sizeof(int));
    bzero(&data->preload , sizeof(ushort));
    bzero(&data->ttl     , sizeof(int));
    bzero(&data->sndbuf  , sizeof(int));
    opt_store(av, ac, data, addr_pos);
}