#include <ft_ping.h>
#include <ft_opt.h>
#include <argp.h>
#include <stdio.h>

static void     opt_error_num_handle(char *arg, size_t limit) {
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

static void     opt_error_handle(char *arg, int opt) {
    int len = (sizeof(_ERROR_HEADER) * 2) + sizeof(' ') // 'ping: '
            + sizeof(_ERROR_INTERFACE_SRCADDR) + sizeof('\n')
            + sizeof(_ERROR_INTERFACE_IFACE)
            + (IFNAMSIZ * 2) + 1;
    char err_interface[len];
    bzero(err_interface, len);

    switch (opt) {
        case _OPT_I:
            ft_strlcat(err_interface, _ERROR_HEADER, len);
            ft_strlcat(err_interface, " ", len);
            ft_strlcat(err_interface, _ERROR_INTERFACE_SRCADDR, len);
            ft_strlcat(err_interface, arg, len);
            ft_strlcat(err_interface, "\n", len);

            ft_strlcat(err_interface, _ERROR_HEADER, len);
            ft_strlcat(err_interface, " ", len);
            ft_strlcat(err_interface, _ERROR_INTERFACE_IFACE, len);
            ft_strlcat(err_interface, arg, len);
        //It may be more to add
    }
    error_handle(_EX_NOERRNO, err_interface);
}

static int     opt_set_num_value(void *buff, long limit, char *arg) {
    long      nb = 0;

    if (arg == NULL || ft_strlen(arg) == 0)
        opt_error_num_handle(arg, 0);
    for (size_t i = 0 ; i < ft_strlen(arg) ; ++i)
        if (!ft_isdigit(arg[i]))
            opt_error_num_handle(arg, 0);
    nb = ft_atol(arg);
    if (nb > limit || nb < 0)
        return EXIT_FAILURE;
    if (limit <= CHAR_MAX)
        *(char*)buff = nb;
    else if (limit <= UCHAR_MAX)
        *(unsigned char*)buff = nb;
    else if (limit <= SHRT_MAX)
        *(short*)buff = nb;
    else if (limit <= USHRT_MAX)
        *(ushort*)buff = nb;
    else if (limit <= INT_MAX)
        *(int*)buff = nb;
    else if (limit <= UINT_MAX)
        *(unsigned int*)buff = nb;
    else if (limit <= LONG_MAX)
        *(long*)buff = nb;
    return EXIT_SUCCESS;
}

static void     opt_fork_timeout(int scd) {
    __pid_t parent_id = getpid();
    __pid_t pid = fork();
    if (pid == 0) {
        sleep(scd);
        kill(parent_id, SIGINT);
        exit(0);
    }
}

bool            get_opt(t_opt_e opt_value, t_opt_d *data) {
    return data->opt[opt_value];
}

char            *get_opt_arg(t_opt_e opt_value, t_opt_d *data) {
    return data->opt_arg[opt_value];
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

void     opt_handle(t_opt_d *data) {
    for (short i = 0 ; i < _OPT_MAX_NB ; ++i) {
        if (data->opt[i])
            switch(i) {
                case _OPT_w:
                    if (opt_set_num_value(&data->timeout, INT_MAX, data->opt_arg[i]) == EXIT_FAILURE)
                        opt_error_num_handle(data->opt_arg[i], INT_MAX);
                    opt_fork_timeout(data->timeout);
                    break;
                case _OPT_W:
                    if (opt_set_num_value(&data->linger, INT_MAX, data->opt_arg[i]) == EXIT_FAILURE)
                        opt_error_num_handle(data->opt_arg[i], INT_MAX);
                    break;
                case _OPT_TTL:
                    if (opt_set_num_value(&data->ttl, CHAR_MAX, data->opt_arg[i]) == EXIT_FAILURE)
                        opt_error_num_handle(data->opt_arg[i], CHAR_MAX);
                    break;
                case _OPT_S:
                    if (opt_set_num_value(&data->sndbuf, INT_MAX, data->opt_arg[i]) == EXIT_FAILURE)
                        opt_error_num_handle(data->opt_arg[i], INT_MAX);
                    break;
                case _OPT_I:
                    if (!data->opt_arg[i] || ft_strlen(data->opt_arg[i]) < 1 || ft_strlen(data->opt_arg[i]) > IFNAMSIZ)
                        opt_error_handle(data->opt_arg[i], i);
                    if (opt_interface_handle(data->opt_arg[i]) == EXIT_FAILURE)
                        opt_error_handle(data->opt_arg[i], i);
                    break;
            }
    }
}

// -h -v -c -i -f -l -n -w -W -p -r -s -T --ttl --ip-timestamp
error_t     opt_parsing(int key, char *arg, struct argp_state *state) {
    t_opt_d     *option_data = state->input;

    // Temporary
    (void)option_data;
    switch (key) {
        // HELP already implemented in argp
        // VERSION already implemented in argp
        case 'c':
            if (arg != NULL)
                printf("%s/n", arg);
            else
                printf("Empty arg c!\n");
            break;
        case _OPT_TTL:
            if (arg != NULL)
                printf("%s/n", arg);
            else
                printf("Empty arg TTL!\n");
            break;
        case _OPT_IPTIMESTAMP:
            if (arg != NULL)
                printf("%s/n", arg);
            else
                printf("Empty arg iptimestamp!\n");
            break;
        case ARGP_KEY_ARG:
            option_data->addr_raw = arg;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

/// OPT list : < -h -v -f -l -I -m -M -n -w -W -p -Q -S -t -T >
/// Only the first two are mandatory
void            opt_store(char *av[], int ac, t_opt_d *opt_data) {
    struct argp argp;
    
    bzero(&argp, sizeof(struct argp));
    argp.options = options;
    argp.parser = opt_parsing;
    argp.args_doc = "HOST";
    argp_parse(&argp, ac, av, 0, 0, opt_data);
}