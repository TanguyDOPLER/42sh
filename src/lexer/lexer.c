#include "lexer.h"

#include <stddef.h>

struct lexer* lexer_new(const char* input)
{
    struct lexer *lexer = malloc(sizeof(struct lexer));
    lexer->input = input;
    lexer->pos = 0;
    lexer->token_list = malloc(sizeof(struct token));
    return lexer;
}

void lexer_free(struct lexer* lexer)
{
    free(lexer->token_list);
    lexer->pos = 0;
    lexer->input = NULL;
    free(lexer);
}


