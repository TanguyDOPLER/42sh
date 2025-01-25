#include <stdio.h>
#include <string.h>

#include "builtins.h"

int builtin_unset(int argc, char **argv)
{
    if (argc == 3)
        variable_pop(argv[1]);
    else
    {
        if (!strcmp(argv[1], "-v"))
            variable_pop(argv[2]);
        else
        {
            fprintf(stderr, "builtin_unset : wrong option\n");
            return 2;
        }
    }
    return 0;
}
