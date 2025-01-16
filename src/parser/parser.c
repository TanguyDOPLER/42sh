#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct ast *redirection(enum parser_status *status, struct lexer *lexer);
static struct ast *prefix(enum parser_status *status, struct lexer *lexer);

/**
 * element =
 *      WORD
 *    | redirection
 *    ;
 */
static struct ast *element(enum parser_status *status, struct lexer *lexer)
{
    if (*status == PARSER_UNEXPECTED_TOKEN) // cas où le parse crash
        return NULL;
    struct token t = lexer_peek(lexer); // touche pas au peek ici connard

    // cas où le prochain token ne correspond à aucune grammaire
    if (t.type != TOKEN_WORDS && t.type != TOKEN_IONUMBER
        && !(t.type >= TOKEN_REDIR_INPUT && t.type <= TOKEN_REDIR_DOUBLE))
    {
        fprintf(stderr,
                "element: Error token: expected WORD or IONUMBER, got %d\n",
                t.type);
        *status = PARSER_UNEXPECTED_TOKEN;
        return NULL;
    }
    if (t.type == TOKEN_WORDS) // cas d'un WORD
    {
        struct ast *elt = ast_new(AST_ARGUMENTS);
        elt->value =
            t.value; // strdup(t.value); // copie la valeur du token dans
                     // celle du nouveau noeud (ast->value = t.value)
        lexer_pop(lexer);
        return elt;
    }
    // cas d'une redirection
    struct ast *elt = redirection(status, lexer);
    if (*status != PARSER_OK) // erreur au sein de la grammaire redirection
    {
        fprintf(stderr, "element : return of child (redirection) is wrong\n");
        ast_free(elt);
        return NULL;
    }
    return elt;
}

static struct ast *choose_redir(struct token t)
{
    // choisi la bonne redirection pour créer le noeud de l'ast
    switch (t.type)
    {
    case TOKEN_REDIR_INPUT: // <
        return ast_new(AST_REDIR_INPUT);
    case TOKEN_REDIR_OUTPUT: // > ou >|
        return ast_new(AST_REDIR_OUTPUT);
    case TOKEN_REDIR_AP_OUTPUT: // >>
        return ast_new(AST_REDIR_AP_OUTPUT);
    case TOKEN_REDIR_DUP_INPUT: // >&
        return ast_new(AST_REDIR_DUP_INPUT);
    case TOKEN_REDIR_DUP_OUTPUT: // <&
        return ast_new(AST_REDIR_DUP_OUTPUT);
    default: // <>
        return ast_new(AST_REDIR_DOUBLE);
    }
    return ast_new(AST_REDIR_DOUBLE);
}

/**
 * redirection = [IONUMBER] ( '>' | '<' | '>>' | '>&' | '<&' | '>|' | '<>' )
 * WORD ;
 */
static struct ast *redirection(enum parser_status *status, struct lexer *lexer)
{
    struct token t = lexer_peek(lexer);
    int io = -1; // la valeur de io_type de l'ast en construction
                 // s'il y en a une, vaut IO_VOID par défaut
    if (t.type == TOKEN_IONUMBER) // partie de la grammaire : [IONUMBER]
    {
        int tmp_io = atoi(t.value); // récupère la valeur d'IONUMBER en int
        free(t.value);
        if (tmp_io < 0) // si mauvais IONUMBER
        {
            fprintf(stderr, "redirection : wrong IONUMBER\n");
            *status = PARSER_UNEXPECTED_TOKEN;
            return NULL;
        }
        io = tmp_io;
        t = lexer_pop(lexer); // on passe à la redirection
    }

    // partie de la grammaire : ( '>' | '<' | '>>' | '>&' | '<&' | '>|' | '<>' )
    if (!(t.type >= TOKEN_REDIR_INPUT && t.type <= TOKEN_REDIR_DOUBLE))
    // erreur si le type n'est pas TYPE_REDIR
    {
        fprintf(stderr, "redir : Error token: expected TYPE_REDIR, got %d\n",
                t.use);
        *status = PARSER_UNEXPECTED_TOKEN;
        return NULL;
    }
    struct ast *redir = choose_redir(t); // le noeud à retourner à la fin
    redir->io_type = io;
    // partie de la grammaire : WORD
    t = lexer_pop(lexer);
    if (t.type != TOKEN_WORDS) // si ce n'est pas un WORD, erreur
    {
        ast_free(redir);
        fprintf(stderr, "redir : Error token: expected WORD, got %d\n", t.type);
        *status = PARSER_UNEXPECTED_TOKEN;
        return NULL;
    }
    lexer_pop(lexer);
    redir->value =
        t.value; // la value du noeud correspond à la valeur du token WORD
    return redir;
}

