#include "exec.h"

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
        return exec_simple_command(ast, -1, -1);
    case AST_SHELL_COMMAND:
        return exec_shell_command(ast);
    case AST_PIPE:
        return exec_pipe(ast);
    case AST_NEGATION:
        return (!exec_ast(ast->left));
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

static int exec_redir_output(char *filename, int io)
{
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
        fprintf(stderr, "exec_redir_output : open fd failed");
        return 2;
    }
    if (io == -1)
        io = 1;
    if (dup2(fd, io) == -1)
    {
        fprintf(stderr, "exec_redir_output : dup2 failed");
        close(fd);
        return 2;
    }

    close(fd);
    return 0;
}

static int exec_redir_input(char *filename, int io)
{
    int fd = open(filename, O_RDONLY, 0644);
    if (fd == -1)
    {
        fprintf(stderr, "exec_redir_input : open fd failed");
        return 2;
    }
    if (io == -1)
        io = 0;
    if (dup2(fd, io) == -1)
    {
        fprintf(stderr, "exec_redir_input : dup2 failed");
        close(fd);
        return 2;
    }

    close(fd);
    return 0;
}

static int exec_redir_output_append(char *filename, int io)
{
    int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1)
    {
        fprintf(stderr, "exec_redir_output_append : open fd failed");
        return 2;
    }
    if (io == -1)
        io = 1;
    if (dup2(fd, io) == -1)
    {
        fprintf(stderr, "exec_redir_output_append : dup2 failed");
        close(fd);
        return 2;
    }

    close(fd);
    return 0;
}

static int exec_redir_dup_output(char *filename, int io)
{
    int fd = atoi(filename);
    if (fd == -1)
    {
        fprintf(stderr, "exec_redir_dup_output : open fd failed");
        return 2;
    }
    if (io == -1)
        io = 1;
    if (dup2(fd, io) == -1)
    {
        fprintf(stderr, "exec_redir_dup_output : dup2 failed");
        close(fd);
        return 2;
    }

    close(fd);
    return 0;
}

static int exec_redir_dup_input(char *filename, int io)
{
    int fd = atoi(filename);
    if (fd == -1)
    {
        fprintf(stderr, "exec_redir_dup_input : open fd failed");
        return 2;
    }
    if (io == -1)
        io = 0;
    if (dup2(fd, io) == -1)
    {
        fprintf(stderr, "exec_redir_dup_input : dup2 failed");
        close(fd);
        return 2;
    }

    close(fd);
    return 0;
}

static int exec_redir_double(char *filename, int io)
{
    int fd_out = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_out == -1)
    {
        fprintf(stderr, "exec_redir_both : open fd failed");
        return 2;
    }
    int fd_in = open(filename, O_RDONLY, 0644);
    if (fd_in == -1)
    {
        fprintf(stderr, "exec_redir_both : open fd failed");
        return 2;
    }
    if (io == -1)
        io = 0;
    if (dup2(fd_out, io) == -1)
    {
        fprintf(stderr, "exec_redir_both : dup2 failed");
        close(fd_out);
        return 2;
    }
    if (dup2(fd_in, io) == -1)
    {
        fprintf(stderr, "exec_redir_both : dup2 failed");
        close(fd_in);
        return 2;
    }

    close(fd_out);
    close(fd_in);
    return 0;
}

int exec_redir(struct ast *ast)
{
    int saved_stdin = dup(STDIN_FILENO); // Sauvegarde de STDIN
    int saved_stdout = dup(STDOUT_FILENO); // Sauvegarde de STDOUT
    int saved_stderr = dup(STDERR_FILENO); // Sauvegarde de STDERR

    if (saved_stdin == -1 || saved_stdout == -1 || saved_stderr == -1)
    // si l'un des trois dup n'a pas fonctionné
    {
        fprintf(stderr, "exec_redir : dup for saving failed");
        return 2;
    }

    while (ast && ast->type >= AST_REDIR_INPUT && ast->type <= AST_REDIR_DOUBLE)
    {
        int ret;
        switch (ast->type)
        {
        case AST_REDIR_OUTPUT:
            ret = exec_redir_output(ast->value, ast->io_type);
            break;
        case AST_REDIR_INPUT:
            ret = exec_redir_input(ast->value, ast->io_type);
            break;
        case AST_REDIR_AP_OUTPUT:
            ret = exec_redir_output_append(ast->value, ast->io_type);
            break;
        case AST_REDIR_DUP_OUTPUT:
            ret = exec_redir_dup_output(ast->value, ast->io_type);
            break;
        case AST_REDIR_DUP_INPUT:
            ret = exec_redir_dup_input(ast->value, ast->io_type);
            break;
        case AST_REDIR_DOUBLE:
            ret = exec_redir_double(ast->value, ast->io_type);
            break;
        default:
            return 2;
        }
        if (ret == 2)
        {
            fprintf(stderr, "exec_redir : error in childs functions");
            return 2;
        }
        ast = ast->left;
    }

    exec_ast(ast);

    // restaurer stdin, stdout et stderr
    if (dup2(saved_stdin, STDIN_FILENO) == -1)
    {
        perror("dup2");
        exit(EXIT_FAILURE);
    }
    if (dup2(saved_stdout, STDOUT_FILENO) == -1)
    {
        perror("dup2");
        exit(EXIT_FAILURE);
    }
    if (dup2(saved_stderr, STDERR_FILENO) == -1)
    {
        perror("dup2");
        exit(EXIT_FAILURE);
    }

    close(saved_stdin); // fermeture de la sauvegarde de stdin
    close(saved_stdout); // fermeture de la sauvegarde de stdout
    close(saved_stderr); // fermeture de la sauvegarde de stderr
    return 0;
}

int detect_builtin(int argc, char **argv) // argv null terminated
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
        argv[i] = ast->value; // on prend la valeur de chaque noeud à gauche
        ast = ast->left;
    }
    return argv;
}

int exec_simple_command(struct ast *ast, int input_fd, int output_fd)
{
    int resultat = 0;
    char **argv = create_argument_list(ast);
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
        if (input_fd != -1)
        {
            close(output_fd);
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }
        if (output_fd != -1)
        {
            close(input_fd);
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }
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
        if (input_fd != -1)
            close(input_fd);
        if (output_fd != -1)
            close(output_fd);
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
        return exec_ast(ast->right);
    }
    return exec_ast(ast->left); // sinon on execute left
}

int count_command_(struct ast *ast)
{
    int count = 0;
    while (ast != NULL && ast->type == AST_PIPE)
    {
        count++;
        ast = ast->right;
    }
    return count;
}

int exec_pipe(struct ast *ast)
{
    if (ast == NULL)
        return 0;
    if (ast->right
        == NULL) // cas où on a juste une simple commande, on se fait pas chier
        return exec_ast(ast->left);
    int save_stdin = 3333; // value to save STDIN
    dup2(STDIN_FILENO, save_stdin); // saving STDIN
    int pipefds[2];
    pipe(pipefds);
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
