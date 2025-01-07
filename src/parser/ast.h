#ifndef AST_H
#define AST_H

#include <unistd.h>

enum ast_type
{
    AST_LIST, // fils gauche éléments, frère droit suite de la liste
    AST_AND_OR, // step 2
    AST_PIPELINE, // step 2
    AST_SIMPLE_COMMAND, // commande, suivi par ses arguments en fils gauche
    AST_SHELL_COMMAND, // 
    AST_ARGUMENT // argument 
};

struct ast
{
    enum ast_type type;
    char *value;
    struct ast *left;
    struct ast *right;
};

struct ast *ast_new(enum ast_type type);

void ast_free(struct ast *ast);

#endif /* !AST_H */

