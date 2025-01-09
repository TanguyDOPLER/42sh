#ifndef PARSER_H
#define PARSER_H

#include "ast/ast.h"
#include "lexer/lexer.h"

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

/**
 * list = and_or;
 */
struct ast *list(enum parser_status *status, struct lexer *lexer);

/**
 * and_or = pipeline ;
 */
struct ast *and_or(enum parser_status *status, struct lexer *lexer);

/**
 *  pipeline = command ;
 */
struct ast *pipeline(enum parser_status *status, struct lexer *lexer);

/**
 * command = simple_command ;
 */
struct ast *command(enum parser_status *status, struct lexer *lexer);

/**
 * simple_command = WORD { element } ;
 */
struct ast *simple_command(enum parser_status *status, struct lexer *lexer);

/*
 * (* for the time being, it is limited to a single rule_if *)
 *   shell_command = rule_if ;
 */
struct ast *shell_command(enum parser_status *status, struct lexer *lexer);

/*
 * rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi' ;
 */
struct ast *rule_if(enum parser_status *status, struct lexer *lexer);

/**
 * element = WORD ;
 */
struct ast *element(enum parser_status *status, struct lexer *lexer);

/**
 * compound_list = {'\n'} and_or { ( ';' | '\n' ) {'\n'} and_or } [';'] {'\n'} ;
 */
struct ast *compound_list(enum parser_status *status, struct lexer *lexer);

struct ast *else_clause(enum parser_status *status, struct lexer *lexer);

#endif /* !PARSER_H */
