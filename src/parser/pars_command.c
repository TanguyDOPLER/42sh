#include "parser.h"

/**
 * simple_command=
 *      prefix { prefix }
 *      | { prefix } WORD { element }
 * ;
 */
static struct ast *simple_command(enum parser_status *status,
                                  struct lexer *lexer)
{
    if (*status == PARSER_UNEXPECTED_TOKEN) // cas où le parse crash
        return NULL;
    int prefix_found = 0; // = 1 si on a eu au moins une fois un prefix
    // effectue soit prefix { prefix } soit { prefix }
    struct ast *simple_com = handle_prefix(status, lexer, &prefix_found);

    if (*status != PARSER_OK) // cas d'erreur
        return NULL;

    struct token t = lexer_peek(lexer);

    if (t.type != TOKEN_WORDS)
    {
        if (prefix_found) // si on est dans le cas prefix { prefix }
            return simple_com;
        else // sinon c'est une erreur car on attend un WORD après { prefix }
        {
            fprintf(stderr,
                    "simple_command: Error token: expected WORD, got %d\n",
                    t.type);
            ast_free(simple_com);
            *status = PARSER_UNEXPECTED_TOKEN;
            return NULL;
        }
    }
    if (simple_com == NULL) // cas où l'on a eu aucun prefix
    {
        simple_com = ast_new(AST_SIMPLE_COMMAND);
        simple_com->value = t.value;
        lexer_pop(lexer);
        return handle_elements(status, lexer, simple_com);
    }
    // cas où l'on a au moins un prefix
    /*struct ast *n = simple_com;
    while (n->left != NULL)
    {
        n = n->left;
    }
    n->left = ast_new(AST_SIMPLE_COMMAND);
    n->left->value = t.value;
    lexer_pop(lexer);
    n->left = handle_elements(status, lexer, n->left);*/
    return simple_com;
}

/**
 command =
    simple_command
    shell_command { redirection };
 */
struct ast *command(enum parser_status *status, struct lexer *lexer)
{
    if (*status == PARSER_UNEXPECTED_TOKEN) // cas où le parse crash
        return NULL;
    struct token token = lexer_peek(lexer); // on test si on est sur une simple
                                            // commande ou une shell commande
    if (token.type == TOKEN_IF || token.type == TOKEN_WHILE
        || token.type == TOKEN_UNTIL) // cas shell command
    {
        struct ast *shell_com = shell_command(status, lexer);
        struct ast *n = shell_com; // noeud courant dans l'ast
        token = lexer_peek(lexer);
        while (token.type == TOKEN_IONUMBER
               || (token.type >= TOKEN_REDIR_INPUT
                   && token.type <= TOKEN_REDIR_DOUBLE))
        {
            struct ast *redir = redirection(status, lexer);
            if (*status != PARSER_OK)
            {
                fprintf(stderr, "command : child (redir) gone wrong\n");
                ast_free(shell_com);
                ast_free(redir);
                return NULL;
            }
            if (n->type == AST_SHELL_COMMAND) // première redirection trouvée
            {
                redir->left = shell_com;
                shell_com = redir;
                n = redir;
            }
            else
            {
                redir->left = n->left;
                n->left = redir;
                n = n->left;
            }
            token = lexer_peek(lexer);
        }
        return shell_com;
    }
    return simple_command(status, lexer); // cas simple command
}
