#include "lexer.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"

static int isnumber(char c)
{
    return (c >= '0' && c <= '9');
}

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

/*
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
*/
static enum token_type keyword_type(char *c, struct lexer *lexer)
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

    else if (strcmp(c, "while") == 0)
        return TOKEN_WHILE;

    else if (strcmp(c, "until") == 0)
        return TOKEN_UNTIL;

    else if (strcmp(c, "do") == 0)
        return TOKEN_DO;

    else if (strcmp(c, "done") == 0)
        return TOKEN_DONE;

    else if (strcmp(c, "for") == 0)
        return TOKEN_FOR;

    else if (strcmp(c, "in") == 0)
        return TOKEN_IN;

    else
    {
        size_t i = 0;
        while (isnumber(c[i]))
            i++;
        if (i == strlen(c))
            if (lexer->input[lexer->pos] == '>'
                || lexer->input[lexer->pos] == '<')
                return TOKEN_IONUMBER;
    }
    return TOKEN_WORDS;
}

static enum token_use token_use(char *c, struct lexer *lexer)
{
    if (c && (c[0] == '>' || c[0] == '<'))
        return TYPE_REDIR;
    if (c && lexer->pos > 0 && lexer->input[lexer->pos - 1] == '\"')
        return TYPE_DOUBLE_QUOTES;
    return TYPE_OTHER;
}

static int is_ope(char curr_char)
{
    switch (curr_char)
    {
    case '>':
        return 2;
    case '<':
        return 2;
    case '&':
        return 2;
    case '|':
        return 2;
    case '$':
        return 2;
    case '!':
        return 2;
    default:
        return 0;
    }
}

static int is_delim(char curr_char)
{
    switch (curr_char)
    {
    case ' ':
        return 1;
    case '\t':
        return 1;
    case '\0':
        return 1;
    case '\n':
        return 1;
    case ';':
        return 2;
    default:
        return 0;
    }
}

/*
static struct token create_token(char *buffer, int flag, struct lexer *lexer) //
creation du token a return
{
    enum token_type type = token_type(buffer, flag, lexer);
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
*/

static struct token create_token(char *buffer, enum token_type type,
                                 struct lexer *lexer)
{
    enum token_type type_exact = type;
    enum token_use use = token_use(buffer, lexer);

    if (type == TOKEN_WORDS)
    {
        type_exact = keyword_type(buffer, lexer);
        if (type_exact != TOKEN_WORDS && type_exact != TOKEN_IONUMBER)
            free(buffer);
    }
    else if (type == TOKEN_INVALID)
    {
        err(EXIT_FAILURE, "ERROR SYNTAX");
    }
    struct token tok = { use, type_exact, buffer };
    if (lexer) // just for compilation
        return tok;
    return tok;
}

static struct token read_delim(struct lexer *lexer)
{
    char curr_char = lexer->input[lexer->pos++];
    if (curr_char == '\n')
    {
        return create_token(NULL, TOKEN_EOL, lexer);
    }
    if (curr_char == ';')
    {
        return create_token(NULL, TOKEN_SEMI_COLON, lexer);
    }
    return create_token(NULL, TOKEN_INVALID, lexer);
}

static struct token read_comment(struct lexer *lexer)
{
    while (lexer->input[lexer->pos] != '\0' && lexer->input[lexer->pos] != '\n')
    {
        lexer->pos++;
    }
    return create_token(NULL, TOKEN_COMMENT, lexer);
}

static struct token read_s_quote(struct lexer *lexer)
{
    size_t size = 128;
    char *buffer = calloc(size, sizeof(char));
    size_t i = 0;
    lexer->pos++;
    while (lexer->input[lexer->pos] != '\'' && lexer->input[lexer->pos] != '\0')
    {
        buffer[i++] = lexer->input[lexer->pos];
        if (i > size - 2)
        {
            size *= 2;
            buffer = realloc(buffer, size);
        }
        lexer->pos++;
    }
    if (lexer->input[lexer->pos] == '\0')
    {
        free(buffer);
        errx(EXIT_FAILURE, "ERROR SYNTAX");
    }
    lexer->pos++;
    buffer[i] = '\0';
    return create_token(buffer, TOKEN_WORDS, lexer);
}

