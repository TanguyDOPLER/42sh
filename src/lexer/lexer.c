#include "lexer.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct lexer *lexer_init(const char *input)
{
    struct lexer *lexer = calloc(1, sizeof(struct lexer));
    lexer->input = input;
    lexer->pos = 0;
    struct token tok = { 1, 1, NULL };
    lexer->curr_token = tok;
    return lexer;
}

void lexer_free(struct lexer *lexer)
{
    lexer->input = NULL;
    lexer->pos = 0;
    free(lexer);
}

static int is_exit(char *c, char type)
{
    int i = 0;
    while (c[i] != '\0')
    {
        if (c[i] == type)
        {
            return 0;
        }
    }
    return 1;
}

enum token_type token_type(char *c, int flag)
{
    if (strcmp(c, "if") == 0)
        return TOKEN_IF;

    else if (strcmp(c, "else") == 0)
        return TOKEN_ELSE;

    else if (strcmp(c, "then") == 0)
        return TOKEN_THEN;

    else if (strcmp(c, "elif") == 0)
        return TOKEN_ELIF;

    else if (strcmp(c, "fi") == 0)
        return TOKEN_FI;

    else if (strcmp(c, ";") == 0)
        return TOKEN_SEMI_COLON;

    else if (flag == '#')
        return TOKEN_COMMENT;

    else if (c[0] == '\0')
        return TOKEN_EOF;

    else if (c[0] == '\n')
        return TOKEN_EOL;

    else
    {
        if (c[strlen(c) - 1] == '\'')
        {
            if (is_exit(c, '\'') == 1)
                err(EXIT_FAILURE, "ERROR SYNTAX");
        }
        return TOKEN_WORDS;
    }
}

enum token_use token_use(char *c)
{
    if (strcmp(c, "if") == 0)
        return TYPE_KEYWORD;

    else if (strcmp(c, "else") == 0)
        return TYPE_KEYWORD;

    else if (strcmp(c, "then") == 0)
        return TYPE_KEYWORD;

    else if (strcmp(c, "elif") == 0)
        return TYPE_KEYWORD;

    else if (strcmp(c, "fi") == 0)
        return TYPE_KEYWORD;

    else if (strcmp(c, ";") == 0)
        return TYPE_COMPOUND;

    else if (c[0] == '\'')
        return TYPE_WORDS;

    else if (c[0] == '#')
        return TYPE_OTHER;

    else if (c[0] == '\n')
        return TYPE_COMPOUND;

    else if (c[0] == '\0')
        return TYPE_OTHER;

    else
        return TYPE_WORDS;
}

struct token create_token(char *buffer, int flag) // creation du token a return
{
    enum token_type type = token_type(buffer, flag);
    enum token_use use = TYPE_WORDS; // valeur de base
    use = token_use(buffer); // changer si j'ai le temps
    if (type != TOKEN_WORDS && type != TOKEN_S_QUOTE)
    {
        free(buffer);
        buffer = NULL;
    }
    struct token tok = { use, type, buffer };
    return tok;
}

struct token lexer_next_token(struct lexer *lexer)
{
    size_t size = 128;
    char *buffer = malloc(size); // va nicker ta mère maxime MAIS
                                 // VRAIMENTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
    size_t i = 0;
    int flag = 0; // Flag pour ' et #
    char curr_char = lexer->input[lexer->pos]; // Current Lexer Char
    while (curr_char == ' ') // retire les espaces parasites
        curr_char = lexer->input[++lexer->pos];
    while (curr_char != '\0'
           && (flag || (curr_char != ' ' && curr_char != ';')))
    {
        if (curr_char == '\'' || curr_char == '#')
        //# se ferme a la fin d'un \n
        //' doit se refermer sinon error
        {
            if (flag == '\'')
            {
                flag = 0;
            }
            else
                flag = curr_char;
        }
        if (flag == '#' && curr_char == '\n') // Stoper le com au 1er \n
        {
            break; // remettre le flag a 0 pour le prochain token
        }
        if (curr_char != '\'')
            buffer[i++] = curr_char;
        curr_char = lexer->input[++lexer->pos];
        if (i >= size - 2) // 127 - \0 - potentiel ;
        {
            size *= 2;
            buffer = realloc(buffer, size);
        }
    }
    if (i == 0 && curr_char == ';') // Ajout du ; pour le tokenizer en plus
    // de l'utiliser en delimiteur
    {
        buffer[i++] = ';';
        curr_char = lexer->input[++lexer->pos];
    }
    buffer[i] = '\0'; // Fin de string
    struct token token = create_token(buffer, flag);
    return token;
}

struct token lexer_pop(struct lexer *lexer)
{
    /*if (lexer->pos == 0 && lexer->curr_token.value != NULL) // cas où on a
    appelé peek avant le premier pop. Dans tous les cas le premier appelle de
    pop doit retourner le premier token
    {
        free(lexer->curr_token.value); //on évite les memory leak
    }*/
    struct token tok = lexer_next_token(lexer); // fonctionnement de base de pop
    lexer->curr_token = tok;
    return tok;
}

struct token lexer_peek(struct lexer *lexer)
{
    // curr_token.value == NULL veut dire qu'on est dans le tout premier appelle
    /*if (lexer->curr_token.value == NULL) // Cf. lexer_pop pour explication
    {
        lexer_pop(lexer);
        lexer->pos = 0;
    }*/
    return lexer->curr_token;
}
