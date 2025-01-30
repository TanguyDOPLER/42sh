#include "builtins/builtins.h"
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
#include "utils/variable.h"

struct shelldon shelldon = { NULL, NULL, NULL, 0, NULL, NULL, NULL };

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

int exec_42sh(char *buff, int flag_reg_file,
              int flag_from_dot) // execution en cas de non stdin
{
    int res = 0;
    if (strlen(buff) > 0 && buff[strlen(buff) - 1] == '\n') // retrait dernier
        buff[strlen(buff) - 1] = '\0';
    struct lexer *lexer = lexer_init(buff); // FAIRE UN WHILE CURR CHAR PAS EOF
    struct exec_status exec_status = { EXEC_OK, 0 };
    while (lexer->curr_token.type != TOKEN_EOF && exec_status.status == EXEC_OK)
    {
        exec_status.status = EXEC_OK;
        enum parser_status status = PARSER_OK;
        struct ast *ast = parse(&status, lexer);
        if (status == PARSER_UNEXPECTED_TOKEN)
        {
            if (flag_reg_file)
                free(buff);
            lexer_free(lexer);
            return 2;
        }
        res = exec_ast(ast, &exec_status);
        ast_free(ast);
        update_res(res); // update $?
    }
    lexer_free(lexer);
    if (flag_reg_file)
        free(buff);
    if (flag_from_dot == 0)
        shelldon_free(); // struct globale shelldon totalement free
    if (exec_status.status == EXEC_EXIT && exec_status.exit_value != -1)
    { // on remet stderr en marche (Cf. builtin_exit.c)
        dup2(STDERR_FILENO, 3333);
        return exec_status.exit_value;
    }
    return res;
}

int exec_for_builtin_dot(char *argv)
{ // exec when we met builtin_dot
    int argc = 2;

    char *buff = NULL;
    int is_regfile = 0;
    if (argc > 1)
    {
        is_regfile = is_regular_file(argv);
    }
    if (is_regular_file(argv)) // Gestion ficher en parametre
        buff = file_reader(argv);
    else
    {
        fprintf(stderr, "exec dot : file invalid\n");
        return 1;
    }
    if (!buff)
    {
        buff = stdin_reader();
        is_regfile++;
    }
    return exec_42sh(buff, is_regfile, 1);
}

int main(int argc, char *argv[])
{
    shelldon_init(); // Initialiser la structure globale shelldon
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
    {
        buff = file_reader(argv[1]);
        list_args_init(argc - 1, argv + 1);
    }
    if (!buff && argc >= 2) // Prend le premier args sauf si buffer deja remplie
        buff = argv[1];
    if (!buff)
    {
        buff = stdin_reader();
        list_args_init(argc - 1, argv + 1);
        is_regfile++;
    }
    return exec_42sh(buff, is_regfile, 0);
}
