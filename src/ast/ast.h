#ifndef AST_H
#define AST_H

#include <unistd.h>

enum ast_type
{
AST_LIST, //fils gauche, frère droit
AST_AND_OR, //step deux, on s en fou pour l instant
PIPELINE, //step deux, on s en fou pour l instant
AST_SIMPLE_COMMAND, // premier word = commande, word suivant = arguments. Tous les arguments sont à gauche en peigne
AST_SHELL_COMMAND, // pour l instant c equivalent à un if, gauche true, droite false ou else. Si droit null -> pas de else
AST_ARGUMENTS
};

struct ast
{
    enum ast_type type;
    char *value;
    struct ast *condition;
    struct ast *left;
    struct ast *right;
};

struct ast *ast_new(enum ast_type type);

void ast_free(struct ast *ast);

#endif /* !AST_H */