static struct ast *prefix(enum parser_status *status, struct lexer *lexer)
{
    return redirection(status, lexer);
}

/**
 * compound_list = {'\n'} and_or { ( ';' | '\n' ) {'\n'} and_or } [';'] {'\n'} ;
 */
static struct ast *compound_list(enum parser_status *status,
                                 struct lexer *lexer)
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
        fprintf(stderr, "compound_list : return of child (and_or) is wrong\n");
        ast_free(compound);
        ast_free(child);
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
                fprintf(stderr,
                        "compound_list : return of child (and_or) is wrong\n");
                ast_free(compound);
                ast_free(child);
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

// fonction utilisée dans simple_command
// effectue les opérations nécessaires pour un ou plusieurs préfixes
static struct ast *handle_prefix(enum parser_status *status,
                                 struct lexer *lexer, int *prefix_found)
{
    struct ast *simple_com = NULL;
    struct token t = lexer_peek(lexer);

    while ((t.type >= TOKEN_REDIR_INPUT && t.type <= TOKEN_REDIR_DOUBLE)
           || t.type == TOKEN_IONUMBER) // tant qu'on a un prefix
    {
        struct ast *n = prefix(status, lexer);
        if (*status != PARSER_OK) // erreur
        {
            ast_free(simple_com);
            ast_free(n);
            fprintf(stderr, "Error in handle_prefix: invalid prefix.\n");
            return NULL;
        }
        if (*prefix_found) // cas où l'on a déjà au moins un prefix
        {
            n->left = simple_com;
            simple_com = n;
        }
        else // cas où l'on croise le premier préfix
        {
            *prefix_found = 1;
            simple_com = n;
        }
        t = lexer_peek(lexer);
    }
    return simple_com;
}

// fonction utilisée dans simple_command
// effectue les opérations nécessaires pour un ou plusieurs éléments
static struct ast *handle_elements(enum parser_status *status,
                                   struct lexer *lexer, struct ast *command)
{
    struct ast *n_arg =
        command; // noeud actuel pour les arguments de la command
    struct ast *n_redir = command; // noeud actuelle pour les redirections
    struct token t = lexer_peek(lexer);

