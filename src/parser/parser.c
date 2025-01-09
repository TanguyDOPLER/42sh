#define _POSIX_C_SOURCE 200809L

#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * input=   list '\n'
 *      | list EOF
 *      | '\n'
 *      | EOF;
 */
struct ast *parse(enum parser_status *status, struct lexer *lexer)
{
    struct token t = lexer_pop(lexer);
    if (t.type == TOKEN_EOL || t.type == TOKEN_EOF)
        return NULL;

    struct ast *res = list(status, lexer);
    if (*status != PARSER_OK)
    {
        ast_free(res);
        return NULL;
    }
    t = lexer_peek(lexer);
    if (t.type != TOKEN_EOL && t.type != TOKEN_EOF)
    {
        fprintf(stderr, "parse: Error token: expected EOF or EOL, got %d\n",
                t.type);
        *status = PARSER_UNEXPECTED_TOKEN;
        ast_free(res);
        return NULL;
    }
    return res;
}

/**
 * list = and_or { ';' and_or } [ ';' ] ;
 */
struct ast *list(enum parser_status *status, struct lexer *lexer)
{
    // fonction qui parse une liste. fonctionnement AST : fils gauche frère
    // droit
    if (*status == PARSER_UNEXPECTED_TOKEN) // cas où le parse crash
        return NULL;
    struct ast *ast = ast_new(AST_LIST); // initialisation struct ast
    ast->left =
        and_or(status, lexer); // on aura forcement un "and_or" au début. Donc
                               // c'est le fils donc on le met à gauche.
    if (ast->left == NULL) // Si on a pas de and_or c'est que le parser a
                           // planté. Il faudra ptet mettre à jour status ici
        return NULL;
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
        }
        token = lexer_peek(lexer);
    }
    return ast; // retour de l'ast complet
}

/**
 * and_or = pipeline ;
 */
struct ast *and_or(enum parser_status *status, struct lexer *lexer)
{
    return pipeline(status, lexer);
}

/**
 *  pipeline = command ;
 */
struct ast *pipeline(enum parser_status *status, struct lexer *lexer)
{
    return command(status, lexer);
}

/**
 command =
    simple_command
    shell_command;
 */
struct ast *command(enum parser_status *status, struct lexer *lexer)
{
    if (*status == PARSER_UNEXPECTED_TOKEN) // cas où le parse crash
        return NULL;
    struct token token = lexer_peek(lexer); // on test si on est sur une simple
                                            // commande ou une shell commande
    if (token.type == TOKEN_IF) // cas shell command
    {
        return shell_command(status, lexer);
    }
    else // cas simple command
    {
        return simple_command(status, lexer);
    }
}

/*
 * (* for the time being, it is limited to a single rule_if *)
 *   shell_command = rule_if ;
 */

struct ast *shell_command(enum parser_status *status, struct lexer *lexer)
{
    return rule_if(status, lexer);
}

/*
 * rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi' ;
 */

struct ast *rule_if(enum parser_status *status, struct lexer *lexer)
{
    if (*status == PARSER_UNEXPECTED_TOKEN) // cas où le parse crash
        return NULL;
    struct ast *ast = NULL;
    struct token token = lexer_peek(lexer); // on test si on est sur une simple
                                            // commande ou une shell commande
    // free(token.value);
    // free(lexer_pop(lexer).value);//on fait sauter le token "if" car
    // maintenant on s'en fou + free buffer
    if (token.type == TOKEN_IF) // cas shell command
    {
        lexer_pop(lexer); // on fait sauter le token "if " car maintenant on
                          // s'en fou + free buffer + check
        ast = ast_new(AST_SHELL_COMMAND); // on crée l'ast final
        ast->condition = compound_list(status, lexer); // on setup la condition
        token = lexer_peek(lexer); // on check si on a bien un "then" à la suite
        lexer_pop(lexer); // on fait sauter le token "then" car maintenant on
                          // s'en fou + check
        if (token.type != TOKEN_THEN)
        {
            *status = PARSER_UNEXPECTED_TOKEN;
            return NULL;
        }
        ast->left =
            compound_list(status, lexer); // fils gauche = condition vérifié
        token = lexer_peek(
            lexer); // check next token pour savoir si on a un else ou pas
        if (token.type == TOKEN_ELSE || token.type == TOKEN_ELIF)
        {
            ast->right = else_clause(
                status,
                lexer); // on call la fonction qui s'occupe du else et elif
            token = lexer_peek(lexer); // on récupère ici le "fi" normalement
        }
        if (token.type == TOKEN_FI)
        {
            lexer_pop(lexer); // on dégage le token fi
            return ast; // on a finit le if, on retourne tout le bordel
            // attention, ici on est dans le cas ou il n'y a pas de else
            //  On finit le if avec else en dessous
        }
        else
        { // ici gestion erreur
            *status = PARSER_UNEXPECTED_TOKEN;
            return NULL;
        }
    }
    else
    { // gestion erreur, on a pas le mot clef "if" au début du if
        *status = PARSER_UNEXPECTED_TOKEN;
        return NULL;
    }
    return ast; // on retourne l'ast global. (c'est ici qu'on retourne un if
                // avec else)
}

