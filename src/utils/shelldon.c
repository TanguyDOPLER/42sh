#include "shelldon.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "fonction.h"
#include "variable.h"

// renvoie le chemin du dossier dans lequel on se trouve actuellement
static char *get_current_directory(void) {
  size_t size = 128;
  char *buffer = malloc(sizeof(char) * size);
  if (buffer == NULL) {
    fprintf(stderr, "get_current_directory : malloc failed\n");
    return NULL;
  }
  if (getcwd(buffer, size) != NULL) {
    return buffer;
  }

  fprintf(stderr, "get_current_directory : getcwd failed\n");
  free(buffer);
  return NULL;
}

// met à jour $?
void update_res(int res) {
  char *value = calloc(8, sizeof(char));

  snprintf(value, 8, "%d", res);
  char *name = calloc(2, sizeof(char));
  strcpy(name, "?");
  variable_add(name, value);
}

void special_var_init(void) {
  // utile pour la variable $RANDOM
  srand(time(NULL));

  char *value;
  char *name;

  // initialisation de $?
  value = calloc(2, sizeof(char));
  value[0] = '0';
  name = calloc(2, sizeof(char));
  strcpy(name, "?");
  variable_add(name, value);

  // initialisation de $PWD
  value = get_current_directory();
  name = calloc(4, sizeof(char));
  strcpy(name, "PWD");
  variable_add(name, value);

  // initialisation de $OLDPWD
  value = calloc(1, sizeof(char));
  name = calloc(7, sizeof(char));
  strcpy(name, "OLDPWD");
  variable_add(name, value);

  // initialisation de $IFS
  value = calloc(4, sizeof(char));
  name = calloc(4, sizeof(char));
  strcpy(name, "IFS");
  strcpy(value, " \t\n");
  variable_add(name, value);
}

char *join_strs(int count, char **args) {
  int total_len = 0;
  for (int i = 0; i < count; i++) {
    total_len += strlen(args[i]);
    if (i < count - 1)
      total_len++;
  }

  char *result = malloc(total_len + 1);
  if (result == NULL) {
    fprintf(stderr, "Erreur malloc");
    exit(2);
  }

  result[0] = '\0';
  for (int i = 0; i < count; i++) {
    strcat(result, args[i]);
    if (i < count - 1)
      strcat(result, " ");
  }

  return result;
}

char *itoa(int n) {
  if (n == 0) {
    char *res = calloc(2, 1);
    res[0] = '0';
    return res;
  }
  int tmp = n;
  int len = 0;
  while (tmp) {
    len++;
    tmp /= 10;
  }

  char *res = calloc(len + 1, 1);

  tmp = n;

  for (int i = len - 1; i >= 0; i--) {
    res[i] = (tmp % 10) + '0';
    tmp /= 10;
  }

  return res;
}

void list_args_init(int len, char **args) {
  if (len == 0) {
    shelldon.list_args = NULL;
    shelldon.str_args = NULL;
    shelldon.len_list_args = 0;
    shelldon.len_as_str = calloc(2, 1);
    shelldon.len_as_str[0] = '0';
  } else {
    shelldon.list_args = args;
    shelldon.str_args = NULL;
    if (len - 1 > 0)
      shelldon.str_args = join_strs(len - 1, args + 1);
    shelldon.len_list_args = len;
    shelldon.len_as_str = itoa(len - 1);
  }
}

// initialise shelldon
void shelldon_init(void) {
  shelldon.var = NULL; // contient les variables (spéciales inclues, sauf $@)
  special_var_init();  // initialisation des variables spéciales
  shelldon.function = NULL;
  free(shelldon.str_args);
  shelldon.loop_struct = malloc(sizeof(struct loop_break_continue));
  shelldon.loop_struct->nb_loop = 0;
  shelldon.loop_struct->nb_continue = 0;
  shelldon.loop_struct->nb_break = 0;
}

// free shelldon (shelldon au cimetière *snif*)
void shelldon_free(void) {
  variable_destroy(shelldon.var);
  function_destroy(shelldon.function);
  if (shelldon.loop_struct != NULL) {
    free(shelldon.loop_struct);
  }
}