    while (t.type == TOKEN_WORDS
           || (t.type >= TOKEN_REDIR_INPUT && t.type <= TOKEN_REDIR_DOUBLE)
           || t.type == TOKEN_IONUMBER) // condition pour avoir un élément
    {
        struct ast *child = element(status, lexer);
        if (*status == PARSER_UNEXPECTED_TOKEN) // erreur
        {
            fprintf(stderr, "Error in handle_elements: invalid element.\n");
            ast_free(command);
            return NULL;
        }

        if (t.type == TOKEN_WORDS) // si c'est un word, on utilise le noeud
                                   // n_arg pour placer l'argument
        {
            n_arg->left = child;
            n_arg = n_arg->left;
        }
        else // sinon, on utilise le noeud n_redir pour placer l'argument
        {
            if (n_redir->type
                == AST_SIMPLE_COMMAND) // cas où on croise une redirection
                                       // pour la première fois
            {
                child->left = n_redir;
                n_redir = child;
                command = n_redir;
            }
            else
            {
                child->left = n_redir->left;
                n_redir->left = child;
                n_redir = n_redir->left;
            }
        }
        t = lexer_peek(lexer);
    }
    return command;
}

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
    struct ast *n = simple_com;
    while (n->left != NULL)
    {
        n = n->left;
    }
    n->left = ast_new(AST_SIMPLE_COMMAND);
    n->left->value = t.value;
    lexer_pop(lexer);
    n->left = handle_elements(status, lexer, n->left);
    return simple_com;
}

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
        lexer_pop(lexer); // on fait sauter le token "then" car maintenant on
                          // s'en fou + check
        if (token.type != TOKEN_THEN || *status == PARSER_UNEXPECTED_TOKEN)
        {
            *status = PARSER_UNEXPECTED_TOKEN;
            fprintf(stderr,
                    "rule_if : Epexted token \"then\" but got something else "
                    "or ast.condition (compound_list) gone wrong\n");
            return ast;
        }
        ast->left =
            compound_list(status, lexer); // fils gauche = condition vérifié
        token = lexer_peek(
            lexer); // check next token pour savoir si on a un else ou pas
        if (*status == PARSER_OK
            && (token.type == TOKEN_ELSE || token.type == TOKEN_ELIF))
        {
            ast->right = else_clause(
                status,
                lexer); // on call la fonction qui s'occupe du else et elif
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

/*
 * (* for the time being, it is limited to a single rule_if *)
 *   shell_command = rule_if ;
 */

static struct ast *shell_command(enum parser_status *status,
                                 struct lexer *lexer)
{
    return rule_if(status, lexer);
}

/**
 command =
    simple_command
    shell_command { redirection };
 */
static struct ast *command(enum parser_status *status, struct lexer *lexer)
{
    if (*status == PARSER_UNEXPECTED_TOKEN) // cas où le parse crash
        return NULL;
    struct token token = lexer_peek(lexer); // on test si on est sur une simple
                                            // commande ou une shell commande
    if (token.type == TOKEN_IF) // cas shell command
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
        command(status, lexer); // on aura forcement un "command" au début. Donc
                                // c'est le fils donc on le met à gauche.
    if (ast->left == NULL
        || *status == PARSER_UNEXPECTED_TOKEN) // Si on a pas de command c'est
                                               // que le parser a
    { // planté. Il faudra ptet mettre à jour status ici
        *status = PARSER_UNEXPECTED_TOKEN;
        fprintf(stderr, "pipeline : ast.left (command) gone wrong\n");
        return ast;
    }
    token = lexer_peek(
        lexer); // variable contenant le token qu'on étudie actuellement
    struct ast *temp;
    while (token.type
           == TOKEN_PIPE) // boucle -> check grammar : { '|' {'\n'} command } ;

    {
        while ((token = lexer_pop(lexer)).type
               == TOKEN_EOL) // on saute tous les retour à la ligne
        {
        }
        if (ast->right
            == NULL) // première iteration. Le bout de l'ast a deux commandes
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
    ast->left =
        pipeline(status, lexer); // on aura forcement un "pipeline" au début.
                                 // Donc c'est le fils donc on le met à gauche.
    if (ast->left == NULL
        || *status == PARSER_UNEXPECTED_TOKEN) // Si on a pas de pipeline c'est
                                               // que le parser a
    { // planté. Il faudra ptet mettre à jour status ici
        *status = PARSER_UNEXPECTED_TOKEN;
        fprintf(stderr, "and_or : ast.left (pipeline) gone wrong\n");
        return ast;
    }
    struct token token = lexer_peek(
        lexer); // variable contenant le token qu'on étudie actuellement
    struct ast *temp;
    if (token.type == TOKEN_OR)
        ast->type = AST_OR;
    int flag_or = 0; // flag to remermber that we are on a "OR" and not a "AND"
    while (token.type == TOKEN_AND
           || token.type == TOKEN_OR) // boucle -> check grammar : { ( '&&' |
                                      // '||' ) {'\n'} pipeline } ;

    {
        if (token.type == TOKEN_OR)
            flag_or = 1;
        else
            flag_or = 0;
        while ((token = lexer_pop(lexer)).type
               == TOKEN_EOL) // on saute tous les retour à la ligne
        {
        }
        if (ast->right
            == NULL) // première iteration. Le bout de l'ast a deux commandes
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
            fprintf(
                stderr,
                "and_or : actual.left(pipeline) in loop while gone wrong\n");
            return ast;
        }
        token = lexer_peek(lexer);
    }
    if (token.type == TOKEN_OR)
        ast->type = AST_OR;
    return ast;
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
    if (t.type == TOKEN_EOL || t.type == TOKEN_EOF)
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
