#ifndef AST_H
#define AST_H

#include <unistd.h>

enum ast_type
{
    AST_LIST, // fils gauche, frère droit
    AST_AND,
    AST_OR,
    AST_NEGATION,
    AST_SIMPLE_COMMAND, // premier word = commande, word suivant = arguments.
                        // Tous les arguments sont à gauche en peigne
    AST_SHELL_COMMAND, // pour l instant c equivalent à un if, gauche true,
                       // droite false ou else. Si droit null -> pas de else
    AST_ARGUMENTS,
    AST_PIPE, // meme structure qu'une liste (fils gauche frr droit)
    AST_REDIR_INPUT, //<
    AST_REDIR_OUTPUT, //> et >|
    AST_REDIR_AP_OUTPUT, //>>
    AST_REDIR_DUP_INPUT, //>&
    AST_REDIR_DUP_OUTPUT, //<&
    AST_REDIR_DOUBLE,
    AST_WHILE,
    AST_UNTIL,
    AST_FOR,
    AST_ASSIGNMENT_WORD
};

struct ast
{
    enum ast_type type;
    char *value;
    int io_type;
    char **for_values;
    int len_values;
    struct ast *condition;
    struct ast *left;
    struct ast *right;
};

struct ast *ast_new(enum ast_type type);

void ast_free(struct ast *ast);

#endif /* !AST_H */
