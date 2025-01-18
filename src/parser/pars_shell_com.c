#include <stdio.h>
#include <string.h>

#include "parser.h"

/*
 * else_clause =
 *    'else' compound_list
 *  | 'elif' compound_list 'then' compound_list [else_clause];
 */

static struct ast *else_clause(enum parser_status *status, struct lexer *lexer)
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
        if (lexer_peek(lexer).type != TOKEN_THEN) // check token then
        {
            *status = PARSER_UNEXPECTED_TOKEN;
            fprintf(stderr,
                    "else_clause : Epexted token \"then\" but got something "
                    "else\n");
        }
        lexer_pop(lexer); // on dégage le then
        ast->left = compound_list(
            status,
            lexer); // on ressort direct ici si status = PARSER_UNEXPECTED_TOKEN
        if (lexer_peek(lexer).type != TOKEN_FI)
            ast->right = else_clause(status, lexer);
        return ast;
    }
    *status = PARSER_UNEXPECTED_TOKEN; // cas où token != elif et else
    return NULL;
}

/*
 * rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi' ;
 */

static struct ast *rule_if(enum parser_status *status, struct lexer *lexer)
{
    if (*status == PARSER_UNEXPECTED_TOKEN) // cas où le parse crash
        return NULL;
    struct ast *ast = NULL;
    struct token token = lexer_peek(lexer);
    if (token.type == TOKEN_IF) // cas shell command
    {
        lexer_pop(lexer); // on fait sauter le token "if " car maintenant on
                          // s'en fou + free buffer
        ast = ast_new(AST_SHELL_COMMAND); // on crée l'ast final
        ast->condition = compound_list(status, lexer); // on setup la condition
        token = lexer_peek(lexer); // on check si on a bien un "then" à la suite
        if (token.type != TOKEN_THEN || *status == PARSER_UNEXPECTED_TOKEN)
        {
            *status = PARSER_UNEXPECTED_TOKEN;
            fprintf(stderr,
                    "rule_if : Epexted token \"then\" but got something else "
                    "or ast.condition (compound_list) gone wrong\n");
            return ast;
        }
        lexer_pop(lexer); // on fait sauter le token "then" car maintenant on
                          // s'en fou
        ast->left = compound_list(status, lexer);
        // fils gauche = condition vérifié
        token = lexer_peek(lexer);
        // check next token pour savoir si on a un else ou pas
        if (*status == PARSER_OK
            && (token.type == TOKEN_ELSE || token.type == TOKEN_ELIF))
        {
            ast->right = else_clause(status, lexer);
            // on call la fonction qui s'occupe du else et elif
            token = lexer_peek(lexer); // on récupère ici le "fi" normalement
        }
        if (*status == PARSER_OK && (token.type == TOKEN_FI))
        {
            lexer_pop(lexer); // on dégage le token fi
        }
        else
        { // ici gestion erreur
            fprintf(stderr,
                    "rule_if : Epexted token \"fi\" but got something else or "
                    "ast.left/right (compound_list/else_clause) gone wrong\n");
            *status = PARSER_UNEXPECTED_TOKEN;
        }
    }
    else
    { // gestion erreur, on a pas le mot clef "if" au début du if
        *status = PARSER_UNEXPECTED_TOKEN;
        fprintf(stderr,
                "rule_if : Epexted token \"if\" but got something else\n");
    }
    return ast; // on retourne l'ast global.
}

static struct ast *parse_while_until(enum parser_status *status,
                                     struct lexer *lexer)
{
    struct token token = lexer_peek(lexer);
    if (token.type != TOKEN_WHILE
        && token.type != TOKEN_UNTIL) // check si on a bien le word "while"
    {
        *status = PARSER_UNEXPECTED_TOKEN;
        return NULL;
    }
    lexer_pop(lexer);
    struct ast *result = ast_new(AST_WHILE);
    if (token.type == TOKEN_UNTIL)
        result->type = AST_UNTIL;
    result->condition =
        compound_list(status, lexer); // on met à jour la condition du while
    if (*status == PARSER_UNEXPECTED_TOKEN
        || lexer_peek(lexer).type
            != TOKEN_DO) // check si on a bien le word "do"
    {
        *status = PARSER_UNEXPECTED_TOKEN;
        return NULL;
    }
    lexer_pop(lexer); // on dégage le mot "do"
    result->left = compound_list(
        status, lexer); // le contenu du while est stocké dans le frr gauche
    if (*status == PARSER_UNEXPECTED_TOKEN
        || lexer_peek(lexer).type
            != TOKEN_DONE) // check si on a bien le word "done"
    {
        *status = PARSER_UNEXPECTED_TOKEN;
        return NULL;
    }
    lexer_pop(lexer); // on dégage le mot "done"
    return result;
}

/*
 * rule_for =
 * 'for' WORD ( [';'] | [ {'\n'} 'in' { WORD } ( ';' | '\n' ) ] ) {'\n'} 'do'
 * compound_list ↪→'done' ;
 */