/*
 * else_clause =
 *    'else' compound_list
 *  | 'elif' compound_list 'then' compound_list [else_clause];
 */

struct ast *else_clause(enum parser_status *status, struct lexer *lexer)
{
    if (*status == PARSER_UNEXPECTED_TOKEN) // cas où le parse crash
        return NULL;
    struct token token = lexer_peek(lexer); // on test si on est sur une simple
                                            // commande ou une shell commande
    lexer_pop(lexer); // on dégage le else ou elif
    if (token.type == TOKEN_ELSE) // cas simple else
    {
        return compound_list(status, lexer);
    }
    if (token.type == TOKEN_ELIF) // cas else + if = elif
    {
        struct ast *ast =
            ast_new(AST_SHELL_COMMAND); // faire un ast shell commande ici et
                                        // faire un if sans verif des mots clef
        ast->condition = compound_list(status, lexer); // set up la condition
        lexer_pop(lexer); // free le token "then" ;
        ast->left = compound_list(status, lexer);
        ast->right = else_clause(status, lexer);
        return ast;
    }
    *status = PARSER_UNEXPECTED_TOKEN; // cas où token != elif et else
    return NULL;
}

/**
 * simple_command = WORD { element } ;
 */
struct ast *simple_command(enum parser_status *status, struct lexer *lexer)
{
    if (*status == PARSER_UNEXPECTED_TOKEN) // cas où le parse crash
        return NULL;
    struct token t = lexer_peek(lexer); // test si on est sur un word
    if (t.type != TOKEN_WORDS)
    {
        fprintf(stderr, "simple_command: Error token: expected WORD, got %d\n",
                t.type);
        *status = PARSER_UNEXPECTED_TOKEN;
        return NULL;
    }
    struct ast *command = ast_new(AST_SIMPLE_COMMAND); // init ast global
    command->value = t.value;
    struct ast *node = command; // struct ast temp
    t = lexer_pop(
        lexer); // on passe au token suivant prcq on veut plus le premier
    while (t.type == TOKEN_WORDS) // boucle pour parcourir les arguments
    {
        struct ast *child = element(
            status, lexer); // on pop la dedans, donc on obtiens la suite
        if (*status != PARSER_OK)
        {
            ast_free(command);
            return NULL;
        }
        node->left = child; // on met à jour la structure ast
        node = node->left;
        t = lexer_pop(lexer); // on test si on est toujours sur un word
    }
    return command;
}

/**
 * element = WORD ;
 */
struct ast *element(enum parser_status *status, struct lexer *lexer)
{
    if (*status == PARSER_UNEXPECTED_TOKEN) // cas où le parse crash
        return NULL;
    struct token t = lexer_peek(lexer); // touche pas au peek ici connard
    if (t.type != TOKEN_WORDS)
    {
        fprintf(stderr, "element: Error token: expected WORD, got %d\n",
                t.type);
        *status = PARSER_UNEXPECTED_TOKEN;
        // free(t.value);
        return NULL;
    }
    struct ast *elt = ast_new(AST_ARGUMENTS);
    elt->value = t.value; // strdup(t.value); // copie la valeur du token dans
                          // celle du nouveau noeud (ast->value = t.value)
    return elt;
}

/**
 * compound_list = {'\n'} and_or { ( ';' | '\n' ) {'\n'} and_or } [';'] {'\n'} ;
 */
struct ast *compound_list(enum parser_status *status, struct lexer *lexer)
{
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
        *status = PARSER_UNEXPECTED_TOKEN;
        ast_free(compound);
        free(t.value); // on garde ca au cas où c'est un word
        return NULL;
    }
    compound->left = child;
    t = lexer_peek(lexer);
    while (t.type == TOKEN_SEMI_COLON || t.type == TOKEN_EOL)
    {
        t = lexer_pop(lexer);
        while (t.type == TOKEN_EOL)
        {
            t = lexer_pop(lexer);
        }

        if ((t = lexer_peek(lexer)).type
            == TOKEN_WORDS) // cas où on a un and_or et non juste un point
                            // virgule
        {
            child = and_or(status, lexer);
            if (*status != PARSER_OK)
            {
                ast_free(compound);
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
    }
    t = lexer_peek(lexer);
    while (t.type == TOKEN_EOL)
    {
        t = lexer_pop(lexer);
    }
    return compound;
}
