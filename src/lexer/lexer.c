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
    struct token tok = { 0, 0, NULL }; // valeur de base
    lexer->curr_token = tok;
    lexer->prev_pos = 0;
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

    else if (strcmp(c, "{") == 0)
        return TOKEN_LEFT_ACC;

    else if (strcmp(c, "}") == 0)
        return TOKEN_RIGHT_ACC;
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
    return TOKEN_WORDS; // 31
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
    case '(':
        return 2;
    case ')':
        return 2;
    case '`':
        return 2;
    case '\n':
        return 2; // 2 = delim que je tokenize
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
    enum token_type potential_context = type; // pour les types un peu speciaux
    if (type == TOKEN_WORDS || type == TOKEN_D_QUOTE || type == TOKEN_S_QUOTE)
    // IO_NUMBER
    {
        if (type == TOKEN_S_QUOTE || type == TOKEN_D_QUOTE)
            type = TOKEN_WORDS;
        else
            potential_context = keyword_type(buffer, lexer);
    }
    struct token tok = { potential_context, type, buffer };
    return tok;
}

static struct token read_comment(struct lexer *lexer)
{
    while (lexer->input[lexer->pos] != '\0' && lexer->input[lexer->pos] != '\n')
    {
        lexer->pos++; // passe tout jusqu'a la fin ou un \n pour les comment
    }
    return create_token(NULL, TOKEN_COMMENT, lexer);
}

static struct token read_delim(struct lexer *lexer)
// Juste les token pour des delimiteur speciaux
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
    if (curr_char == '(')
    {
        return create_token(NULL, TOKEN_LEFT_PAR, lexer);
    }
    if (curr_char == ')')
    {
        return create_token(NULL, TOKEN_RIGHT_PAR, lexer);
    }
    if (curr_char == '`')
    {
        return create_token(NULL, TOKEN_BACKQUOTES, lexer);
    }
    return create_token(NULL, TOKEN_INVALID, lexer);
}

static int read_s_quote(struct lexer *lexer, char *buffer, size_t i)
// actualise le buffer de read_word pour ajouter les quotes
// i c'est la position dans le buffer
{
    size_t size = strlen(lexer->input);
    ;
    lexer->pos++; // passe le premier '
    while (lexer->input[lexer->pos] != '\'' && lexer->input[lexer->pos] != '\0')
    {
        buffer[i++] = lexer->input[lexer->pos];
        if (i > size - 2) // -1 pour eviter les overflow et -2 pour \0
        {
            size *= 2;
            buffer = realloc(buffer, size);
        }
        lexer->pos++;
    }
    if (lexer->input[lexer->pos] == '\0')
    {
        free(buffer);
        fprintf(stderr, "BAD SINGLE QUOTE\n");
        exit(2);
    }
    lexer->pos++; // passe le 2eme '
    return i;
}

static int read_d_quote(struct lexer *lexer, char *buffer, size_t i)
{
    size_t size = strlen(lexer->input);
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
        fprintf(stderr, "BAD DOUBLE QUOTE\n");
        exit(2);
    }
    lexer->pos++;
    return i;
}

static struct token read_word(struct lexer *lexer)
{
    int already_caught_equal = 0;
    enum token_type type = TOKEN_WORDS;
    size_t size = strlen(lexer->input);
    char *buffer = calloc(size + 1, sizeof(char));
    size_t i = 0;
    while (!is_delim(lexer->input[lexer->pos])
           && !is_ope(lexer->input[lexer->pos]))
    {
        if (lexer->input[lexer->pos] == '\\')
        {
            lexer->pos++;
        }

        if (lexer->input[lexer->pos] == '\'')
        {
            i = read_s_quote(lexer, buffer, i);
            type = TOKEN_S_QUOTE;
            continue;
        }

        if (lexer->input[lexer->pos] == '\"')
        {
            i = read_d_quote(lexer, buffer, i);
            type = TOKEN_D_QUOTE;
            continue;
        }
        if (lexer->input[lexer->pos] == '=')
        {
            if (i != 0 && !already_caught_equal)
            {
                lexer->pos++;
                return create_token(buffer, TOKEN_ASSIGNMENT_WORD, lexer);
            }
            already_caught_equal = 1;
        }
        buffer[i++] = lexer->input[lexer->pos++];
        if (i > size - 2)
        {
            size *= 2;
            buffer = realloc(buffer, size);
        }
    }
    buffer[i] = '\0';
    return create_token(buffer, type, lexer);
}

static struct token read_operator(struct lexer *lexer) // 37
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
    if (curr_char == '#')
        return read_comment(lexer);
    if (is_delim(curr_char))
    {
        return read_delim(lexer);
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
    lexer->prev_pos = lexer->pos;
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

struct token lexer_time_machine(struct lexer *lexer)
{
    lexer->pos = lexer->prev_pos;
    return lexer_pop(lexer);
}
