#define _POSIX_C_SOURCE 200809L

#include "exec.h"

#include <string.h>

#include "../utils/variable.h"

static int exec_list(struct ast *ast)
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

static int exec_and_or(struct ast *ast)
{ // dans notre programme, 0 = true, 1 = false, 2 = erreur
    if (ast == NULL)
        return 0;
    int res;
    if (ast->type == AST_AND) // cas AND
    {
        if ((res = exec_ast(ast->left)) != 0) // left associativity
            return res;
        return exec_ast(ast->right);
    }
    else if (ast->type == AST_OR) // cas OR
    {
        if ((res = exec_ast(ast->left)) == 0) // left associativity
            return res;
        if (res == 2)
        {
            fprintf(stderr,
                    "exec_and_or(or), failed on execution of ast.left\n");
            return 2;
        }
        return exec_ast(ast->right);
    }
    else // WRONG CASE
    {
        fprintf(stderr, "exec_and_or, got wrong ast.type\n");
        return 2;
    }
}

static int detect_builtin(int argc, char **argv) // argv null terminated
{
    if (!strcmp(argv[0], "echo")) // cas où on tombe sur le builtin echo
    {
        return builtin_echo(argc - 1, argv); // call fonction builtin
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

static char **create_argument_list(struct ast *ast)
{
    int number_argument = count_arguments(ast);
    char **argv = calloc(
        number_argument,
        sizeof(char *)); // initialisation du tableau de char * pour execvp
    for (int i = 0; i + 1 < number_argument; i++) // on rempli le tableau
    {
        if (i == 0) // le premier élément trouver est le nom de la commande
                    // donc pas une commande
        {
            argv[i] = ast->value;
        }
        else
        {
            int is_var = is_variable(&ast->value);
            if (is_var == 0) // pas une variable
            {
                argv[i] =
                    ast->value; // on prend la valeur de chaque noeud à gauche
            }
            else if (is_var == 1) // une variable
            {
                argv[i] = value_of_variable(ast->value);
            }
            else // erreur
            {
                free(argv);
                fprintf(stderr, "%s: bad substitution\n", ast->value);
                return NULL;
            }
        }
        ast = ast->left;
    }
    return argv;
}

static int exec_simple_command(struct ast *ast)
{
    int resultat = 0;
    char **argv = create_argument_list(ast);
    if (argv == NULL)
        return 2;
    resultat = detect_builtin(count_arguments(ast),
                              argv); // on detecte si on est sur un builtins
    if (resultat != -3) // cas où on est sur un builtin
    {
        free(argv); // free le tableau completement
        return resultat; // free result builtins
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
        free(argv); // free le tableau completement
        if (exit_stat == 127) // on gère en fonction
        {
            fprintf(stderr,
                    "failed on exec simple command. returned result from child "
                    "process is 127\n");
            return 127;
        }
        return exit_stat; // a changer ici prcq ca marche pas
    }
}

static int exec_while_until(struct ast *ast)
{
    int res = 0;
    if (ast->type == AST_WHILE)
    {
        while (exec_ast(ast->condition) == 0)
            res = exec_ast(ast->left);
    }
    else if (ast->type == AST_UNTIL)
    {
        while (exec_ast(ast->condition)
               != 0) // il se passe quoi si la condition crach?
            res = exec_ast(ast->left);
    }
    else
    {
        return 2;
    }
    return res;
}

static int exec_shell_command(struct ast *ast)
{
    int condition = exec_ast(ast->condition); // on évalue la condition
    if (condition == 2) // gestion erreur
    {
        fprintf(stderr, "failed on exec shell command condition\n");
        return 2;
    }
    if (condition) // on execute right si la condition est à 1 (fausse)
    {
        return exec_ast(ast->right);
    }
    return exec_ast(ast->left); // sinon on execute left
}

/*static int count_command_(struct ast *ast)
{
    int count = 0;
    while (ast != NULL && ast->type == AST_PIPE)
    {
        count++;
        ast = ast->right;
    }
    return count;
}*/

static int exec_pipe(struct ast *ast)
{
    if (ast == NULL)
        return 0;
    if (ast->right
        == NULL) // cas où on a juste une simple commande, on se fait pas chier
        return exec_ast(ast->left);
    int save_stdin = 3333; // value to save STDIN
    dup2(STDIN_FILENO, save_stdin); // saving STDIN
    int pipefds[2];
    if (pipe(pipefds) == -1)
    {
        return 2;
    }
    int id = fork(); // FIRST commande (left one)
    if (id == 0) // child process
    {
        close(pipefds[0]); // Cf vide conf redirection 2019
        dup2(pipefds[1], STDOUT_FILENO);
        close(pipefds[1]);
        exit(exec_ast(ast->left));
    }
    else // parent process
    {
        close(pipefds[1]);
        waitpid(id, NULL, 0);
    }
    int id2 = fork(); // seoncd commande (right one)
    if (id2 == 0)
    {
        close(pipefds[1]); // Cf vide conf redirection 2019
        dup2(pipefds[0], STDIN_FILENO);
        close(pipefds[0]);
        exit(exec_ast(ast->right));
    }
    else
    {
        int status2;
        close(pipefds[0]);
        close(pipefds[1]);
        waitpid(id2, &status2, 0);
        int exit_stat2;
        exit_stat2 = WEXITSTATUS(status2);
        dup2(save_stdin, STDIN_FILENO);
        return exit_stat2;
    }
}

static int exec_for(struct ast *ast)
{
    if (ast == NULL)
        return 0;
    return 0;
}

// permet d'enregistrer la variable
int exec_assign_var(struct ast *ast)
{
    char *name = strdup(ast->value); // copie le nom de la variable
    if (ast->left->type != AST_SIMPLE_COMMAND)
        return 2;
    char *val = strdup(
        ast->left->value); // copie la valeur de la variable (qui est dans left)
    variable_add(name, val); // enregistrement de la variable dans shelldon ;)
    return 0;
}

int exec_ast(struct ast *ast)
{ // on étudie le type du premier noeud de l'ast
    if (ast == NULL)
        return 0;
    if (ast->type >= AST_REDIR_INPUT && ast->type <= AST_REDIR_DOUBLE)
        return exec_redir(ast);
    switch (ast->type)
    {
    case AST_LIST:
        return exec_list(ast);
    case AST_AND:
    case AST_OR:
        return exec_and_or(ast);
    case AST_SIMPLE_COMMAND:
        return exec_simple_command(ast);
    case AST_SHELL_COMMAND:
        return exec_shell_command(ast);
    case AST_PIPE:
        return exec_pipe(ast);
    case AST_NEGATION:
        return (!exec_ast(ast->left));
    case AST_UNTIL:
    case AST_WHILE:
        return exec_while_until(ast);
    case AST_FOR:
        return exec_for(ast);
    case AST_ARGUMENTS:
        fprintf(stderr, "got AST_ARGUMENTS nodes in fonction exec_ast\n");
        return 2; // return 2 = soucis dans le code
    default:
        fprintf(stderr, "got UNKNOWN nodes in fonction exec_ast\n");
        return 2;
    }
}