static struct token read_d_quote(struct lexer *lexer)
{
    size_t size = 128;
    char *buffer = calloc(size, sizeof(char));
    size_t i = 0;
    lexer->pos++;
    while (lexer->input[lexer->pos] != '\"' && lexer->input[lexer->pos] != '\0')
    {
        if (lexer->input[lexer->pos] == '\\')
        {
            lexer->pos++;
        }
        buffer[i++] = lexer->input[lexer->pos];
        if (i > size - 2)
        {
            size *= 2;
            buffer = realloc(buffer, size);
        }
        lexer->pos++;
    }
    if (lexer->input[lexer->pos] == '\0')
    {
        free(buffer);
        errx(EXIT_FAILURE, "ERROR SYNTAX");
    }
    lexer->pos++;
    buffer[i] = '\0';
    return create_token(buffer, TOKEN_WORDS, lexer);
}

static struct token read_word(struct lexer *lexer)
{
    size_t size = 128;
    char *buffer = calloc(size, sizeof(char));
    size_t i = 0;
    while (!is_delim(lexer->input[lexer->pos])
           && !is_ope(lexer->input[lexer->pos]))
    {
        if (lexer->input[lexer->pos] == '\\')
        {
            lexer->pos++;
        }
        buffer[i++] = lexer->input[lexer->pos++];
        if (i > size - 2)
        {
            size *= 2;
            buffer = realloc(buffer, size);
        }
    }
    buffer[i] = '\0';
    return create_token(buffer, TOKEN_WORDS, lexer);
}

static struct token read_operator(struct lexer *lexer)
{
    char curr_char = lexer->input[lexer->pos++];
    if (curr_char == '!')
        return create_token(NULL, TOKEN_NEGATION, lexer);
    if (curr_char == '>')
    {
        curr_char = lexer->input[lexer->pos++];
        if (curr_char == '>')
            return create_token(NULL, TOKEN_REDIR_AP_OUTPUT, lexer);
        else if (curr_char == '&')
            return create_token(NULL, TOKEN_REDIR_DUP_OUTPUT, lexer);
        else if (curr_char == '|')
            return create_token(NULL, TOKEN_REDIR_OUTPUT, lexer);
        else
        {
            lexer->pos--;
            return create_token(NULL, TOKEN_REDIR_OUTPUT, lexer);
        }
    }
    else if (curr_char == '<')
    {
        curr_char = lexer->input[lexer->pos++];
        if (curr_char == '&')
            return create_token(NULL, TOKEN_REDIR_DUP_INPUT, lexer);
        else if (curr_char == '>')
            return create_token(NULL, TOKEN_REDIR_DOUBLE, lexer);
        else
        {
            lexer->pos--;
            return create_token(NULL, TOKEN_REDIR_INPUT, lexer);
        }
    }
    else if (curr_char == '&')
    {
        curr_char = lexer->input[lexer->pos++];
        if (curr_char == '&')
            return create_token(NULL, TOKEN_AND, lexer);
    }
    else if (curr_char == '|')
    {
        curr_char = lexer->input[lexer->pos++];
        if (curr_char == '|')
            return create_token(NULL, TOKEN_OR, lexer);
        else
        {
            lexer->pos--;
            return create_token(NULL, TOKEN_PIPE, lexer);
        }
    }
    return create_token(NULL, TOKEN_INVALID, lexer);
}

struct token lexer_next_token(struct lexer *lexer)
{
    char curr_char = lexer->input[lexer->pos];
    if (curr_char == '\0')
    {
        return create_token(NULL, TOKEN_EOF, lexer);
    }
    if (curr_char == ' ' || curr_char == '\t')
    {
        lexer->pos++;
        return lexer_next_token(lexer);
    }
    if (is_delim(curr_char))
    {
        return read_delim(lexer);
    }
    if (curr_char == '#')
    {
        return read_comment(lexer);
    }
    if (curr_char == '\'')
    {
        return read_s_quote(lexer);
    }
    if (curr_char == '\"')
    {
        return read_d_quote(lexer);
    }
    if (is_ope(curr_char))
    {
        return read_operator(lexer);
    }
    return read_word(lexer);
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
