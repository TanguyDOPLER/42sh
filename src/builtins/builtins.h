#ifndef BUILTINS_H
#define BUILTINS_H

#include <stdio.h>
#include <stdlib.h>

#include "../utils/variable.h"

enum exit_status
{
    EXEC_OK,
    EXEC_CRASHED,
    EXEC_EXIT
};

struct exec_status
{
    enum exit_status status;
    int exit_value;
};

int builtin_true(void);
int builtin_false(void);
int builtin_echo(int argc, char *argv[]);
int builtin_exit(char **argv, struct exec_status *status);
int builtin_cd(char *directory_operand);
int builtin_dot(char *path);
int exec_for_builtin_dot(char *argv);
int builtin_unset(int argc, char **argv);
int builtin_export(char **argv);

#endif /* !BUILTINS_H */
