#ifndef TOKEN_H
#define TOKEN_H

#include <stdlib.h>

enum token_type
{
    TOKEN_INVALID,
    TOKEN_WORDS,
    TOKEN_OPERATOR,
    // THIS IS THE ONLY SUR TOKEN
    // FOLLOWING TOKEN ARE ONLY FOR CONTEXT IN THE LEXER
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
    TOKEN_ASSIGNMENT_WORD,
    TOKEN_LEFT_PAR,
    TOKEN_RIGHT_PAR,
    TOKEN_LEFT_ACC,
    TOKEN_RIGHT_ACC,
    TOKEN_BACKQUOTES
}; // 38

struct token
{
    enum token_type use;
    enum token_type type; // type of the token
    char *value; // if no value -> set to null
};
#endif /* ! TOKEN_H */
