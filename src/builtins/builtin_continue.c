#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../utils/shelldon.h"
#include "builtins.h"

int builtin_continue(int argc, char **argv, struct exec_status *status)
{
    if (shelldon.loop_struct->nb_loop == 0)
    {
        return 1;
    }
    if (status->status != EXEC_OK)
        return 0;
    if (argc > 2)
        fprintf(stderr, "Too many args for continue");
    if (argc == 1)
        shelldon.loop_struct->nb_continue = 1;
    else
        shelldon.loop_struct->nb_continue = atoi(argv[1]);
    status->status = EXEC_EXIT;
    return 0;
}
