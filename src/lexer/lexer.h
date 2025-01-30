#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>

#include "token.h"

struct lexer {
  const char *input; // L'input de 'exe'
  size_t pos;        // positio=n dans la string input
  struct token curr_token;
  size_t prev_pos;
};

struct lexer *lexer_init(const char *input);
// init a new lexer

void lexer_free(struct lexer *lexer);
// free a lexer

struct token lexer_next_token(struct lexer *lexer);

/**
 * \brief Returns the next token, but doesn't move forward: calling lexer_peek
 * multiple times in a row always returns the same result.
 * This function is meant to help the parser check if the next token matches
 * some rule.
 */
struct token lexer_peek(struct lexer *lexer);

/**
 * \brief Returns the next token, and removes it from the stream:
 *   calling lexer_pop in a loop will iterate over all tokens until EOF.
 */
struct token lexer_pop(struct lexer *lexer);

enum token_type lexer_time_machine(struct lexer *lexer);

#endif /* !LEXER_H */
