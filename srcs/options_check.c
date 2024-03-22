#include <libft.h>
#include <ft_ping.h>

int      option_check_is_digit(char *argument) {
    for (size_t i = 0 ; i < ft_strlen(argument) ; ++i) {
        if (!ft_isdigit(argument[i]))
            return 1;
    }
    return 0;
}

int     option_check_is_alnum(char *argument) {
    for (size_t i = 0 ; i < ft_strlen(argument) ; ++i)
        if (!ft_isalnum(argument[i]))
            return 1;
    return 0;
}

int     option_check_is_hex(char *argument) {
    if (option_check_is_alnum(argument) == 0) {
        for (size_t i = 0 ; argument[i] ; ++i) {
            if (ft_isdigit(argument[i]))
                continue;
            else if (argument[i] >= 'a' && argument[i] <= 'f')
                continue;
            else if (argument[i] >= 'A' && argument[i] <= 'F')
                continue;
            return 1;
        }
        return 0;
    }
    return 1;
}