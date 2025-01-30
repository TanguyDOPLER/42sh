#define _POSIX_C_SOURCE 200809L

#include "ast.h"

#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct ast *ast_new(enum ast_type type) {
  struct ast *new = calloc(1, sizeof(struct ast));
  if (!new)
    return NULL;
  new->type = type;
  new->is_d_quote = 0;
  return new;
}

void ast_free(struct ast *ast) {
  if (ast == NULL)
    return;

  ast_free(ast->left);
  ast->left = NULL;

  ast_free(ast->right);
  ast->right = NULL;

  ast_free(ast->condition);
  ast->condition = NULL;

  if (ast->value)
    free(ast->value); // free les fameux memory leaks de maxime et son
                      // buffer :)

  for (int i = 0; i < ast->len_values; i++)
    free(ast->for_values[i]);
  if (ast->len_values > 0)
    free(ast->for_values);

  free(ast);
}

struct ast *ast_copy(struct ast *original) {
  if (original == NULL)
    return NULL;

  struct ast *new_ast = malloc(sizeof(struct ast));
  if (new_ast == NULL)
    return NULL;

  new_ast->type = original->type;             // type
  new_ast->io_type = original->io_type;       // io_type
  new_ast->len_values = original->len_values; // len_values

  if (original->value) // value
  {
    new_ast->value = strdup(original->value);
    if (!new_ast->value) {
      free(new_ast);
      return NULL;
    }
  } else {
    new_ast->value = NULL;
  }

  if (original->for_values) // for_values
  {
    new_ast->for_values = malloc(sizeof(char *) * original->len_values);
    if (!new_ast->for_values) {
      free(new_ast->value);
      free(new_ast);
      return NULL;
    }
    for (int i = 0; i < original->len_values; i++) {
      if (original->for_values[i]) {
        new_ast->for_values[i] = strdup(original->for_values[i]);
        if (!new_ast->for_values[i]) {
          for (int j = 0; j < i; ++j) {
            free(new_ast->for_values[j]);
          }
          free(new_ast->for_values);
          free(new_ast->value);
          free(new_ast);
          return NULL;
        }
      } else {
        new_ast->for_values[i] = NULL;
      }
    }
  } else {
    new_ast->for_values = NULL;
  }

  new_ast->condition = ast_copy(original->condition);
  new_ast->left = ast_copy(original->left);
  new_ast->right = ast_copy(original->right);

  return new_ast;
}
