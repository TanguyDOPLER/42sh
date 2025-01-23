#include "exec.h"

static int exec_redir_output(char *filename, int io, struct exec_status *status)
{
    if (status->status != EXEC_OK)
        return 2;
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
        fprintf(stderr, "exec_redir_output : open fd failed\n");
        return 2;
    }
    if (io == -1)
        io = 1;
    if (dup2(fd, io) == -1)
    {
        fprintf(stderr, "exec_redir_output : dup2 failed\n");
        close(fd);
        return 2;
    }

    close(fd);
    return 0;
}

static int exec_redir_input(char *filename, int io, struct exec_status *status)
{
    if (status->status != EXEC_OK)
        return 2;
    int fd = open(filename, O_RDONLY, 0644);
    if (fd == -1)
    {
        fprintf(stderr, "exec_redir_input : open fd failed\n");
        return 2;
    }
    if (io == -1)
        io = 0;
    if (dup2(fd, io) == -1)
    {
        fprintf(stderr, "exec_redir_input : dup2 failed\n");
        close(fd);
        return 2;
    }

    close(fd);
    return 0;
}

static int exec_redir_output_append(char *filename, int io,
                                    struct exec_status *status)
{
    if (status->status != EXEC_OK)
        return 2;
    int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1)
    {
        fprintf(stderr, "exec_redir_output_append : open fd failed\n");
        return 2;
    }
    if (io == -1)
        io = 1;
    if (dup2(fd, io) == -1)
    {
        fprintf(stderr, "exec_redir_output_append : dup2 failed\n");
        close(fd);
        return 2;
    }

    close(fd);
    return 0;
}

static int exec_redir_dup_output(char *filename, int io,
                                 struct exec_status *status)
{
    if (status->status != EXEC_OK)
        return 2;
    int fd = atoi(filename);
    if (fd == -1)
    {
        fprintf(stderr, "exec_redir_dup_output : open fd failed\n");
        return 2;
    }
    if (io == -1)
        io = 1;
    if (dup2(fd, io) == -1)
    {
        fprintf(stderr, "exec_redir_dup_output : dup2 failed\n");
        close(fd);
        return 2;
    }

    close(fd);
    return 0;
}

static int exec_redir_dup_input(char *filename, int io,
                                struct exec_status *status)
{
    if (status->status != EXEC_OK)
        return 2;
    int fd = atoi(filename);
    if (fd == -1)
    {
        fprintf(stderr, "exec_redir_dup_input : open fd failed\n");
        return 2;
    }
    if (io == -1)
        io = 0;
    if (dup2(fd, io) == -1)
    {
        fprintf(stderr, "exec_redir_dup_input : dup2 failed\n");
        close(fd);
        return 2;
    }

    close(fd);
    return 0;
}

static int exec_redir_double(char *filename, int io, struct exec_status *status)
{
    if (status->status != EXEC_OK)
        return 2;
    int fd_out = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_out == -1)
    {
        fprintf(stderr, "exec_redir_both : open fd failed\n");
        return 2;
    }
    int fd_in = open(filename, O_RDONLY, 0644);
    if (fd_in == -1)
    {
        fprintf(stderr, "exec_redir_both : open fd failed\n");
        return 2;
    }
    if (io == -1)
        io = 0;
    if (dup2(fd_out, io) == -1)
    {
        fprintf(stderr, "exec_redir_both : dup2 failed\n");
        close(fd_out);
        return 2;
    }
    if (dup2(fd_in, io) == -1)
    {
        fprintf(stderr, "exec_redir_both : dup2 failed\n");
        close(fd_in);
        return 2;
    }

    close(fd_out);
    close(fd_in);
    return 0;
}

void end_exec_redir(int saved_stdin, int saved_stdout, int saved_stderr,
                    struct exec_status *status)
{
    if (status->status != EXEC_OK)
        return;
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
}

int exec_redir(struct ast *ast, struct exec_status *status)
{
    if (status->status != EXEC_OK)
        return 2;
    int saved_stdin = dup(STDIN_FILENO); // Sauvegarde de STDIN
    int saved_stdout = dup(STDOUT_FILENO); // Sauvegarde de STDOUT
    int saved_stderr = dup(STDERR_FILENO); // Sauvegarde de STDERR

    if (saved_stdin == -1 || saved_stdout == -1 || saved_stderr == -1)
    // si l'un des trois dup n'a pas fonctionné
    {
        fprintf(stderr, "exec_redir : dup for saving failed");
        return 1;
    }

    while (ast && ast->type >= AST_REDIR_INPUT && ast->type <= AST_REDIR_DOUBLE)
    {
        int ret;
        switch (ast->type)
        {
        case AST_REDIR_OUTPUT:
            ret = exec_redir_output(ast->value, ast->io_type, status);
            break;
        case AST_REDIR_INPUT:
            ret = exec_redir_input(ast->value, ast->io_type, status);
            break;
        case AST_REDIR_AP_OUTPUT:
            ret = exec_redir_output_append(ast->value, ast->io_type, status);
            break;
        case AST_REDIR_DUP_OUTPUT:
            ret = exec_redir_dup_output(ast->value, ast->io_type, status);
            break;
        case AST_REDIR_DUP_INPUT:
            ret = exec_redir_dup_input(ast->value, ast->io_type, status);
            break;
        case AST_REDIR_DOUBLE:
            ret = exec_redir_double(ast->value, ast->io_type, status);
            break;
        default:
            return 1;
        }
        if (ret == 2)
        {
            fprintf(stderr, "exec_redir : error in childs functions\n");
            return 1;
        }
        ast = ast->left;
    }

    exec_ast(ast, status);
    end_exec_redir(saved_stdin, saved_stdout, saved_stderr, status);
    return 0;
}
