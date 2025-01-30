#include <stdio.h>
#include <string.h>

#include "parser.h"

/*
 * else_clause =
 *    'else' compound_list
 *  | 'elif' compound_list 'then' compound_list [else_clause];
 */

static struct ast *else_clause(enum parser_status *status,
                               struct lexer *lexer) {
  if (*status == PARSER_UNEXPECTED_TOKEN) // cas où le parse crash
    return NULL;
  struct token token = lexer_peek(lexer); // on test si on est sur une simple
                                          // commande ou une shell commande
  free(token.value);
  lexer_pop(lexer);            // on dégage le else ou elif
  if (token.use == TOKEN_ELSE) // cas simple else
  {
    return compound_list(status, lexer);
  } else if (token.use == TOKEN_ELIF) // cas else + if = elif
  {
    struct ast *ast =
        ast_new(AST_SHELL_COMMAND); // faire un ast shell commande ici et
                                    // faire un if sans verif des mots clef
    ast->condition = compound_list(status, lexer); // set up la condition
    token = lexer_peek(lexer);
    if (token.value)
      free(token.value);
    if (token.use != TOKEN_THEN) // check token then
    {
      *status = PARSER_UNEXPECTED_TOKEN;
      fprintf(stderr, "else_clause : Epexted token \"then\" but got something "
                      "else\n");
    }
    lexer_pop(lexer); // on dégage le then
    ast->left = compound_list(
        status,
        lexer); // on ressort direct ici si status = PARSER_UNEXPECTED_TOKEN
    if (lexer_peek(lexer).use != TOKEN_FI)
      ast->right = else_clause(status, lexer);
    return ast;
  }
  *status = PARSER_UNEXPECTED_TOKEN; // cas où token != elif et else
  return NULL;
}

/*
 * rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi' ;
 */
void token_issue(struct token token, enum parser_status *status) {
  if (token.value)
    free(token.value);
  *status = PARSER_UNEXPECTED_TOKEN;
  fprintf(stderr, "rule_if:Expected token IF but got something else\n");
}

static void error_rule_if(struct token token, enum parser_status *status) {
  if (token.value)
    free(token.value);
  *status = PARSER_UNEXPECTED_TOKEN;
  fprintf(stderr, "rule_if:Expected THEN,got %d or compound_list issue\n",
          token.use);
}

static struct ast *rule_if(enum parser_status *status, struct lexer *lexer) {
  if (*status == PARSER_UNEXPECTED_TOKEN) // cas où le parse crash
    return NULL;
  struct ast *ast = NULL;
  struct token token = lexer_peek(lexer);
  if (token.use == TOKEN_IF) // cas shell command
  {
    free(token.value);
    lexer_pop(lexer); // on fait sauter le token "if " car maintenant on
                      // s'en fou + free buffer
    ast = ast_new(AST_SHELL_COMMAND);              // on crée l'ast final
    ast->condition = compound_list(status, lexer); // on setup la condition
    token = lexer_peek(lexer); // on check si on a bien un "then" à la suite
    if (token.use != TOKEN_THEN || *status == PARSER_UNEXPECTED_TOKEN) {
      error_rule_if(token, status);
      return ast;
    }
    free(token.value);
    lexer_pop(lexer); // on fait sauter le token "then" car maintenant on
                      // s'en fou
    ast->left = compound_list(status, lexer);
    if (*status != PARSER_OK) {
      ast_free(ast);
      fprintf(stderr, "rule_if: error in compound_list\n");
      return NULL;
    }
    // fils gauche = condition vérifié
    token = lexer_peek(lexer);
    // check next token pour savoir si on a un else ou pas
    if (*status == PARSER_OK &&
        (token.use == TOKEN_ELSE || token.use == TOKEN_ELIF)) {
      ast->right = else_clause(status, lexer);
      // on call la fonction qui s'occupe du else et elif
      token = lexer_peek(lexer); // on récupère ici le "fi" normalement
    }
    if (*status == PARSER_OK && (token.use == TOKEN_FI)) {
      free(token.value);
      lexer_pop(lexer); // on dégage le token fi
    } else {            // ici gestion erreur
      if (token.value)
        free(token.value);
      fprintf(stderr,
              "rule_if:Expected token FI but got other token or "
              "ast.left/right (compound_list/else_clause) gone wrong\n");
      *status = PARSER_UNEXPECTED_TOKEN;
    }
  } else {
    token_issue(token, status);
  }
  return ast; // on retourne l'ast global.
} // 38

