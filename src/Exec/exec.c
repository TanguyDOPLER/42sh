#define _POSIX_C_SOURCE 200809L

#include "exec.h"

#include <stdbool.h>
#include <string.h>

#include "utils/fonction.h"
#include "utils/shelldon.h"
#include "utils/variable.h"

static int exec_list(struct ast *ast, struct exec_status *status) {
  if (status->status != EXEC_OK)
    return 2;
  if (ast == NULL) // on fait rien si on tombe sur null
    return 0;

  int result = exec_ast(ast->left, status); // on exec le premier fils

  ast = ast->right; // on passe au deuxième fils (implémentation fils gauche
                    // frr droit)
  int temp = result;
  while (ast != NULL && ast->type == AST_LIST) {
    temp = exec_ast(ast->left, status); // on exec le noeud suivant
    if (status->status == EXEC_OK)
      result = temp;

    ast = ast->right; // on passe au frère suivant
  }
  return result; // normalement ici c'est comme return 0;
}

static int exec_and_or(
    struct ast *ast,
    struct exec_status
        *status) { // dans notre programme, 0 = true, 1 = false, 2 = erreur
  if (status->status != EXEC_OK)
    return 2;
  if (ast == NULL)
    return 0;
  int res;
  if (ast->type == AST_AND) // cas AND
  {
    if ((res = exec_ast(ast->left, status)) != 0) // left associativity
      return res;
    return exec_ast(ast->right, status);
  } else if (ast->type == AST_OR) // cas OR
  {
    if ((res = exec_ast(ast->left, status)) == 0) // left associativity
      return res;
    if (res == 2 || status->status != EXEC_OK) {
      fprintf(stderr, "exec_and_or(or), failed on execution of ast.left\n");
      return 2;
    }
    return exec_ast(ast->right, status);
  } else // WRONG CASE
  {
    fprintf(stderr, "exec_and_or, got wrong ast.type\n");
    return 2;
  }
}

static int detect_builtin(int argc, char **argv,
                          struct exec_status *status) // argv null terminated
{
  if (status->status != EXEC_OK)
    return 2;
  if (!strcmp(argv[0], "echo"))          // cas où on tombe sur le builtin echo
    return builtin_echo(argc - 1, argv); // call fonction builtin
  else if (!strcmp(argv[0], "true"))     // cas où on tombe sur le builtin true
    return builtin_true();
  else if (!strcmp(argv[0], "false")) // cas où on tombe sur le builtin false
    return builtin_false();
  else if (!strcmp(argv[0], "exit")) // cas où on tombe sur le builtin exit
    return builtin_exit(argv, status);
  else if (!strcmp(argv[0], "cd")) // cas où on tombe sur le builtin cd
    return builtin_cd(argv[1]);
  else if (!strcmp(argv[0], ".")) // cas où on tombe sur le builtin dot
    return builtin_dot(argv[1]);
  else if (!strcmp(argv[0], "unset")) // cas où on tombe sur le builtin unset
    return builtin_unset(argc, argv);
  else if (!strcmp(argv[0],
                   "export")) // cas où on tombe sur le builtin export
    return builtin_export(argv);
  else if (!strcmp(argv[0], "continue"))
    return builtin_continue(argc - 1, argv, status);
  else if (!strcmp(argv[0], "break"))
    return builtin_break(argc - 1, argv, status);
  return -3; // on a pas détecter de builtin, donc on continu sur execvp
}

static int count_arguments(struct ast *ast, struct exec_status *status) {
  if (status->status != EXEC_OK)
    return 2;
  int count = 1; // Initialisé à 1 pour inclure le NULL final
  while (ast)    // on compte le nb d'arguments y compris la commande. Cf. man
                 // page execvp(3)
  {
    count++;
    ast = ast->left;
  }
  return count;
}

