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
int is_var_index(const char *str);
char *value_of_variable(char *name);
char *get_var_at_index(int i);
char *exp_var_in_d_quote(char *buffer);

#endif /* ! VARIABLE_H */
