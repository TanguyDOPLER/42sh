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
#include "parser/parser.h"
#include "utils/shelldon.h"

struct shelldon shelldon = { NULL };

char *file_reader(char *input)
{
    size_t size = 1024;
    char *buffer = malloc(size);
    FILE *stream = fopen(input, "r");
    if (!stream)
        errx(EXIT_FAILURE, "ERROR INPUT FILE");
    size_t i = 0;
    char c = '\0';
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
    char c = '\0';
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
        enum parser_status status = PARSER_OK;
        struct ast *ast = parse(&status, lexer);
        if (status == PARSER_UNEXPECTED_TOKEN)
        {
            if (flag)
                free(buff);
            lexer_free(lexer);
            return 2;
        }
        res = exec_ast(ast);
        ast_free(ast);
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
