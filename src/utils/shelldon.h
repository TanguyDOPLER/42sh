#ifndef SHELLDON_H
#define SHELLDON_H

struct shelldon
{
    struct variable *var;
    char **list_args;
    int len_list_args;
    struct function *function;
};

// déclaratin de la struct globale
extern struct shelldon shelldon;

void shelldon_init(void);
void shelldon_free(void);
void update_res(int res);

#endif /* ! SHELLDON_H */
