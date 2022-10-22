#include "ft_ping.h"

/// OPT list : < -h -v -f -l -I -m -M -n -w -W -p -Q -S -t -T >
/// Only the first two are mandatory
void            opt_store(char *arr[], int arr_size, t_opt_d *opt_data) {
    static const char   *opt_values[_OPT_MAX_NB] = {
        "-h", "-v", "-f", "-l", "-I", "-m", "-M", "-n",
        "-w", "-W", "-p", "-Q", "-S", "-t", "-T",
    };

    // Iterate over arguments and check every arg if this is an option or not.
    for (int i = 0 ; i < arr_size ; ++i) {
        for (int j = 0 ; j < _OPT_MAX_NB ; ++j) {
            // If this is an option, store it
            if (ft_strcmp(arr[i], opt_values[j]) == 0) {
                opt_data->opt[j] = true;
                // If the option should have an argument associated.
                if ((j >= _OPT_l && j <= _OPT_M) || (j >= _OPT_w && j <= _OPT_T)) {
                    // Then check if a correct arg is associated,
                    // like it's not an option.
                    // If it is -> store it / Either throw error.
                    if (i + 1 < _OPT_MAX_NB && arr[i + 1][0] != '-')
                        opt_data->opt_arg[j] = ft_strdup(arr[i + 1]);
                    else
                        error_handle(EX_USAGE, _HEADER_USAGE);
                }
            }
        }
    }
}

bool            get_opt(t_opt_e opt_value, t_opt_d *data) {
    return data->opt[opt_value];
}

char            *get_opt_arg(t_opt_e opt_value, t_opt_d *data) {
    return data->opt_arg[opt_value];
}

void            opt_init(int ac, char *av[], t_opt_d *data) {
    for (int i = 0 ; i < _OPT_MAX_NB ; ++i)
        data->opt_arg[i] = NULL;
    bzero(data->opt, _OPT_MAX_NB);
    opt_store(av, ac, data);
}