static struct ast *parse_while_until(enum parser_status *status,
                                     struct lexer *lexer) {
  struct token token = lexer_peek(lexer);
  if (token.value)
    free(token.value);
  if (token.use != TOKEN_WHILE &&
      token.use != TOKEN_UNTIL) // check si on a bien le word "while"
  {
    *status = PARSER_UNEXPECTED_TOKEN;
    return NULL;
  }
  lexer_pop(lexer);
  struct ast *result = ast_new(AST_WHILE);
  if (token.use == TOKEN_UNTIL)
    result->type = AST_UNTIL;
  token = lexer_peek(lexer);
  if (token.type == TOKEN_SEMI_COLON) {
    ast_free(result);
    *status = PARSER_UNEXPECTED_TOKEN;
    return NULL;
  }
  result->condition =
      compound_list(status, lexer); // on met à jour la condition du while
  token = lexer_peek(lexer);
  if (token.value)
    free(token.value);
  if (*status == PARSER_UNEXPECTED_TOKEN ||
      token.use != TOKEN_DO) // check si on a bien le word "do"
  {
    ast_free(result);
    *status = PARSER_UNEXPECTED_TOKEN;
    return NULL;
  }
  lexer_pop(lexer); // on dégage le mot "do"
  result->left = compound_list(
      status, lexer); // le contenu du while est stocké dans le frr gauche
  token = lexer_peek(lexer);
  if (token.value)
    free(token.value);
  if (*status == PARSER_UNEXPECTED_TOKEN ||
      token.use != TOKEN_DONE) // check si on a bien le word "done"
  {
    *status = PARSER_UNEXPECTED_TOKEN;
    ast_free(result);
    return NULL;
  }
  lexer_pop(lexer); // on dégage le mot "done"
  return result;
} // 39

// [ {'\n'} 'in' { WORD } ( ';' | '\n' ) ]
static char **for_with_in(enum parser_status *status, struct lexer *lexer,
                          int *values_count) {
  struct token token = lexer_peek(lexer);
  if (token.value)
    free(token.value);
  if (token.use != TOKEN_IN) {
    *status = PARSER_UNEXPECTED_TOKEN;
    fprintf(stderr, "parse_for : expected IN but got %d\n", token.type);
    return NULL;
  }
  token = lexer_pop(lexer);
  char **res = NULL;
  while (token.type == TOKEN_WORDS) {
    if (res == NULL)
      res = malloc(sizeof(char *));
    else
      res = realloc(res, sizeof(char *) * (*values_count + 1));
    if (!res) {
      *status = PARSER_UNEXPECTED_TOKEN;
      return NULL;
    }
    res[*values_count] = token.value;
    *values_count = *values_count + 1;
    token = lexer_pop(lexer);
  }
  if (token.type != TOKEN_SEMI_COLON && token.type != TOKEN_EOL) {
    *status = PARSER_UNEXPECTED_TOKEN;
    if (res != NULL) {
      for (int i = 0; i < *values_count; i++) {
        free(res[i]);
      }
      free(res);
    }
    return NULL;
  }
  lexer_pop(lexer);
  return res;
}

static struct ast *end_of_for(enum parser_status *status, struct lexer *lexer) {
  struct token token = lexer_peek(lexer);
  while (token.type == TOKEN_EOL)
    token = lexer_pop(lexer);
  if (token.value)
    free(token.value);
  if (token.use != TOKEN_DO) {
    fprintf(stderr, "parse_for : Expected token DO but got %d\n", token.type);
    *status = PARSER_UNEXPECTED_TOKEN;
    return NULL;
  }
  token = lexer_pop(lexer);
  struct ast *res = compound_list(status, lexer);
  if (*status != PARSER_OK) {
    ast_free(res);
    fprintf(stderr, "parse_for (in compound list)\n");
    *status = PARSER_UNEXPECTED_TOKEN;
    return NULL;
  }
  token = lexer_peek(lexer);
  if (token.value)
    free(token.value);
  if (token.use != TOKEN_DONE) {
    ast_free(res);
    fprintf(stderr, "parse_for : Expected token DONE but got %d\n", token.type);
    *status = PARSER_UNEXPECTED_TOKEN;
    return NULL;
  }
  token = lexer_pop(lexer);
  return res;
}

