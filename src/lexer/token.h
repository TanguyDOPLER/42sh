#include <stdlib.h>

enum token_use
{
    TYPE_KEYWORD,
    TYPE_WORDS,
    TYPE_OPERATOR,
    TYPE_COMPOUND,
    TYPE_OTHER
};

enum token_type
{
    TOKEN_IF,
    TOKEN_THEN,
    TOKEN_ELIF,
    TOKEN_ELSE,
    TOKEN_FI,
    TOKEN_SEMI_COLON,
    TOKEN_EOL,
    TOKEN_EOF,
    TOKEN_S_QUOTE,
    TOKEN_WORDS,
    TOKEN_COMMENT
};

struct token
{
    enum token_use use;
    enum token_type type; // type of the token
    char *value; // if no value -> set to null
};
