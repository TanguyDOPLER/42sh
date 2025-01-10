#ifndef EXEC_H
#define EXEC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../ast/ast.h"
#include "../builtins/builtins.h"

int exec_ast(struct ast *ast);
int exec_list(struct ast *ast);
int exec_and_or(struct ast *ast);
int exec_simple_command(struct ast *ast);
int exec_shell_command(struct ast *ast);

#endif /* !EXEC_H */
