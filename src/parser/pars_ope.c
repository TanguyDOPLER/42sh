#include "parser.h"

/**
 *  pipeline = ['!'] command { '|' {'\n'} command } ;
 */
static struct ast *pipeline(enum parser_status *status, struct lexer *lexer)
{
    if (*status == PARSER_UNEXPECTED_TOKEN) // cas où le parse crash
        return NULL;
    struct token token = lexer_peek(lexer);
    int flag = 0;
    if (token.type == TOKEN_NEGATION)
    {
        lexer_pop(lexer); // on dégage le '!'
        flag = 1;
    }
    struct ast *ast = ast_new(AST_PIPE); // initialisation struct ast
    ast->left =
        command(status, lexer); // on aura forcement un "command"
                                // au début. Donc
                                // c'est le fils donc on le met à gauche.
    if (ast->left == NULL || *status == PARSER_UNEXPECTED_TOKEN)
    // Si on a pas de command c'est que le parser a
    { // planté. Il faudra ptet mettre à jour status ici
        *status = PARSER_UNEXPECTED_TOKEN;
        fprintf(stderr, "pipeline : ast.left (command) gone wrong\n");
        return ast;
    }
    token = lexer_peek(lexer); // variable contenant current token
    struct ast *temp;
    while (token.type == TOKEN_PIPE)
    // boucle -> check grammar : { '|' {'\n'} command } ;

    {
        while ((token = lexer_pop(lexer)).type == TOKEN_EOL)
        // on saute tous les retour à la ligne
        {}
        if (ast->right == NULL)
            // première iteration. Le bout de l'ast a deux commandes

            ast->right = command(status, lexer);
        else
        {
            temp = ast; // save old ast
            ast = ast_new(AST_PIPE); // new one
            ast->left = temp; // we put the new one above the oldest one
            ast->right = command(status, lexer);
        }
        if (*status == PARSER_UNEXPECTED_TOKEN) // le parser a planté. Il faudra
                                                // ptet mettre à jour status ici
        {
            fprintf(
                stderr,
                "pipeline : actual.left(command) in loop while gone wrong\n");
            return ast;
        }
        token = lexer_peek(lexer);
    }
    if (flag == 1)
    {
        temp = ast_new(AST_NEGATION); // on rajoute un noeud negation au top de
                                      // l'ast actuel
        temp->left = ast;
        ast = temp;
    }
    return ast; // retour de l'ast complet
}

/**
 * and_or = pipeline { ( '&&' | '||' ) {'\n'} pipeline } ;
 */
struct ast *and_or(enum parser_status *status, struct lexer *lexer)
{ // on créé de base un AST_AND. Si c'est un OR on change son type. Sinon on
  // touche pas
    if (*status == PARSER_UNEXPECTED_TOKEN) // cas où le parse crash
        return NULL;
    struct ast *ast = ast_new(AST_AND); // initialisation struct ast
    ast->left = pipeline(status, lexer);
    // on aura forcement un "pipeline" au début.
    // Donc c'est le fils donc on le met à gauche.
    if (ast->left == NULL || *status == PARSER_UNEXPECTED_TOKEN)
    // Si on a pas de pipeline c'est
    // que le parser a
    { // planté. Il faudra ptet mettre à jour status ici
        *status = PARSER_UNEXPECTED_TOKEN;
        fprintf(stderr, "and_or : ast.left (pipeline) gone wrong\n");
        return ast;
    }
    struct token token = lexer_peek(lexer);
    // variable contenant le token qu'on étudie actuellement
    struct ast *temp;
    if (token.type == TOKEN_OR)
        ast->type = AST_OR;
    int flag_or = 0; // flag to remermber that we are on a "OR" and not a "AND"
    while (token.type == TOKEN_AND || token.type == TOKEN_OR)
    // boucle -> check grammar : { ( '&&' |
    // '||' ) {'\n'} pipeline } ;
    {
        if (token.type == TOKEN_OR)
            flag_or = 1;
        else
            flag_or = 0;
        while ((token = lexer_pop(lexer)).type == TOKEN_EOL)
        // on saute tous les retour à la ligne
        {}
        if (ast->right == NULL)
            // première iteration. Le bout de l'ast a deux commandes
            ast->right = pipeline(status, lexer);
        else
        {
            temp = ast; // save old ast
            ast = ast_new(AST_AND); // new one
            if (flag_or == 1)
                ast->type = AST_OR;
            ast->left = temp; // we put the new one above the oldest one
            ast->right = pipeline(status, lexer);
        }
        if (*status == PARSER_UNEXPECTED_TOKEN) // le parser a planté. Il faudra
                                                // ptet mettre à jour status ici
        {
            fprintf(stderr, "and_or:actual.left(pipeline) in lop while err\n");
            return ast;
        }
        token = lexer_peek(lexer);
    }
    if (token.type == TOKEN_OR)
        ast->type = AST_OR;
    return ast;
}
