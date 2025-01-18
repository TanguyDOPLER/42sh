#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>

#include "../ast/ast.h"
#include "../lexer/lexer.h"

enum parser_status
{
    PARSER_OK,
    PARSER_UNEXPECTED_TOKEN,
};

/**
 * input=   list '\n'
 *      | list EOF
 *      | '\n'
 *      | EOF;
 */
struct ast *parse(enum parser_status *status, struct lexer *lexer);
struct ast *compound_list(enum parser_status *status, struct lexer *lexer);
struct ast *and_or(enum parser_status *status, struct lexer *lexer);
struct ast *shell_command(enum parser_status *status, struct lexer *lexer);
struct ast *command(enum parser_status *status, struct lexer *lexer);
struct ast *redirection(enum parser_status *status, struct lexer *lexer);
struct ast *handle_prefix(enum parser_status *status, struct lexer *lexer,
                          int *prefix_found);
struct ast *handle_elements(enum parser_status *status, struct lexer *lexer,
                            struct ast *command);
#endif /* !PARSER_H */
