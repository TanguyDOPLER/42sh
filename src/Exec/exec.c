#include "exec.h"

int exec_ast(struct ast *ast)
{ // on étudie le type du premier noeud de l'ast
    if (ast == NULL)
        return 0;
    switch (ast->type)
    {
    case AST_LIST:
        return exec_list(ast);
    case AST_AND_OR:
        return exec_and_or(ast);
    case PIPELINE:
        return 2;
    case AST_SIMPLE_COMMAND:
        return exec_simple_command(ast);
    case AST_SHELL_COMMAND:
        return exec_shell_command(ast);
    case AST_ARGUMENTS:
        fprintf(stderr, "got AST_ARGUMENTS nodes in fonction exec_ast\n");
        return 2; // return 2 = soucis dans le code
    default:
        fprintf(stderr, "got UNKNOWN nodes in fonction exec_ast\n");
        return 2;
    }
}

int exec_list(struct ast *ast)
{
    if (ast == NULL) // on fait rien si on tombe sur null
        return 0;
    int result = exec_ast(ast->left); // on exec le premier fils
    if (result == 2) // gestion erreur
    {
        fprintf(stderr, "exec_list, failed on execution of first son\n");
        return 2;
    }
    ast = ast->right; // on passe au deuxième fils (implémentation fils gauche
                      // frr droit)
    while (ast != NULL && ast->type == AST_LIST)
    {
        result = exec_ast(ast->left); // on exec le noeud suivant
        if (result == 2) // gestion erreur, ici c'est si ca a crash
        {
            fprintf(stderr,
                    "exec_list, failed on execution of on of the sons\n");
            return 2;
        }
        ast = ast->right; // on passe au frère suivant
    }
    return result; // normalement ici c'est comme return 0;
}

int exec_and_or(struct ast *ast)
{
    if (ast == NULL) // probleme compilation, unused ast
        return 2;
    return 2; // normalement on n'a pas encore de node "AST_AND_OR", donc c'est
              // pas normal;
}

int detect_builtin(int argc, char **argv) // argv null terminated
{
    if (!strcmp(argv[0], "echo")) // cas où on tombe sur le builtin echo
    {
        int res = builtin_echo(argc - 1, argv); // call fonction builtin
        return res;
    }
    else if (!strcmp(argv[0], "true")) // cas où on tombe sur le builtin true
        return builtin_true();
    else if (!strcmp(argv[0], "false")) // cas où on tombe sur le builtin echo
        return builtin_false();
    return -3; // on a pas détecter de builtin, donc on continu sur execvp
}

static int count_arguments(struct ast *ast)
{
    int count = 1; // Initialisé à 1 pour inclure le NULL final
    while (ast) // on compte le nb d'arguments y compris la commande. Cf. man
                // page execvp(3)
    {
        count++;
        ast = ast->left;
    }
    return count;
}

int exec_simple_command(struct ast *ast)
{
    int number_argument = count_arguments(ast);
    char **argv = calloc(
        number_argument,
        sizeof(char *)); // initialisation du tableau de char * pour execvp
    for (int i = 0; i + 1 < number_argument; i++) // on rempli le tableau
    {
        argv[i] = ast->value; // on prend la valeur de chaque noeud à gauche
        ast = ast->left;
    }
    int res_builtin = detect_builtin(
        number_argument, argv); // on detecte si on est sur un builtins
    if (res_builtin != -3) // cas où on est sur un builtin
    {
        int i = 0;
        while (argv[i]) // on free le tableau d'argument
        {
            free(argv[i]); // ca free ici les token.value normalement
            i++;
        }
        free(argv); // free le tableau completement
        return res_builtin; // free result builtins
    }
    int id = fork(); // fork pour execvp
    if (id == 0) // enfant
    {
        if (execvp(argv[0], argv) == -1) // on execute la commande
        {
            fprintf(stderr, "non existing command\n"); // ca se passe mal
            exit(127); // jsp pq 127 mais ca vient de 21sh donc ca marche
        }
        return 0; // ca se passe bien
    }
    else // parent
    {
        int status;
        waitpid(id, &status, 0); // on attend l'enfant
        int exit_stat;
        exit_stat = WEXITSTATUS(status); // on récupère le résultat de l'enfant
        int i = 0;
        while (argv[i]) // on free le tableau d'argument
        {
            free(argv[i]); // ca free ici les token.value normalement
            i++;
        }
        free(argv); // free le tableau completement
        if (exit_stat == 127) // on gère en fonction
        {
            fprintf(stderr,
                    "failed on exec simple command. returned result from child "
                    "process is 127\n");
            return 127;
        }
        // printf("%s exited with %d!\n", argv[0], exit_stat); //ca servira
        // surement pour debug
        return exit_stat; // a changer ici prcq ca marche pas
    }
}

void free_other_branch_if(struct ast *ast)
{
    if (ast == NULL)
        return;
    if (ast->type == AST_SIMPLE_COMMAND || ast->type == AST_ARGUMENTS)
        free(ast->value);
    free_other_branch_if(ast->left);
    free_other_branch_if(ast->right);
    free_other_branch_if(ast->condition);
}

int exec_shell_command(struct ast *ast)
{
    int condition = exec_ast(ast->condition); // on évalue la condition
    if (condition == 2) // gestion erreur
    {
        fprintf(stderr, "failed on exec shell command condition\n");
        return 2;
    }
    if (condition) // on execute right si la condition est à 1 (fausse)
    {
        free_other_branch_if(ast->left);
        return exec_ast(ast->right);
    }
    free_other_branch_if(ast->right);
    return exec_ast(ast->left); // sinon on execute left
}
