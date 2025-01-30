#include "variable.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "shelldon.h"

/*
static void variable_print(void)
{
    struct variable *v = shelldon.var;
    if (v == NULL)
        puts("NULL");
    while (v)
    {
        printf("%s = %s\n", v->name, v->value);
        v = v->next;
    }
    puts("----------");
}
*/

// ajoute (ou écrase si elle existe) la variable
// dans shelldon.var
int variable_add(char *name, char *val) {
  struct variable *v = shelldon.var;
  if (v == NULL) // cas où shelldon.var ne contient encore aucune variable
  {
    shelldon.var = malloc(sizeof(struct variable));
    if (shelldon.var == NULL) {
      free(name);
      free(val);
      return 2;
    }
    shelldon.var->name = name;
    shelldon.var->value = val;
    shelldon.var->next = NULL;
    return 0;
  }
  // sinon on parcours toutes les variables pour voir si celle qu'on
  // tente d'ajouter n'existe pas déjà
  while (v->next != NULL && strcmp(v->name, name) != 0)
    v = v->next;
  if (strcmp(v->name, name) == 0) // si elle existe, on change juste sa valeur
  {
    free(name);
    free(v->value);
    v->value = val;
  } else // sinon on en crée une nouvelle
  {
    struct variable *new_v = malloc(sizeof(struct variable));
    if (new_v == NULL) {
      free(name);
      free(val);
      return 2;
    }
    new_v->name = name;
    new_v->value = val;
    new_v->next = NULL;
    v->next = new_v;
  }
  return 0;
}

// free toutes les variables enregistrées
void variable_destroy(struct variable *var) {
  while (var != NULL) {
    struct variable *tmp = var;
    var = var->next;

    if (tmp->name != NULL)
      free(tmp->name);

    if (tmp->value != NULL)
      free(tmp->value);

    free(tmp);
  }
}

// delete la variable dans shelldon.var
int variable_pop(char *name) {
  struct variable *v = shelldon.var;
  if (v == NULL) // cas où shelldon.var ne contient encore aucune variable
    return 0;
  if (!strcmp(v->name, name)) // cas où c'est la premiere var qu'on veut
                              // delete
  {
    shelldon.var = v->next;
    free(v->name);
    if (v->value)
      free(v->value);
    free(v);
    return 0;
  }
  // sinon on parcours toutes les variables pour voir si celle qu'on
  // tente de delete existe bien
  struct variable *prev = v;
  v = v->next;
  while (v != NULL && strcmp(v->name, name)) {
    prev = v;
    v = v->next;
  }
  if (v != NULL) // si elle existe, on change juste sa valeur
  {
    prev->next = v->next;
    free(v->name);
    if (v->value)
      free(v->value);
    free(v);
  }
  return 0;
}

// retourne 0 si value n'est pas une variable
// 1 si value est une variable
// 2 s'il y a une erreur
int is_variable(char **value) {
  if (*value == NULL)
    return 0;
  char *v = *value;
  int i = 0;
  int j = strlen(v) - 1;
  if (v[i] == '$' && v[i + 1] != '\0') {
    i++;
    if (v[i] == '{' && v[j] == '}') {
      i++;
      j--;
    }
    for (int k = i; k <= j; k++) {
      if (v[k] == '{' || v[k] == '}') {
        return 2;
      }
    }
    char *name = malloc(sizeof(char) * (j - i + 2));
    int k = 0;
    for (; k < j - i + 1; k++) {
      name[k] = v[i + k];
    }
    name[k] = '\0';
    free(*value);
    *value = name;
    return 1;
  } else {
    return 0;
  }
}

// set la variable du pid ($$)
static void set_val_pid(void) {
  char *s_pid = calloc(12, sizeof(char));
  if (s_pid == NULL) {
    fprintf(stderr, "get_val_pid: malloc failed\n");
    return;
  }
  snprintf(s_pid, 12, "%d", getpid());
  char *cp_name = calloc(2, sizeof(char));
  if (cp_name == NULL) {
    fprintf(stderr, "get_val_pid: malloc failed\n");
    return;
  }
  strcpy(cp_name, "$");
  variable_add(cp_name, s_pid);
}

// set la variable de l'UID ($UID)
static void set_val_uid(void) {
  char *s_uid = calloc(12, sizeof(char));
  if (s_uid == NULL) {
    fprintf(stderr, "get_val_uid: malloc failed\n");
    return;
  }
  snprintf(s_uid, 12, "%d", geteuid());
  char *cp_name = calloc(4, sizeof(char));
  if (cp_name == NULL) {
    fprintf(stderr, "get_val_uid: malloc failed\n");
    return;
  }
  strcpy(cp_name, "UID");
  variable_add(cp_name, s_uid);
}

// set la variable $RANDOM
static void set_val_random(void) {
  char *s_rand = calloc(12, sizeof(char));
  if (s_rand == NULL) {
    fprintf(stderr, "get_val_uid: malloc failed\n");
    return;
  }
  int val = rand() % 32768;
  snprintf(s_rand, 12, "%d", val);
  char *cp_name = calloc(7, sizeof(char));
  if (cp_name == NULL) {
    fprintf(stderr, "get_val_uid: malloc failed\n");
    return;
  }
  strcpy(cp_name, "RANDOM");
  variable_add(cp_name, s_rand);
}

int is_var_index(const char *str) {
  int res = 0;
  while (*str) {
    if (!(*str >= '0' && *str <= '9')) {
      return 0;
    }
    res *= 10;
    res += *str - '0';
    str++;
  }
  return res;
}

