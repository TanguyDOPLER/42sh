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

int is_regular_file(const char *input) // check si c'est un ficher valide
{
    struct stat input_s;
    stat(input, &input_s);
    return S_ISREG(input_s.st_mode);
}

void exec_42sh(char *buff, int flag) // execution en cas de non stdin
{
    if (strlen(buff) > 0 && buff[strlen(buff) - 1] == '\n')//retrait dernier
        //retour a la ligne
        buff[strlen(buff) - 1] = '\0';
    struct lexer *lexer = lexer_init(buff);
    enum parser_status status = PARSER_OK;
    struct ast *ast = parse(&status, lexer);
    exec_ast(ast);
    lexer_free(lexer);
    ast_free(ast);
    if (flag)
        free(buff);
}

int main(int argc, char *argv[])
{
    char *buff = NULL;
    if (argc >= 2 && strcmp(argv[1], "-c") == 0) // Gestion string et option -c
    {
        buff = argv[2];
    }
    if (is_regular_file(argv[1])) // Gestion ficher en parametre
        buff = file_reader(argv[1]);
    if (!buff && argc >= 2) // Prend le premier args sauf si buffer deja remplie
        buff = argv[1];
    if (buff) // Si buffer remplie alors pas de stdin alors exec_42
    {
        exec_42sh(buff, is_regular_file(argv[1]));
        return 0;
    }
    size_t size = 0;
    buff = NULL;
    ssize_t ending = 0; // check la fin
    while ((ending = getline(&buff, &size, stdin)) != -1)
    {
        // printf("42sh$ "); c'est pour faire beau
        //printf("%s\n",buff);// print le buffer
        if (strlen(buff) > 0 && buff[strlen(buff) - 1] == '\n')//retrait \n 
            //final
            buff[strlen(buff) - 1] = '\0';
        struct lexer *lexer = lexer_init(buff);
        enum parser_status status = PARSER_OK;
        struct ast *ast = parse(&status, lexer);
        exec_ast(ast);
        lexer_free(lexer);
        ast_free(ast);
    }
    free(buff);
    return 0;
}
