#include "parser.h"

/**
 * element =
 *      WORD
 *    | redirection
 *    ;
 */
static struct ast *element(enum parser_status *status, struct lexer *lexer) {
  if (*status == PARSER_UNEXPECTED_TOKEN) // cas où le parse crash
    return NULL;
  struct token t = lexer_peek(lexer); // touche pas au peek ici connard

  // cas où le prochain token ne correspond à aucune grammaire
  if (t.type != TOKEN_WORDS && t.use != TOKEN_IONUMBER &&
      t.type != TOKEN_ASSIGNMENT_WORD &&
      !(t.type >= TOKEN_REDIR_INPUT && t.type <= TOKEN_REDIR_DOUBLE)) {
    fprintf(stderr, "element: Error token: expected WORD or IONUMBER, got %d\n",
            t.type);
    *status = PARSER_UNEXPECTED_TOKEN;
    return NULL;
  }

  if (t.use == TOKEN_IONUMBER ||
      (t.type >= TOKEN_REDIR_INPUT && t.type <= TOKEN_REDIR_DOUBLE)) {
    struct ast *elt = redirection(status, lexer);
    if (*status != PARSER_OK) // erreur au sein de la grammaire redirection
    {
      fprintf(stderr, "element : return of child (redirection) is wrong\n");
      ast_free(elt);
      return NULL;
    }
    return elt;
  }

  struct ast *elt = ast_new(AST_ARGUMENTS);
  if (t.use == TOKEN_D_QUOTE)
    elt->is_d_quote = 1;
  elt->value = t.value; // strdup(t.value);
                        // copie la valeur du token dans
  // celle du nouveau noeud (ast->value = t.value)
  lexer_pop(lexer);
  return elt;
}

/*
 * prefix=
 *      ASSIGNMENT_WORD
 *      | redirection
 * ;
 */
static struct ast *prefix(enum parser_status *status, struct lexer *lexer) {
  struct token t = lexer_peek(lexer);
  if (t.use == TOKEN_IONUMBER ||
      (t.type >= TOKEN_REDIR_INPUT && t.type <= TOKEN_REDIR_DOUBLE)) {
    return redirection(status, lexer);
  } else if (t.type == TOKEN_ASSIGNMENT_WORD) {
    struct ast *assignement = ast_new(AST_ASSIGNMENT_WORD);
    assignement->value = t.value;
    lexer_pop(lexer);
    return assignement;
  } else {
    fprintf(stderr,
            "prefix : Error token: expected ASSIGNEMENT_WORD or IONUMBER "
            "or REDIR, got %d\n",
            t.type);
    return NULL;
  }
}

// fonction utilisée dans simple_command
// effectue les opérations nécessaires pour un ou plusieurs préfixes
struct ast *handle_prefix(enum parser_status *status, struct lexer *lexer,
                          int *prefix_found) {
  struct ast *simple_com = NULL;
  struct token t = lexer_peek(lexer);

  while ((t.type >= TOKEN_REDIR_INPUT && t.type <= TOKEN_REDIR_DOUBLE) ||
         t.use == TOKEN_IONUMBER ||
         t.type == TOKEN_ASSIGNMENT_WORD) // tant qu'on a un prefix
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
    } else // cas où l'on croise le premier prefix
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
struct ast *handle_elements(enum parser_status *status, struct lexer *lexer,
                            struct ast *command) {
  struct ast *n_arg = command; // noeud actuel pour les arguments de la command
  struct ast *n_redir = command; // noeud actuelle pour les redirections
  struct token t = lexer_peek(lexer);

  while (t.type == TOKEN_WORDS ||
         (t.type >= TOKEN_REDIR_INPUT && t.type <= TOKEN_REDIR_DOUBLE) ||
         t.use == TOKEN_IONUMBER ||
         t.type == TOKEN_ASSIGNMENT_WORD) // condition pour avoir un élément
  {
    struct ast *child = element(status, lexer);
    if (*status == PARSER_UNEXPECTED_TOKEN) // erreur
    {
      fprintf(stderr, "Error in handle_elements: invalid element.\n");
      ast_free(command);
      return NULL;
    }
    if ((t.type >= TOKEN_REDIR_INPUT && t.type <= TOKEN_REDIR_DOUBLE) ||
        t.use == TOKEN_IONUMBER) // redir ou io_number
    {
      if (n_redir->type ==
          AST_SIMPLE_COMMAND) // cas où on croise une redirection
                              // pour la première fois
      {
        child->left = n_redir;
        n_redir = child;
        command = n_redir;
      } else {
        child->left = n_redir->left;
        n_redir->left = child;
        n_redir = n_redir->left;
      }
    } else // si c'est un word, on utilise le noeud
           // n_arg pour placer l'argument
    {
      n_arg->left = child;
      n_arg = n_arg->left;
    }
    t = lexer_peek(lexer);
  }
  return command;
}
