#include "ft_ping.h"

int error_handle(int errnum, char *err_value) {
    switch (errnum) {
        case EX_NOHOST :
            fprintf(stderr, "%s %s: %s: Unknown host\n", _ERROR_HEADER, _ERROR_RESOLVE, err_value);
            break;
        case EX_USAGE :
            fprintf(stderr, "usage: %s\n", _ERROR_USAGE);
            break;
        default :
            perror(err_value);
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