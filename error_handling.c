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