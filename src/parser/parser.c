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

struct ast *compound_list(enum parser_status *status, struct lexer *lexer)
{ // dans cette fonction on vérifie si il y a des ";" ou des "\n" tout le temps
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

        if ((t = lexer_peek(lexer)).type == TOKEN_WORDS)
        // cas où on a un and_or et non juste un point
        // virgule
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
        lexer_pop(lexer);
        flag_semicol_newline = 1;
    }
    t = lexer_peek(lexer);
    while (t.type == TOKEN_EOL)
    {
        flag_semicol_newline = 1;
        t = lexer_pop(lexer);
    }
    if (flag_semicol_newline == 0)
    {
        *status = PARSER_UNEXPECTED_TOKEN;
        fprintf(stderr, "compound_list : expected semi colon or \\n \n");
    }
    return compound;
}

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
    ast->left =
        and_or(status, lexer); // on aura forcement un "and_or" au début. Donc
                               // c'est le fils donc on le met à gauche.
    if (ast->left == NULL
        || *status == PARSER_UNEXPECTED_TOKEN) // Si on a pas de and_or c'est
                                               // que le parser a
    { // planté. Il faudra ptet mettre à jour status ici
        *status = PARSER_UNEXPECTED_TOKEN;
        fprintf(stderr, "list : ast.left (and_or) gone wrong\n");
        return ast;
    }
    struct ast *actual = ast; // variable temporaire pour parcourir les fils
                              // gauche, soit les frère
    struct token token = lexer_peek(
        lexer); // variable contenant le token qu'on étudie actuellement
    while (
        token.type
        == TOKEN_SEMI_COLON) // boucle -> check grammar : { ';' and_or } [ ';' ]
    {
        if ((token = lexer_pop(lexer)).type
            == TOKEN_WORDS) // cas où on a un and_or et non juste un point
                            // virgule
        {
            actual->right = ast_new(AST_LIST); // on rajoute un frère
            actual = actual->right; // on actualise la variable actual
            actual->left =
                and_or(status,
                       lexer); // on met la valeur du and_or dans le fils gauche
            if (*status
                == PARSER_UNEXPECTED_TOKEN) // le parser a planté. Il faudra
                                            // ptet mettre à jour status ici
            {
                fprintf(
                    stderr,
                    "list : actual.left(and_or) in loop while gone wrong\n");
                return ast;
            }
        }
        if (token.type == TOKEN_SEMI_COLON)
        {
            *status = PARSER_UNEXPECTED_TOKEN;
            fprintf(stderr, "list : Got two semicolon together.\n");
            return ast;
        }
        token = lexer_peek(lexer);
    }
    return ast; // retour de l'ast complet
}

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