static char **create_argument_list(struct ast *ast,
                                   struct exec_status *status) {
  int number_argument = count_arguments(ast, status);

  char **argv =
      calloc(number_argument,
             sizeof(char *)); // initialisation du tableau de char * pour execvp
  for (int i = 0; i + 1 < number_argument; i++) // on rempli le tableau
  {
    if (!ast->value) {
      free(argv);
      fprintf(stderr, "bad substitution in double quote\n");
      return NULL;
    }
    if (ast->is_d_quote) // cas d'une double quote
    {
      ast->value = exp_var_in_d_quote(ast->value);
      argv[i] = ast->value;
    } else {
      if (strcmp("$@", ast->value) == 0 || strcmp("$*", ast->value) == 0) {
        argv[i] = shelldon.str_args;
      } else if (strcmp("$#", ast->value) == 0) {
        argv[i] = shelldon.len_as_str;
      } else {
        char *cp_ast_value = strdup(ast->value);
        int is_var = is_variable(&cp_ast_value);
        if (is_var == 0) // pas une variable
        {
          argv[i] = ast->value; // on prend la valeur de chaque noeud
                                // à gauche
        } else if (is_var == 1) // une variable
        {
          int index = is_var_index(cp_ast_value);
          if (index)
            argv[i] = get_var_at_index(index); // pour les variables $1...$n
          else {
            argv[i] =
                value_of_variable(cp_ast_value); // pour les autre variables
          }
        } else // erreur
        {
          free(argv);
          fprintf(stderr, "%s: bad substitution\n", ast->value);
          free(cp_ast_value);
          return NULL;
        }
        free(cp_ast_value);
      }
    }
    ast = ast->left;
  }
  return argv;
} // 38

static int exec_fun(struct ast *ast, char **args, struct exec_status *status) {
  char **save_list_args = shelldon.list_args;
  int save_len_list_args = shelldon.len_list_args;
  char *save_str_args = shelldon.str_args;
  char *save_len_as_str = shelldon.len_as_str;

  shelldon.list_args = args;
  shelldon.len_list_args = 0;
  for (int i = 0; shelldon.list_args[i]; i++)
    shelldon.len_list_args++;
  shelldon.str_args = join_strs(shelldon.len_list_args - 1, args + 1);
  shelldon.len_as_str = itoa(shelldon.len_list_args - 1);

  int res = exec_ast(ast, status);

  free(shelldon.str_args);
  free(shelldon.len_as_str);

  shelldon.list_args = save_list_args;
  shelldon.len_list_args = save_len_list_args;
  shelldon.str_args = save_str_args;
  shelldon.len_as_str = save_len_as_str;

  free(args);
  return res;
}

static int exec_simple_command(struct ast *ast, struct exec_status *status) {
  if (status->status != EXEC_OK)
    return 2;

  int resultat = 0;
  char **argv = create_argument_list(ast, status);
  struct ast *is_func = is_function(argv[0]);
  if (is_func) {
    int res = exec_fun(is_func, argv, status);
    return res;
  }

  if (argv == NULL || status->status != EXEC_OK) {
    update_res(2);
    return 2;
  }
  resultat = detect_builtin(count_arguments(ast, status), argv,
                            status); // on detecte si on est sur un builtins
  if (resultat != -3)                // cas où on est sur un builtin
  {
    free(argv); // free le tableau completement
    update_res(resultat);
    return resultat; // free result builtins
  }
  int id = fork(); // fork pour execvp
  if (id == 0)     // enfant
  {
    if (execvp(argv[0], argv) == -1) // on execute la commande
    {
      fprintf(stderr, "non existing command\n"); // ca se passe mal
      exit(127); // jsp pq 127 mais ca vient de 21sh donc ca marche
    }
    return 0; // ca se passe bien
  } else      // parent
  {
    int status;
    waitpid(id, &status, 0); // on attend l'enfant
    int exit_stat;
    exit_stat = WEXITSTATUS(status); // on récupère le résultat de l'enfant
    free(argv);                      // free le tableau completement
    if (exit_stat == 127)            // on gère en fonction
    {
      fprintf(stderr,
              "failed on exec simple command. returned result from child "
              "process is 127\n");
      return 127;
    }
    update_res(exit_stat);
    return exit_stat; // a changer ici prcq ca marche pas
  }
} // 36

