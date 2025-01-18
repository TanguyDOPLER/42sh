#ifndef EXEC_H
#define EXEC_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../ast/ast.h"
#include "../builtins/builtins.h"

int exec_ast(struct ast *ast);
int exec_redir(struct ast *ast);

#endif /* !EXEC_H */