// recherche la valeur d'une variable par son
// nom si elle existe, sinon renvoie une chaine vide
char *value_of_variable(char *name) {
  if (strcmp(name, "$") == 0) {
    set_val_pid();
  }
  if (strcmp(name, "UID") == 0) {
    set_val_uid();
  }
  if (strcmp(name, "RANDOM") == 0) {
    set_val_random();
  }
  struct variable *var = shelldon.var;
  while (var) {
    if (strcmp(name, var->name) == 0)
      return var->value;
    var = var->next;
  }
  char *result = getenv(name); // on va check dans les var d'environnement
  if (result) // Je sais que c'est normalement gérer de base par le shell
    // mais c'est pour le builtin export. Si vous voulez + d'explications,
    // demandez à tanguy
    return result;
  return "";
}

// permet d'obtenir l'une des variable $1...$n
char *get_var_at_index(int i) {
  if (i >= shelldon.len_list_args)
    return "";
  return shelldon.list_args[i];
}

static char *get_var_name(char *buffer, int with_acc, size_t *index_b) {
  size_t i = *index_b;
  if (with_acc) // avec accolade
  {
    if (buffer[i] >= '0' && buffer[i] <= '9') {
      while (buffer[i] >= '0' && buffer[i] <= '9') {
        i++;
      }
      if (buffer[i] != '}') {
        return NULL;
      }
    } else {
      while (buffer[i] && buffer[i] != '}' &&
             ((buffer[i] >= 'a' && buffer[i] <= 'z') ||
              (buffer[i] >= 'A' && buffer[i] <= 'Z') ||
              (buffer[i] >= '0' && buffer[i] <= '9') || buffer[i] == '_')) {
        i++;
      }
      if (buffer[i] != '}') {
        return NULL;
      }
    }
  } else // sans accolade
  {
    if (!(buffer[i] >= '0' && buffer[i] <= '9') && buffer[i] != '@' &&
        buffer[i] != '*' && buffer[i] != '#') {
      while (buffer[i] && buffer[i] != ' ' && buffer[i] != '$') {
        if (!((buffer[i] >= 'a' && buffer[i] <= 'z') ||
              (buffer[i] >= 'A' && buffer[i] <= 'Z') ||
              (buffer[i] >= '0' && buffer[i] <= '9') || buffer[i] == '_')) {
          return NULL;
        }
        i++;
      }
    } else if ((buffer[i] == '@' || buffer[i] == '*' || buffer[i] == '#') &&
               (buffer[i + 1] != ' ' && buffer[i + 1] != '\0')) {
      return NULL;
    } else {
      i++;
    }
  }
  char *name = calloc(i - *index_b + 1, 1);
  strncpy(name, buffer + *index_b, i - *index_b);
  if (with_acc)
    i++;
  *index_b = i;
  return name;
}

static char *resize_buffer(char *buffer, size_t *size, size_t new_size) {
  while (new_size >= *size - 1) {
    *size *= 2;
    char *new_buffer = realloc(buffer, *size);
    if (!new_buffer) {
      free(buffer);
      return NULL;
    }
    buffer = new_buffer;
  }
  return buffer;
}

static void copy_variable_value(char **output_buffer, size_t *index_nb,
                                size_t *size, char *var_value) {
  size_t var_len = strlen(var_value);
  *output_buffer = resize_buffer(*output_buffer, size, *index_nb + var_len);
  if (!*output_buffer)
    return;
  memcpy(*output_buffer + *index_nb, var_value, var_len);
  *index_nb += var_len;
}

static char *set_var_value(char *var_name) {
  char *var_value;
  if (strcmp(var_name, "@") == 0 || strcmp(var_name, "*") == 0)
    var_value = shelldon.str_args;
  else if (strcmp(var_name, "#") == 0)
    var_value = shelldon.len_as_str;
  else {
    int var_index = is_var_index(var_name);
    if (var_index)
      var_value = get_var_at_index(var_index);
    else
      var_value = value_of_variable(var_name);
  }
  return var_value;
}

char *exp_var_in_d_quote(char *buffer) {
  size_t size = strlen(buffer); // Taille initiale du buffer de sortie.
  char *output_buffer = calloc(size, 1);

  size_t index_nb = 0; // Indice dans le buffer de sortie.
  size_t index_b = 0;  // Indice dans le buffer d'entrée.

  while (buffer[index_b]) {
    if (buffer[index_b] == '\\')
      index_b++;

    // detection d'une variable
    if (buffer[index_b] == '$' && buffer[index_b + 1] != ' ' &&
        (index_b == 0 || (buffer[index_b - 1] != '\\'))) {
      index_b++;
      char *var_name;
      if (buffer[index_b] == '{') // Expansion avec accolades.
      {
        index_b++;
        var_name = get_var_name(buffer, 1, &index_b);
      } else // Expansion sans accolades.
      {
        var_name = get_var_name(buffer, 0, &index_b);
      }

      if (!var_name) {
        free(output_buffer);
        return NULL;
      }

      char *var_value = set_var_value(var_name);

      free(var_name);

      if (!var_value)
        continue;

      copy_variable_value(&output_buffer, &index_nb, &size, var_value);
      if (!output_buffer) {
        return NULL;
      }
    } else // Copier les caractères simplement quand c'est pas une variable
    {
      output_buffer = resize_buffer(output_buffer, &size, index_nb + 1);
      if (!output_buffer)
        return NULL;
      output_buffer[index_nb++] = buffer[index_b++];
    }
  }
  output_buffer[index_nb] = '\0';
  free(buffer);
  return output_buffer;
}