static int continue_break(struct exec_status *status) {
  if (shelldon.loop_struct->nb_continue) {
    shelldon.loop_struct->nb_continue--;
    status->status = EXEC_OK;
    if (shelldon.loop_struct->nb_loop == 1) {
      shelldon.loop_struct->nb_continue = 0;
      return 0;
    }
    if (shelldon.loop_struct->nb_loop > 1 &&
        shelldon.loop_struct->nb_continue > 0) {
      return 1;
    }
  }
  if (shelldon.loop_struct->nb_break) {
    shelldon.loop_struct->nb_break--;
    status->status = EXEC_OK;
    return 1;
  }
  return 0;
}

static int exec_while_until(struct ast *ast, struct exec_status *status) {
  if (shelldon.loop_struct->nb_loop == 0) {
    shelldon.loop_struct->nb_continue = 0;
    shelldon.loop_struct->nb_break = 0;
  }
  if (status->status != EXEC_OK)
    return 2;
  int res = 0;
  if (ast->type == AST_WHILE) {
    shelldon.loop_struct->nb_loop++;
    while (exec_ast(ast->condition, status) == 0) {
      if (continue_break(status)) {
        break;
      }
      res = exec_ast(ast->left, status);
    }
  } else if (ast->type == AST_UNTIL) {
    shelldon.loop_struct->nb_loop++;
    while (exec_ast(ast->condition, status) != 0) {
      if (continue_break(status)) {
        break;
      }
      // il se passe quoi si la condition crach?
      res = exec_ast(ast->left, status);
    }
  } else {
    return 2;
  }
  shelldon.loop_struct->nb_loop--;
  return res;
}

static int exec_for(struct ast *ast, struct exec_status *status) {
  if (shelldon.loop_struct->nb_loop == 0) {
    shelldon.loop_struct->nb_continue = 0;
    shelldon.loop_struct->nb_break = 0;
  }
  int res = 0;
  char *var_name;
  char *var_value;
  shelldon.loop_struct->nb_loop++;
  if (ast->len_values) {
    for (int i = 0; i < ast->len_values; i++) {
      if (continue_break(status)) {
        break;
      }
      var_name = strdup(ast->value);
      var_value = strdup(ast->for_values[i]);
      variable_add(var_name, var_value);
      res = exec_ast(ast->left, status);
    }
  } else {
    for (int i = 0; i < shelldon.len_list_args; i++) {
      if (continue_break(status)) {
        break;
      }
      var_name = strdup(ast->value);
      var_value = strdup(shelldon.list_args[i]);
      variable_add(var_name, var_value);
      res = exec_ast(ast->left, status);
    }
  }
  shelldon.loop_struct->nb_loop--;
  return res;
}

