#include "parser.h"

static struct ast *choose_redir(struct token t) {
  // choisi la bonne redirection pour créer le noeud de l'ast
  switch (t.type) {
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
struct ast *redirection(enum parser_status *status, struct lexer *lexer) {
  struct token t = lexer_peek(lexer);
  int io = -1;                 // la valeur de io_type de l'ast en construction
                               // s'il y en a une, vaut IO_VOID par défaut
  if (t.use == TOKEN_IONUMBER) // partie de la grammaire : [IONUMBER]
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
