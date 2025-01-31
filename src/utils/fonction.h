#ifndef FONCTION_H
#define FONCTION_H

#include "ast/ast.h"

struct function
{
    char *name;
    struct ast *ast;
    struct function *next;
};

int function_add(char *name, struct ast *ast);
void function_destroy(struct function *function);
struct ast *is_function(char *name);

#endif /* ! FONCTION_H */