static int exec_shell_command(struct ast *ast, struct exec_status *status) {
  if (status->status != EXEC_OK)
    return 2;
  int condition = exec_ast(ast->condition, status); // on évalue la condition
  if (condition == 2 || status->status != EXEC_OK)  // gestion erreur
  {
    fprintf(stderr, "failed on exec shell command condition\n");
    update_res(2);
    return 2;
  }
  if (condition) // on execute right si la condition est à 1 (fausse)
  {
    int res = exec_ast(ast->right, status);
    update_res(res);
    return res;
  }
  int res = exec_ast(ast->left, status); // sinon on execute left
  update_res(res);
  return res;
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

static int exec_pipe(struct ast *ast, struct exec_status *status) {
  if (status->status != EXEC_OK)
    return 2;
  if (ast == NULL)
    return 0;
  if (ast->right ==
      NULL) // cas où on a juste une simple commande, on se fait pas chier
    return exec_ast(ast->left, status);
  int save_stdin = 3333;          // value to save STDIN
  dup2(STDIN_FILENO, save_stdin); // saving STDIN
  int pipefds[2];
  if (pipe(pipefds) == -1 || status->status != EXEC_OK) {
    return 2;
  }
  int id = fork(); // FIRST commande (left one)
  if (id == 0)     // child process
  {
    close(pipefds[0]); // Cf vide conf redirection 2019
    dup2(pipefds[1], STDOUT_FILENO);
    close(pipefds[1]);
    exit(exec_ast(ast->left, status));
  } else // parent process
  {
    close(pipefds[1]);
    waitpid(id, NULL, 0);
  }
  int id2 = fork(); // seoncd commande (right one)
  if (id2 == 0) {
    close(pipefds[1]); // Cf vide conf redirection 2019
    dup2(pipefds[0], STDIN_FILENO);
    close(pipefds[0]);
    exit(exec_ast(ast->right, status));
  } else {
    int status2;
    close(pipefds[0]);
    close(pipefds[1]);
    waitpid(id2, &status2, 0);
    int exit_stat2;
    exit_stat2 = WEXITSTATUS(status2);
    dup2(save_stdin, STDIN_FILENO);
    return exit_stat2;
  }
} // 37

static int is_variable_name_ok(char *name) {
  if ((name[0] >= 'a' && name[0] <= 'z') ||
      (name[0] >= 'A' && name[0] <= 'Z') || name[0] == '_') {
    for (int i = 1; name[i] != '\0'; i++) {
      if (!((name[i] >= 'a' && name[i] <= 'z') ||
            (name[i] >= 'A' && name[i] <= 'Z') ||
            (name[i] >= '0' && name[i] <= '9') || (name[i] == '_')))
        return 0;
    }
  } else {
    return 0;
  }
  return 1;
}

// permet d'enregistrer la variable
int exec_assign_var(struct ast *ast, struct exec_status *status) {
  if (status->status != EXEC_OK)
    return 2;
  char *name = strdup(ast->value); // copie le nom de la variable
  if (!is_variable_name_ok(name)) {
    free(name);
    fprintf(stderr, "bad variable assignment\n");
    return 127;
  }
  if (ast->left && ast->left->type != AST_SIMPLE_COMMAND) {
    free(name);
    return 2;
  }
  char *val;
  if (ast->left)
    val = strdup(ast->left->value);
  else
    val = NULL;
  int is_var = is_variable(&val);
  if (is_var == 0) // pas une variable
  {
    variable_add(name, val);
  } else if (is_var == 1) // une variable
  {
    int index = is_var_index(val);
    if (index) {
      free(val);
      val = get_var_at_index(index);
      // pour les variables $1...$n
      variable_add(name, val);
    } else {
      char *v = strdup(value_of_variable(val));
      // pour les autre variables
      variable_add(name, v);
      free(val);
    }
  } else // erreur
  {
    fprintf(stderr, "%s: bad substitution\n", val);
    free(val);
    return 2;
  }
  return 0;
}

// permet d'enregistrer une fonction
int exec_funcdec(struct ast *ast, struct exec_status *status) {
  if (status->status != EXEC_OK)
    return 2;
  char *name = strdup(ast->value); // copie le nom de la fonction
  function_add(name,
               ast->left); // enregistrement de la fonction dans shelldon ;)
  return 0;
}

int exec_ast(
    struct ast *ast,
    struct exec_status *status) { // on étudie le type du premier noeud de l'ast
  if (status->status == EXEC_EXIT)
    return 0;
  if (ast == NULL)
    return 0;
  if (ast->type >= AST_REDIR_INPUT && ast->type <= AST_REDIR_DOUBLE)
    return exec_redir(ast, status);
  switch (ast->type) {
  case AST_LIST:
    return exec_list(ast, status);
  case AST_AND:
  case AST_OR:
    return exec_and_or(ast, status);
  case AST_SIMPLE_COMMAND:
    return exec_simple_command(ast, status);
  case AST_SHELL_COMMAND:
    return exec_shell_command(ast, status);
  case AST_PIPE:
    return exec_pipe(ast, status);
  case AST_NEGATION:
    return (!exec_ast(ast->left, status));
  case AST_UNTIL:
  case AST_WHILE:
    return exec_while_until(ast, status);
  case AST_FOR:
    return exec_for(ast, status);
  case AST_ASSIGNMENT_WORD:
    return exec_assign_var(ast, status);
  case AST_FUNCTION:
    return exec_funcdec(ast, status);
  case AST_ARGUMENTS:
    fprintf(stderr, "got AST_ARGUMENTS nodes in fonction exec_ast\n");
    return 2; // return 2 = soucis dans le code
  default:
    fprintf(stderr, "got UNKNOWN nodes in fonction exec_ast\n");
    return 2;
  }
}
