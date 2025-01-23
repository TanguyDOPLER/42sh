#include "ast.h"

#include <err.h>
#include <stdlib.h>

struct ast *ast_new(enum ast_type type)
{
    struct ast *new = calloc(1, sizeof(struct ast));
    if (!new)
        return NULL;
    new->type = type;
    return new;
}

void ast_free(struct ast *ast)
{
    if (ast == NULL)
        return;

    ast_free(ast->left);
    ast->left = NULL;

    ast_free(ast->right);
    ast->right = NULL;

    if (ast->type == AST_ARGUMENTS || ast->type == AST_SIMPLE_COMMAND
        || (ast->type >= AST_REDIR_INPUT && ast->type <= AST_REDIR_DOUBLE)
        || ast->type == AST_ASSIGNMENT_WORD)
        free(ast->value); // free les fameux memory leaks de maxime et son
                          // buffer :)

    ast_free(ast->condition);
    ast->condition = NULL;

    if (ast->len_values > 0)
        free(ast->for_values);

    free(ast);
}
