#include "ft_opt.h"
#include <ft_ping.h>

void     opt_fork_timeout(int scd) {
    __pid_t parent_id = getpid();
    __pid_t pid = fork();
    if (pid == 0) {
        sleep(scd);
        kill(parent_id, SIGINT);
        exit(0);
    }
}

static int    opt_interface_handle(const char *raw_addr) {
    struct sockaddr  addr = {0};
    int              rtn = EXIT_FAILURE;
    ifa_flag_t       flag = 0;

    rtn = interface_id(&flag, (t_ifid)raw_addr, _IFE_NAME);
    if (rtn == EXIT_FAILURE && interface_lookup((struct sockaddr_in*)&addr, raw_addr, 0) == EXIT_SUCCESS) {
        rtn = interface_id(&flag, (t_ifid)&addr, _IFE_ADDR);
    }
    if (rtn == EXIT_SUCCESS) {
        if (flag & IFF_LOOPBACK) {
            fprintf(stdout, "%s Warning: source address might be selected on device other than: %s\n",
                    _ERROR_HEADER, raw_addr);
        }
    }
    return rtn;
}

// -h -v -c -i -f -l -n -w -W -p -r -s -T --ttl --ip-timestamp
error_t     opt_parsing(int key, char *arg, struct argp_state *state) {
    t_arg_d     *option_data = state->input;
    error_t     error_exnum = -1;

    switch (key) {
        // TODO: HELP already implemented in argp, need a way to mitigate it
        case 'v':
            option_data->verbose = true;
            break;

        case 'V':
            option_data->version = true;
            break;
        
        case 'c':
            if (option_check_is_digit(arg) )
                opt_error_handle(OPT_ERR_INVALID, arg, error_exnum);
            option_data->count = ft_atoi(arg);
            break;

        case 'i':
            if (option_check_is_digit(arg) )
                opt_error_handle(OPT_ERR_INVALID, arg, error_exnum);
            else if (ft_atoi(arg) < 1)
                opt_error_handle(OPT_ERR_TOOSMALL, arg, error_exnum);
            option_data->interval = ft_atoi(arg);
            break;

        case 'n':
            option_data->numeric = true;
            break;

        case _OPT_ARGPK_TTL:
            if (option_check_is_digit(arg) )
                opt_error_handle(OPT_ERR_INVALID, arg, error_exnum);
            else if (ft_atoi(arg) < 1)
                opt_error_handle(OPT_ERR_TOOSMALL, arg, error_exnum);
            option_data->ttl = ft_atoi(arg);
            break;

        case 'T':
            if (option_check_is_digit(arg) )
                opt_error_handle(OPT_ERR_INVALID, arg, error_exnum);
            // Depends on the architecture
            else if (ft_strlen(arg) > 3)
                opt_error_handle(OPT_ERR_TOOBIG, arg, error_exnum);
            option_data->tos = ft_atoi(arg);
            if (option_data->tos > CHAR_MAX)
                opt_error_handle(OPT_ERR_TOOBIG, arg, error_exnum);
            else if (option_data->tos < 1)
                opt_error_handle(OPT_ERR_TOOSMALL, arg, error_exnum);
            break;


        case 'w':
            if (option_check_is_digit(arg) )
                opt_error_handle(OPT_ERR_INVALID, arg, error_exnum);
            // Depends on the architecture
            else if (ft_strlen(arg) > 10)
                opt_error_handle(OPT_ERR_TOOBIG, arg, error_exnum);
            if (ft_atol(arg) > INT_MAX)
                opt_error_handle(OPT_ERR_TOOBIG, arg, error_exnum);
            option_data->timeout = ft_atoi(arg);
            if (option_data->timeout < 1)
                opt_error_handle(OPT_ERR_TOOSMALL, arg, error_exnum);
            break;

        case 'W':
            if (option_check_is_digit(arg) )
                opt_error_handle(OPT_ERR_INVALID, arg, error_exnum);
            // Depends on the architecture
            else if (ft_strlen(arg) > 10)
                opt_error_handle(OPT_ERR_TOOBIG, arg, error_exnum);
            if (ft_atol(arg) > INT_MAX)
                opt_error_handle(OPT_ERR_TOOBIG, arg, error_exnum);
            option_data->linger = ft_atoi(arg);
            if (option_data->linger < 1)
                opt_error_handle(OPT_ERR_TOOSMALL, arg, error_exnum);
            break;

        case 'f':
            option_data->flood = true;
            break;

        // case _OPT_IPTIMESTAMP:
        //     if (arg != NULL)
        //         printf("%s/n", arg);
        //     else
        //         printf("Empty arg iptimestamp!\n");
        //     break;

        case 'l':
            if (option_check_is_digit(arg) )
                opt_error_handle(OPT_ERR_INVALID, arg, error_exnum);
            // Depends on the architecture
            else if (ft_strlen(arg) > 10)
                opt_error_handle(OPT_ERR_TOOBIG, arg, error_exnum);
            if (ft_atol(arg) > INT_MAX)
                opt_error_handle(OPT_ERR_TOOBIG, arg, error_exnum);
            option_data->preload = ft_atoi(arg);
            break;

        case 'p':
            if (option_check_is_hex(arg) )
                opt_error_handle(OPT_ERR_INVALID, arg, error_exnum);
            option_data->pattern = arg;
            break;

        case 'q':
            if (option_check_is_digit(arg) )
                opt_error_handle(OPT_ERR_INVALID, arg, error_exnum);
            option_data->quiet = true;
            break;

        case 's':
            if (option_check_is_digit(arg) )
                opt_error_handle(OPT_ERR_INVALID, arg, error_exnum);
            // If has more char than IP_MAXPACKET number
            if (ft_strlen(arg) > 5)
                opt_error_handle(OPT_ERR_TOOBIG, arg, error_exnum);
            option_data->size = ft_atoi(arg);
            if (option_data->size > IP_MAXPACKET - _IP_HDR_SIZE - _ICMP_HDR_SIZE)
                opt_error_handle(OPT_ERR_TOOBIG, arg, error_exnum);
            break;

        case 'I':
            if (!arg || ft_strlen(arg) < 1 || ft_strlen(arg) > IFNAMSIZ)
                opt_error_handle(OPT_ERR_IFA, arg, error_exnum);
            if (opt_interface_handle(arg) == EXIT_FAILURE)
                opt_error_handle(OPT_ERR_IFA, arg, error_exnum);
            option_data->interface = arg;
            break;

        case 'S':
            if (option_check_is_digit(arg) )
                opt_error_handle(OPT_ERR_INVALID, arg, error_exnum);
            // Depends on the architecture
            else if (ft_strlen(arg) > 10)
                opt_error_handle(OPT_ERR_TOOBIG, arg, error_exnum);
            if (ft_atol(arg) > INT_MAX)
                opt_error_handle(OPT_ERR_TOOBIG, arg, error_exnum);
            option_data->sndbuf = ft_atoi(arg);
            break;

        case ARGP_KEY_ARG:
            option_data->arg_raw = arg;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}