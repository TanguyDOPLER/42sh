#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../parser/parser.h"
#include "ast.h"
#include "exec.h"
#include "lexer.h"
// Fonction pour convertir l'ast_type en chaîne de caractères
char *ast_type_to_string(struct ast *node)
{
    switch (node->type)
    {
    case AST_LIST:
        return "AST_LIST";
    case AST_AND_OR:
        return "AST_AND_OR";
    case PIPELINE:
        return "PIPELINE";
    case AST_SIMPLE_COMMAND:
        return "AST_SIMPLE_COMMAND";
    case AST_SHELL_COMMAND:
        return "AST_SHELL_COMMAND (if en gros)";
    case AST_ARGUMENTS:
        return "AST_ARGUMENTS";
    default:
        return "UNKNOWN";
    }
}

// Fonction pour afficher l'arbre AST en format DOT
void print_ast_dot(struct ast *node, FILE *f)
{
    if (node == NULL)
        return;

    // Identifier le noeud
    fprintf(f, "  node%p [label=\"%s: %s\"];\n", (void *)node,
            ast_type_to_string(node), node->value ? node->value : "");

    // Si il y a un fils gauche, on crée l'arc
    if (node->left)
    {
        fprintf(f, "  node%p -> node%p [label=\"left\"];\n", (void *)node,
                (void *)(node->left));
        print_ast_dot(node->left, f);
    }

    // Si il y a un fils droit, on crée l'arc
    if (node->right)
    {
        fprintf(f, "  node%p -> node%p [label=\"right\"];\n", (void *)node,
                (void *)(node->right));
        print_ast_dot(node->right, f);
    }
}

int main(int argc, char *argv[])
{
    struct lexer *lexer = lexer_init(argv[1]);
    enum parser_status status = PARSER_OK;
    struct ast *ast = parse(&status, lexer);
    FILE *f = fopen("ast_graph.dot", "w");
    if (f == NULL)
    {
        perror("Failed to open file");
        return 1;
    }

    // Début du fichier DOT
    fprintf(f, "digraph ast {\n");

    // Affichage de l'AST dans le fichier DOT
    print_ast_dot(ast, f);

    // Fin du fichier DOT
    fprintf(f, "}\n");

    // Fermer le fichier
    fclose(f);

    int arg = exec_ast(ast);
    printf("result : %d\n", arg);

    // printf("DOT file generated successfully.\n");
    ast_free(ast);
    lexer_free(lexer);
    return argc;
}
