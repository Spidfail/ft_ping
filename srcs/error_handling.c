#include <ft_ping.h>
#include <string.h>

char    *error_icmp_mapping(int type, int code) {
    for (int i = 0 ; i < ICMP_ERROR_MAXNB ; ++i)
        if (g_icmp_error[i].type == type && g_icmp_error[i].code == code)
            return g_icmp_error[i].description;
    return NULL;
}

int error_handle(int errnum, char *err_value) {
    switch (errnum) {
        case EX_NOHOST :
            fprintf(stderr, "%s %s: %s: Unknown host\n", _ERROR_HEADER, _ERROR_RESOLVE, err_value);
            break;
        case EX_USAGE :
            if (err_value)
                fprintf(stderr, "%s Wrong arguments\n%s\n", _ERROR_HEADER, err_value);
            else
                fprintf(stderr, "%s usage error: %s\n", _ERROR_HEADER, _ERROR_USAGE);
            break;
        case _EX_NOERRNO:
            fprintf(stderr, "%s\n", err_value);
            break;
        default :
            fprintf(stderr, "%s %s: %s\n",_ERROR_HEADER, err_value, strerror(errno));
    }
    exit(errnum);
}

void    error_gai_handle(char *input, int8_t ec) {
    // DEB11 version
    fprintf(stderr, "ping: %s: %s\n", input, gai_strerror(ec));
    // MACOS version
    // fprintf(stderr, "ping: cannot resolve %s: %s\n", input, gai_strerror(ec));
    exit(ec);
}