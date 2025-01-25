#define _POSIX_C_SOURCE 200112L

#include <stdlib.h>

#include "builtins.h"

int builtin_export(char **argv)
{
    char *name = argv[1];
    char *value = argv[2];
    if (value == NULL)
        value = value_of_variable(name);
    setenv(name, value, 1);
    return 0;
}
