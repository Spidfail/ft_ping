#include <argp.h>
#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <stdbool.h>
#include <string.h>

typedef struct mysavings {
    bool toto;
    char *arg;
} t_my;

static struct argp_option options[] = {
    {
        "toto",
        't',
        0,
        0,
        "Toto will say Hello!",
        0
    },
    {0}
};

error_t     parsing_fun(int key, char *arg, struct argp_state *state) {
    t_my    *mine = state->input;

    switch (key) {
        case 't':
            printf("Hello World from Toto!\n");
            mine->toto = true;
            if (arg != NULL)
                mine->arg = arg;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

int     main(int ac, char **av) {
    struct argp         argp;
    t_my                save;
    
    bzero(&argp, sizeof(argp));
    bzero(&save, sizeof(save));
    argp.options = options;
    argp.parser = parsing_fun;
    // TODO
    argp_parse(&argp, ac, av, 0, 0, &save);
    if (save.toto == true)
        printf("Hello Toto!\n");
}