#ifndef EXEC_H
#define EXEC_H

#include "ast/ast.h"
#include "builtins/builtins.h"

#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

int exec_ast(struct ast *ast);
int exec_list(struct ast *ast);
int exec_and_or(struct ast *ast);
int exec_simple_command(struct ast *ast);
int exec_shell_command(struct ast *ast);


#endif /* !EXEC_H */

