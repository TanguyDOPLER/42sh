#include "variable.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
int variable_add(char *name, char *val)
{
    struct variable *v = shelldon.var;
    if (v == NULL)
    {
        shelldon.var = malloc(sizeof(struct variable));
        if (shelldon.var == NULL)
        {
            free(name);
            free(val);
            return 2;
        }
        shelldon.var->name = name;
        shelldon.var->value = val;
        shelldon.var->next = NULL;
        // variable_print();
        return 0;
    }
    while (v->next != NULL && strcmp(v->name, name) != 0)
        v = v->next;
    if (strcmp(v->name, name) == 0)
    {
        free(name);
        free(v->value);
        v->value = val;
    }
    else
    {
        struct variable *new_v = malloc(sizeof(struct variable));
        if (new_v == NULL)
        {
            free(name);
            free(val);
            return 2;
        }
        new_v->name = name;
        new_v->value = val;
        new_v->next = NULL;
        v->next = new_v;
    }
    // variable_print();
    return 0;
}

// free toutes les variables enregistrées
void variable_destroy(struct variable *var)
{
    while (var != NULL)
    {
        struct variable *tmp = var;
        var = var->next;
        free(tmp->name);
        free(tmp->value);
        free(tmp);
    }
}

// retourne 0 si value n'est pas une variable
// 1 si value est une variable
// 2 s'il y a une erreur
int is_variable(char **value)
{
    char *v = *value;
    int i = 0;
    int j = strlen(v) - 1;
    if (v[i] == '$')
    {
        i++;
        if (v[i] == '{' && v[j] == '}')
        {
            i++;
            j--;
        }
        for (int k = i; k <= j; k++)
        {
            if (v[k] == '{' || v[k] == '}')
            {
                return 2;
            }
        }
        char *name = malloc(sizeof(char) * (j - i + 2));
        int k = 0;
        for (; k < j - i + 1; k++)
        {
            name[k] = v[i + k];
        }
        name[k] = '\0';
        free(*value);
        *value = name;
        return 1;
    }
    else
    {
        return 0;
    }
}

// recherche la valeur d'une variable par son
// nom si elle existe, sinon renvoie une chaine vide
char *value_of_variable(char *name)
{
    struct variable *var = shelldon.var;
    while (var)
    {
        if (strcmp(name, var->name) == 0)
            return var->value;
        var = var->next;
    }
    return "";
}
