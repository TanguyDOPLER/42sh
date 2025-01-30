#ifndef SHELLDON_H
#define SHELLDON_H

struct shelldon {
  struct variable *var;
  char **list_args;
  char *str_args;
  int len_list_args;
  char *len_as_str;
  struct function *function;
  struct loop_break_continue *loop_struct;
};

struct loop_break_continue {
  int nb_loop;
  int nb_continue;
  int nb_break;
};

// déclaratin de la struct globale
extern struct shelldon shelldon;

void list_args_init(int len, char **args);
void shelldon_init(void);
void shelldon_free(void);
void update_res(int res);
char *join_strs(int count, char **args);
char *itoa(int n);

#endif /* ! SHELLDON_H */
