#ifndef TOKEN_H
#define TOKEN_H

#include <stdlib.h>

enum token_use
{
    TYPE_DOUBLE_QUOTES,
    TYPE_REDIR,
    TYPE_OTHER
};

enum token_type
{
    TOKEN_IF, // GOOD
    TOKEN_THEN, // GOOD
    TOKEN_ELIF, // GOOD
    TOKEN_ELSE, // GOOD
    TOKEN_FI, // GOOD
    TOKEN_SEMI_COLON,
    TOKEN_EOL, // GOOD
    TOKEN_EOF, // GOOD
    TOKEN_S_QUOTE, // GOOD
    TOKEN_D_QUOTE, // GOOD
    TOKEN_VAR,
    TOKEN_WORDS, // GOOD
    TOKEN_COMMENT, // GOOD
    TOKEN_REDIR_INPUT, //<
    TOKEN_REDIR_OUTPUT, //> et >|
    TOKEN_REDIR_AP_OUTPUT, //>>
    TOKEN_REDIR_DUP_OUTPUT, //>&
    TOKEN_REDIR_DUP_INPUT, //<&
    TOKEN_REDIR_DOUBLE, //<>
    TOKEN_PIPE, // |
    TOKEN_OR, // GOOD
    TOKEN_AND, // GOOD
    TOKEN_WHILE, // GOOD
    TOKEN_UNTIL, // GOOD
    TOKEN_DO, // GOOD
    TOKEN_DONE, // GOOD
    TOKEN_FOR, // GOOD
    TOKEN_IN, // GOOD
    TOKEN_NEGATION, // GOOD
    TOKEN_IONUMBER,
    TOKEN_INVALID,
    TOKEN_ASSIGNMENT_WORD
};

struct token
{
    enum token_use use;
    enum token_type type; // type of the token
    char *value; // if no value -> set to null
};
#endif /* ! TOKEN_H */
