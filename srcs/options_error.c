#include <ft_ping.h>

static void     _opt_error_interface(char *arg) {
    int len = (sizeof(_ERROR_HEADER) * 2) + sizeof(' ') // 'ping: '
            + sizeof(_ERROR_INTERFACE_SRCADDR) + sizeof('\n')
            + sizeof(_ERROR_INTERFACE_IFACE)
            + (IF_NAMESIZE * 2) + 1;
    char err_interface[len];

    ft_bzero(err_interface, len);
    ft_strlcat(err_interface, _ERROR_HEADER, len);
    ft_strlcat(err_interface, " ", len);
    ft_strlcat(err_interface, _ERROR_INTERFACE_SRCADDR, len);
    ft_strlcat(err_interface, arg, len);
    ft_strlcat(err_interface, "\n", len);

    ft_strlcat(err_interface, _ERROR_HEADER, len);
    ft_strlcat(err_interface, " ", len);
    ft_strlcat(err_interface, _ERROR_INTERFACE_IFACE, len);
    ft_strlcat(err_interface, arg, len);
    error_handle(_EX_NOERRNO, err_interface);
}

void    opt_error_handle(t_opt_err errnum, char *arg, int exnum) {
    switch (errnum) {
        case OPT_ERR_INVALID:
            fprintf(stderr, "invalid value (`%s' near `%s')\n", arg, arg);
            break;
        case OPT_ERR_IFA:
            _opt_error_interface(arg);
            break;
        case OPT_ERR_TOOSMALL:
            fprintf(stderr, "option value too small: %i\n", ft_atoi(arg));
            break;
        case OPT_ERR_TOOBIG:
            fprintf(stderr, "option value too big: %i\n", ft_atoi(arg));
            break;
        case OPT_ERR_PATTERN:
            fprintf(stderr, "error in pattern near %s\n", arg);
            break;
        default:
            break;
    }    
    exit(exnum);
}