/*
 * rule_for=
 *    'for' WORD ( [';'] | [ {'\n'} 'in' { WORD } ( ';' | '\n' ) ] ) {'\n'} 'do'
 * compound_list 'done' ;
 */

static void error_for(struct token token, enum parser_status *status,
                      struct ast *ast_for) {
  *status = PARSER_UNEXPECTED_TOKEN;
  fprintf(stderr, "parse_for : Expected token WORD but got %d\n", token.type);
  free(ast_for);
  if (token.value)
    free(token.value);
}
static struct ast *parse_for(enum parser_status *status, struct lexer *lexer) {
  struct token token = lexer_peek(lexer); // 'for'
  if (token.value)
    free(token.value);
  if (token.use != TOKEN_FOR) // check si on a bien un token for ici
  {
    *status = PARSER_UNEXPECTED_TOKEN;
    fprintf(stderr, "parse_for : Expected token FOR but got %d\n", token.type);
    return NULL;
  }
  struct ast *ast_for = ast_new(AST_FOR); // création du noeud pour for
  token = lexer_pop(lexer); // premier WORD après FOR : nom de variable
  if (token.type != TOKEN_WORDS) {
    error_for(token, status, ast_for);
    return NULL;
  }
  ast_for->value = token.value;
  ast_for->len_values = 0;
  token = lexer_pop(lexer);
  if (token.type == TOKEN_SEMI_COLON) // [';']
  {
    token = lexer_pop(lexer);
    ast_for->left = end_of_for(status, lexer);
    if (*status != PARSER_OK) {
      ast_free(ast_for);
      fprintf(stderr, "error : parse_for (end_of_for)\n");
      return NULL;
    }
    return ast_for;
  }
  while (token.type == TOKEN_EOL)
    token = lexer_pop(lexer);
  if (token.use == TOKEN_IN) // 'in'
  {
    ast_for->for_values = for_with_in(status, lexer, &ast_for->len_values);
    if (*status != PARSER_OK) {
      ast_free(ast_for);
      return NULL;
    }
  }
  token = lexer_peek(lexer);
  ast_for->left = end_of_for(status, lexer);
  if (*status != PARSER_OK) {
    ast_free(ast_for);
    fprintf(stderr, "error : parse_for (end_of_for)\n");
    return NULL;
  }
  return ast_for;
} // 38

/*
 *   shell_command =
 *      ‘{’ compound_list ‘}’
 *   |  rule_if
 *   |  rule_while
 *   |  rule_until
 *   |  rule_for */

struct ast *shell_command(enum parser_status *status, struct lexer *lexer) {
  struct token token = lexer_peek(lexer);
  switch (token.use) {
  case TOKEN_LEFT_ACC:
    free(token.value);
    token = lexer_pop(lexer);
    struct ast *ast = compound_list(status, lexer);
    token = lexer_peek(lexer);
    if (token.value)
      free(token.value);
    if (token.use != TOKEN_RIGHT_ACC) {
      ast_free(ast);
      *status = PARSER_UNEXPECTED_TOKEN;
      fprintf(stderr, "shell_command : mauvais token : %d\n", token.use);
      return NULL;
    }
    lexer_pop(lexer);
    return ast;
  case TOKEN_IF:
    return rule_if(status, lexer);
  case TOKEN_WHILE:
  case TOKEN_UNTIL:
    return parse_while_until(status, lexer);
  case TOKEN_FOR:
    return parse_for(status, lexer);
  default:
    if (token.value)
      free(token.value);
    *status = PARSER_UNEXPECTED_TOKEN;
    fprintf(stderr, "shell_command : mauvais token : %d\n", token.use);
    return NULL;
  }
}
