#include "shelldon.h"

#include <stddef.h>

#include "variable.h"

// initialise shelldon
void shelldon_init(void)
{
    // Allouer de la mémoire pour une variable
    shelldon.var = NULL;
}

// free shelldon (shelldon au cimetière *snif*)
void shelldon_free(void)
{
    variable_destroy(shelldon.var);
}
