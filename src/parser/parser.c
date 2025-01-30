#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * compound_list = {'\n'} and_or { ( ';' | '\n' ) {'\n'} and_or } [';'] {'\n'} ;
 */

static void error_copound_list(struct ast *compound, struct ast *child)
{
    fprintf(stderr, "compound_list : return of child (and_or) is wrong\n");
    ast_free(compound);
    ast_free(child);
}

static void error_if_smth(int flag_semicol_newline, enum parser_status *status,
                          struct ast **ast)
{
    if (flag_semicol_newline == 0)
    {
        *status = PARSER_UNEXPECTED_TOKEN;
        ast_free(*ast);
        *ast = NULL;
        fprintf(stderr, "compound_list : expected semi colon or \\n \n");
    }
}

static int condition_compound_list(struct token t)
{
    return (t.use == TOKEN_WORDS || t.use == TOKEN_IF || t.use == TOKEN_WHILE
            || t.use == TOKEN_UNTIL || t.use == TOKEN_FOR
            || t.type == TOKEN_ASSIGNMENT_WORD || t.type == TOKEN_NEGATION
            || t.use == TOKEN_IONUMBER
            || (t.type >= TOKEN_REDIR_INPUT && t.type <= TOKEN_REDIR_DOUBLE));
}

struct ast *compound_list(enum parser_status *status, struct lexer *lexer)
{ // dans cette fonction on vérifie si il y a
  // des ";" ou des "\n" tout le temps
    // meme si la grammair dit autre chose car c'est ici qu'on vérifie les
    //";" dans les if
    if (*status == PARSER_UNEXPECTED_TOKEN) // cas où le parse crash
        return NULL;
    struct token t = lexer_peek(lexer);
    while (t.type == TOKEN_EOL)
    {
        t = lexer_pop(lexer);
    }
    struct ast *compound = ast_new(AST_LIST);
    struct ast *n = compound;
    struct ast *child = and_or(status, lexer);
    if (*status != PARSER_OK)
    {
        error_copound_list(compound, child);
        return NULL;
    }
    compound->left = child;
    int flag_semicol_newline = 0;
    t = lexer_peek(lexer);
    while (t.type == TOKEN_SEMI_COLON || t.type == TOKEN_EOL)
    {
        flag_semicol_newline = 1;
        t = lexer_pop(lexer);
        while (t.type == TOKEN_EOL)
        {
            t = lexer_pop(lexer);
        }
        if (t.use == TOKEN_DO || t.use == TOKEN_DONE)
            break;
        if (condition_compound_list(t))
        {
            child = and_or(status, lexer);
            if (*status != PARSER_OK)
            {
                error_copound_list(compound, child);
                return NULL;
            }
            n->right = ast_new(AST_LIST);
            n = n->right;
            n->left = child;
        }

        t = lexer_peek(lexer);
    }
    if (t.type == TOKEN_SEMI_COLON)
    {
        t = lexer_pop(lexer);
        flag_semicol_newline = 1;
    }
    while (t.type == TOKEN_EOL)
    {
        flag_semicol_newline = 1;
        t = lexer_pop(lexer);
    }
    error_if_smth(flag_semicol_newline, status, &compound);
    return compound;
} // 38

/**
 * list = and_or { ';' and_or } [ ';' ] ;
 */
static struct ast *list(enum parser_status *status, struct lexer *lexer)
{
    // fonction qui parse une liste. fonctionnement AST : fils gauche frère
    // droit
    if (*status == PARSER_UNEXPECTED_TOKEN) // cas où le parse crash
        return NULL;
    struct ast *ast = ast_new(AST_LIST); // initialisation struct ast
    ast->left = and_or(status, lexer);
    if (ast->left == NULL || *status == PARSER_UNEXPECTED_TOKEN)
    // Si on a pas de and_or c'est
    // que le parser a
    { // planté. Il faudra ptet mettre à jour status ici
        *status = PARSER_UNEXPECTED_TOKEN;
        fprintf(stderr, "list : ast.left (and_or) gone wrong\n");
        return ast;
    }
    struct ast *actual = ast; // variable temporaire pour parcourir les fils
                              // gauche, soit les frère
    struct token token = lexer_peek(lexer);
    while (token.type == TOKEN_SEMI_COLON) // boucle -> check grammar
    {
        token = lexer_pop(lexer);
        if (token.type == TOKEN_WORDS || token.type == TOKEN_ASSIGNMENT_WORD
            || token.type == TOKEN_NEGATION || token.use == TOKEN_IONUMBER
            || (token.type >= TOKEN_REDIR_INPUT
                && token.type <= TOKEN_REDIR_DOUBLE))
        // cas où on a un and_or et non juste un point virgule
        {
            actual->right = ast_new(AST_LIST); // on rajoute un frère
            actual = actual->right; // on actualise la variable actual
            actual->left = and_or(status, lexer);
            if (*status == PARSER_UNEXPECTED_TOKEN)
            // ptet mettre à jour status ici
            {
                fprintf(stderr, "list:actual.left(and_or) in while issue\n");
                return ast;
            }
        }
        else if (token.type == TOKEN_SEMI_COLON)
        {
            *status = PARSER_UNEXPECTED_TOKEN;
            fprintf(stderr, "list : Got two semicolon together.\n");
            return ast;
        }
        token = lexer_peek(lexer);
    }
    return ast; // retour de l'ast complet
} // 29

/**
 * input=   list '\n'
 *      | list EOF
 *      | '\n'
 *      | EOF;
 */
struct ast *parse(enum parser_status *status, struct lexer *lexer)
{
    if (*status
        == PARSER_UNEXPECTED_TOKEN) // cas où le parse crash (ca peut avoir
                                    // crash sur un appelle précédent)
        return NULL;
    struct token t = lexer_pop(lexer);
    if (t.type == TOKEN_EOL || t.type == TOKEN_EOF || t.type == TOKEN_COMMENT)
        return NULL;

    struct ast *res = list(status, lexer);
    if (*status != PARSER_OK)
    {
        fprintf(stderr, "parse : result(list) gone wrong\n");
        ast_free(res);
        return NULL;
    }
    t = lexer_peek(lexer);
    if (t.value)
        free(t.value);
    if (t.type != TOKEN_EOL && t.type != TOKEN_EOF && t.type != TOKEN_COMMENT)
    {
        fprintf(stderr, "parse: Error token: expected EOF or EOL, got %d\n",
                t.type);
        *status = PARSER_UNEXPECTED_TOKEN;
        ast_free(res);
        return NULL;
    }
    return res;
}
