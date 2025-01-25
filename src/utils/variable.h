#ifndef VARIABLE_H
#define VARIABLE_H

struct variable
{
    char *name;
    char *value;
    struct variable *next;
};

int variable_add(char *name, char *val);
void variable_destroy(struct variable *var);
int variable_pop(char *name);
int is_variable(char **value);
char *value_of_variable(char *name);
char *get_var_at_index(int i);

#endif /* ! VARIABLE_H */
