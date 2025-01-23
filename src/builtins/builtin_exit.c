#include <stdlib.h>
#include <unistd.h>

#include "builtins.h"

int builtin_exit(char **argv, struct exec_status *status)
{
    if (argv[1] == NULL)
        status->exit_value = 0;
    else
        status->exit_value = atoi(argv[1]); // set up return number of exit
    status->status = EXEC_EXIT; // set up state of struct
    dup2(3333, STDERR_FILENO); // on ferme stderr prcq rien ne doit
    // se passer ou s'afficher lors de l'appelle de exit
    // on le remet en place dans main.c
    close(STDERR_FILENO);
    return 0;
}
