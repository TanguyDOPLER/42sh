#include "shelldon.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "function.h"
#include "variable.h"

// renvoie le chemin du dossier dans lequel on se trouve actuellement
static char *get_current_directory(void)
{
    size_t size = 128;
    char *buffer = malloc(sizeof(char) * size);
    if (buffer == NULL)
    {
        fprintf(stderr, "get_current_directory : malloc failed\n");
        return NULL;
    }
    if (getcwd(buffer, size) != NULL)
    {
        return buffer;
    }

    fprintf(stderr, "get_current_directory : getcwd failed\n");
    free(buffer);
    return NULL;
}

// met à jour $?
void update_res(int res)
{
    char *value = calloc(8, sizeof(char));

    snprintf(value, 8, "%d", res);
    char *name = calloc(2, sizeof(char));
    strcpy(name, "?");
    variable_add(name, value);
}

void special_var_init(void)
{
    // utile pour la variable $RANDOM
    srand(time(NULL));

    char *value;
    char *name;

    // initialisation de $?
    value = calloc(2, sizeof(char));
    value[0] = '0';
    name = calloc(2, sizeof(char));
    strcpy(name, "?");
    variable_add(name, value);

    // initialisation de $PWD
    value = get_current_directory();
    name = calloc(4, sizeof(char));
    strcpy(name, "PWD");
    variable_add(name, value);

    // initialisation de $OLDPWD
    value = calloc(1, sizeof(char));
    name = calloc(7, sizeof(char));
    strcpy(name, "OLDPWD");
    variable_add(name, value);
}

// initialise shelldon
void shelldon_init(void)
{
    shelldon.var = NULL; // contient les variables (spéciales inclues, sauf $@)
    shelldon.list_args = NULL; // = $@
    shelldon.len_list_args = 0; // taille de $@
    special_var_init(); // initialisation des variables spéciales
    shelldon.function = NULL;
}

// free shelldon (shelldon au cimetière *snif*)
void shelldon_free(void)
{
    variable_destroy(shelldon.var);
    function_destroy(shelldon.function);
}
