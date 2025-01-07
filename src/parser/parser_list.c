#include <cstddef>
#include "ast.h"
#include "parser.h"

struct ast *parser_list(enum parser_status *status, struct lexer *lexer)
{ //fonction qui parse une liste. fonctionnement AST : fils gauche frère droit
    if (*status == PARSER_UNEXPECTED_TOKEN) //cas où le parse crash
        return NULL;
    struct ast *ast = ast_new(AST_LIST); //initialisation struct ast
    ast->left = and_or(status, lexer); //on aura forcement un "and_or" au début. Donc c'est le fils donc on le met à gauche.
    if (ast->left == NULL)// Si on a pas de and_or c'est que le parser a planté. Il faudra ptet mettre à jour status ici
        return NULL;
    struct ast *actual = ast; //variable temporaire pour parcourir les fils gauche, soit les frère
    struct token *token = lexer_next_token(lexer); //variable contenant le token qu'on étudie actuellement
    while(1/*token == TOKEN_SEMI_COLON*/)//boucle -> check grammar : { ';' and_or } [ ';' ]
    {
        if (peek_token(lexer)->type == TOKEN_WORDS) //cas où on a un and_or et non juste un point virgule
        {
            actual->right = ast_new(AST_AND_OR); //on rajoute un frère
            actual = actual->right; //on actualise la variable actual
            actual->left = and_or(status, lexer); //on met la valeur du and_or dans le fils gauche
        }
        token = lexer_next_token(lexer); //on prend le token suivant
    }
    return ast; //retour de l'ast complet
}