/*
=======

>>>>>>> 6c52223d85c95d2a786a0a89c0d2d5b4fe6d64a8
static void first_step_for(enum parser_status *status, struct lexer *lexer)
{ // fonction pour passer les ";" et "\n" avant les deuxième word dans la
  // grammair de for
    // il vérifie aussi qu'il y les bon charactères.
    struct token token = lexer_peek(lexer);
    int flag_semi = 0;
    int flag_newline = 0;
    while (token.type != TOKEN_IN && token.type != TOKEN_DO
           && token.type != TOKEN_EOF)
    { // on déroule jusqu'à tomber sur un mot clée. On passe donc tous les ";"
      // et "\n"
        if (token.type == TOKEN_SEMI_COLON)
        {
            if (flag_semi == 1 || flag_newline == 1)
            {
                *status = PARSER_UNEXPECTED_TOKEN;
                fprintf(stderr, "parse_for : too much \";\" inside the for\n");
                return;
            }
            flag_semi = 1;
        }
        if (token.type == TOKEN_EOL)
            flag_newline = 1;
        token = lexer_pop(lexer);
    }
<<<<<<< HEAD
}*/
/*
=======
}

>>>>>>> 6c52223d85c95d2a786a0a89c0d2d5b4fe6d64a8
static struct ast *parse_for(enum parser_status *status, struct lexer *lexer)
{
    struct token token = lexer_peek(lexer);
    if (token.type != TOKEN_FOR) // check si on a bien un token for ici
    {
        *status = PARSER_UNEXPECTED_TOKEN;
        fprintf(stderr,
                "parse_for : Expected token \"for\" but got something else\n");
        return NULL;
    }
    struct ast *result = ast_new(AST_FOR); // creation node
    token = lexer_pop(lexer); // get first word of the grammar
    if (token.type != TOKEN_WORDS)
    {
        *status = PARSER_UNEXPECTED_TOKEN;
        fprintf(stderr,
                "parse_for : Expected token \"word\" but got something else\n");
        return NULL;
    }
    result->value = token.value; // on garde sa value dans l'ast
    token = lexer_pop(lexer); // on dégage le précedent token word
    first_step_for(status, lexer); // on déroule jusqu'à tomber sur un mot clée.
                                   // On passe donc tous les ";" et "\n"
    if (*status == PARSER_OK && token.type == TOKEN_IN) // case with "in"
    {
        token = lexer_pop(lexer); // on dégage le in
<<<<<<< HEAD
        char *temp =
            result->value; // on fait une var temp qui stockera la valeur de
                           // tous les words qui vont s'enchainer
        while (token.type == TOKEN_WORDS)
        {
            temp = realloc(
                temp, sizeof(temp) + sizeof(token.value)); // on agrandit temp
            temp = strcat(
                temp, token.value); // on lui rajoute la value du token actuel
=======
        char *temp = result->value;
        // on fait une var temp qui stockera la valeur de
        // tous les words qui vont s'enchainer
        while (token.type == TOKEN_WORDS)
        {
            temp = realloc(temp, sizeof(temp) + sizeof(token.value));
            // on agrandit temp
            temp = strcat(temp, token.value);
            // on lui rajoute la value du token actuel
>>>>>>> 6c52223d85c95d2a786a0a89c0d2d5b4fe6d64a8
        }
        result->value = temp; // on met à jour result.value. En effet temp à
                              // surement changé due à realloc.
    }
<<<<<<< HEAD
    while (token.type != TOKEN_DO
           && token.type != TOKEN_EOF) // on skip tous les ";" et "\n"
        token = lexer_pop(lexer);
    if (*status == PARSER_OK && token.type == TOKEN_DO) // on arrive sur le DO
    {
        result->left = compound_list(
            status, lexer); // on met en fils gauche les instructions du for.
        if (lexer_peek(lexer).type
            != TOKEN_DONE) // on est censé avoir le mot clée done
=======
    while (token.type != TOKEN_DO && token.type != TOKEN_EOF)
        // on skip tous les ";" et "\n"
        token = lexer_pop(lexer);
    if (*status == PARSER_OK && token.type == TOKEN_DO) // on arrive sur le DO
    {
        result->left = compound_list(status, lexer);
        // on met en fils gauche les instructions du for.
        if (lexer_peek(lexer).type != TOKEN_DONE)
        // on est censé avoir le mot clée done
>>>>>>> 6c52223d85c95d2a786a0a89c0d2d5b4fe6d64a8
        {
            *status = PARSER_UNEXPECTED_TOKEN;
            fprintf(stderr, "parse_for : missing \"done\" inside for\n");
        }
        return result;
    }
    else // cas où on a pas de token "done".
    {
        *status = PARSER_UNEXPECTED_TOKEN;
        fprintf(stderr, "parse_for : missing \"do\" inside for\n");
        return result;
    }
<<<<<<< HEAD
}*/

/*
 *   shell_command =
 *      rule_if
 *   |  rule_while
 *   |  rule_until
 *   |  rule_for */

struct ast *shell_command(enum parser_status *status, struct lexer *lexer)
{
    switch (lexer_peek(lexer).type)
    {
    case TOKEN_IF:
        return rule_if(status, lexer);
    case TOKEN_WHILE:
    case TOKEN_UNTIL:
        return parse_while_until(status, lexer);
    /*case TOKEN_FOR:
        return parse_for(status, lexer);*/
    default:
        *status = PARSER_UNEXPECTED_TOKEN;
        fprintf(stderr, "shell_command : mauvais token\n");
        return NULL;
    }
}
