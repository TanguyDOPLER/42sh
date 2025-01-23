#include "lexer/token.h"
#define _POSIX_C_SOURCE 200809L

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "Exec/exec.h"
#include "ast/ast.h"
#include "lexer/lexer.h"
#include "lexer/token.h"
#include "parser/parser.h"
#include "utils/shelldon.h"

const char *token_type_to_string(enum token_type type)
{
    switch (type)
    {
    case TOKEN_WORDS:
        return "TOKEN_WORDS";
    case TOKEN_OPERATOR:
        return "TOKEN_OPERATOR";
    case TOKEN_IF:
        return "TOKEN_IF";
    case TOKEN_THEN:
        return "TOKEN_THEN";
    case TOKEN_ELIF:
        return "TOKEN_ELIF";
    case TOKEN_ELSE:
        return "TOKEN_ELSE";
    case TOKEN_FI:
        return "TOKEN_FI";
    case TOKEN_SEMI_COLON:
        return "TOKEN_SEMI_COLON";
    case TOKEN_EOL:
        return "TOKEN_EOL";
    case TOKEN_EOF:
        return "TOKEN_EOF";
    case TOKEN_S_QUOTE:
        return "TOKEN_S_QUOTE";
    case TOKEN_D_QUOTE:
        return "TOKEN_D_QUOTE";
    case TOKEN_VAR:
        return "TOKEN_VAR";
    case TOKEN_COMMENT:
        return "TOKEN_COMMENT";
    case TOKEN_REDIR_INPUT:
        return "TOKEN_REDIR_INPUT";
    case TOKEN_REDIR_OUTPUT:
        return "TOKEN_REDIR_OUTPUT";
    case TOKEN_REDIR_AP_OUTPUT:
        return "TOKEN_REDIR_AP_OUTPUT";
    case TOKEN_REDIR_DUP_OUTPUT:
        return "TOKEN_REDIR_DUP_OUTPUT";
    case TOKEN_REDIR_DUP_INPUT:
        return "TOKEN_REDIR_DUP_INPUT";
    case TOKEN_REDIR_DOUBLE:
        return "TOKEN_REDIR_DOUBLE";
    case TOKEN_PIPE:
        return "TOKEN_PIPE";
    case TOKEN_OR:
        return "TOKEN_OR";
    case TOKEN_AND:
        return "TOKEN_AND";
    case TOKEN_WHILE:
        return "TOKEN_WHILE";
    case TOKEN_UNTIL:
        return "TOKEN_UNTIL";
    case TOKEN_DO:
        return "TOKEN_DO";
    case TOKEN_DONE:
        return "TOKEN_DONE";
    case TOKEN_FOR:
        return "TOKEN_FOR";
    case TOKEN_IN:
        return "TOKEN_IN";
    case TOKEN_NEGATION:
        return "TOKEN_NEGATION";
    case TOKEN_IONUMBER:
        return "TOKEN_IONUMBER";
    case TOKEN_INVALID:
        return "TOKEN_INVALID";
    case TOKEN_ASSIGNMENT_WORD:
        return "TOKEN_ASSIGNMENT_WORD";
    default:
        return "UNKNOWN_TOKEN_TYPE";
    }
}

struct shelldon shelldon = { NULL };

char *file_reader(char *input)
{
    size_t size = 1024;
    char *buffer = malloc(size);
    FILE *stream = fopen(input, "r");
    if (!stream)
        errx(EXIT_FAILURE, "ERROR INPUT FILE");
    size_t i = 0;
    int c = '\0';
    while ((c = fgetc(stream)) != -1)
    {
        buffer[i++] = c;
        if (i > size - 2) // 1023 - 1 for \0
        {
            size *= 2;
            buffer = realloc(buffer, size);
        }
    }
    buffer[i] = '\0';
    fclose(stream);
    return buffer;
}

char *stdin_reader(void)
{
    size_t size = 1024;
    char *buffer = malloc(size);
    FILE *stream = stdin;
    if (!stream)
        errx(EXIT_FAILURE, "ERROR INPUT FILE");
    size_t i = 0;
    int c = '\0';
    while ((c = fgetc(stream)) != -1)
    {
        buffer[i++] = c;
        if (i > size - 2) // 1023 - 1 for \0
        {
            size *= 2;
            buffer = realloc(buffer, size);
        }
    }
    buffer[i] = '\0';
    // fclose(stream);
    return buffer;
}

int is_regular_file(const char *input) // check si c'est un ficher valide
{
    struct stat input_s;
    stat(input, &input_s);
    return S_ISREG(input_s.st_mode);
}

int exec_42sh(char *buff, int flag) // execution en cas de non stdin
{
    int res = 0;
    if (strlen(buff) > 0 && buff[strlen(buff) - 1] == '\n') // retrait dernier
                                                            // retour a la ligne

        buff[strlen(buff) - 1] = '\0';
    struct lexer *lexer = lexer_init(buff); // FAIRE UN WHILE CURR CHAR PAS EOF
    while (lexer->curr_token.type != TOKEN_EOF)
    {
        struct token tok = lexer_pop(lexer);
        printf("USE : %s, TYPE :%s, BUFFER %s\n", token_type_to_string(tok.use),
               token_type_to_string(tok.type), tok.value);
    }
    lexer_free(lexer);
    if (flag)
        free(buff);
    shelldon_free(); // struct globale shelldon totalement free
    return res;
}

int main(int argc, char *argv[])
{
    // Initialiser la structure globale
    shelldon_init();
    char *buff = NULL;
    int is_regfile = 0;
    if (argc > 1)
    {
        is_regfile = is_regular_file(argv[1]);
    }
    if (argc >= 2 && strcmp(argv[1], "-c") == 0) // Gestion string et option -c
    {
        buff = argv[2];
    }
    if (is_regular_file(argv[1])) // Gestion ficher en parametre
        buff = file_reader(argv[1]);
    if (!buff && argc >= 2) // Prend le premier args sauf si buffer deja remplie
        buff = argv[1];
    if (!buff)
    {
        buff = stdin_reader();
        is_regfile++;
    }
    return exec_42sh(buff, is_regfile);
}
