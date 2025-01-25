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
int variable_add(char *name, char *val)
{
    struct variable *v = shelldon.var;
    if (v == NULL) // cas où shelldon.var ne contient encore aucune variable
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
    }
    else // sinon on en crée une nouvelle
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
    return 0;
}

// free toutes les variables enregistrées
void variable_destroy(struct variable *var)
{
    while (var != NULL)
    {
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
int variable_pop(char *name)
{
    struct variable *v = shelldon.var;
    if (v == NULL) // cas où shelldon.var ne contient encore aucune variable
        return 0;
    if (!strcmp(v->name, name)) // cas où c'est la premiere var qu'on veut
                                // delete
    {
        shelldon.var = v->next;
        free(v->name);
        free(v->value);
        free(v);
        return 0;
    }
    // sinon on parcours toutes les variables pour voir si celle qu'on
    // tente de delete existe bien
    struct variable *prev = v;
    v = v->next;
    while (v != NULL && strcmp(v->name, name))
    {
        prev = v;
        v = v->next;
    }
    if (v != NULL) // si elle existe, on change juste sa valeur
    {
        prev->next = v->next;
        free(v->name);
        free(v->value);
        free(v);
    }
    return 0;
}

// retourne 0 si value n'est pas une variable
// 1 si value est une variable
// 2 s'il y a une erreur
int is_variable(char **value)
{
    char *v = *value;
    int i = 0;
    int j = strlen(v) - 1;
    if (v[i] == '$' && v[i + 1] != '\0')
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

// set la variable du pid ($$)
static void set_val_pid(void)
{
    char *s_pid = calloc(12, sizeof(char));
    if (s_pid == NULL)
    {
        fprintf(stderr, "get_val_pid: malloc failed\n");
        return;
    }
    snprintf(s_pid, 12, "%d", getpid());
    char *cp_name = calloc(2, sizeof(char));
    if (cp_name == NULL)
    {
        fprintf(stderr, "get_val_pid: malloc failed\n");
        return;
    }
    strcpy(cp_name, "$");
    variable_add(cp_name, s_pid);
}

// set la variable de l'UID ($UID)
static void set_val_uid(void)
{
    char *s_uid = calloc(12, sizeof(char));
    if (s_uid == NULL)
    {
        fprintf(stderr, "get_val_uid: malloc failed\n");
        return;
    }
    snprintf(s_uid, 12, "%d", geteuid());
    char *cp_name = calloc(4, sizeof(char));
    if (cp_name == NULL)
    {
        fprintf(stderr, "get_val_uid: malloc failed\n");
        return;
    }
    strcpy(cp_name, "UID");
    variable_add(cp_name, s_uid);
}

// set la variable $RANDOM
static void set_val_random(void)
{
    char *s_rand = calloc(12, sizeof(char));
    if (s_rand == NULL)
    {
        fprintf(stderr, "get_val_uid: malloc failed\n");
        return;
    }
    int val = rand() % 32768;
    snprintf(s_rand, 12, "%d", val);
    char *cp_name = calloc(7, sizeof(char));
    if (cp_name == NULL)
    {
        fprintf(stderr, "get_val_uid: malloc failed\n");
        return;
    }
    strcpy(cp_name, "RANDOM");
    variable_add(cp_name, s_rand);
}

// recherche la valeur d'une variable par son
// nom si elle existe, sinon renvoie une chaine vide
char *value_of_variable(char *name)
{
    if (strcmp(name, "$") == 0)
    {
        set_val_pid();
    }
    if (strcmp(name, "UID") == 0)
    {
        set_val_uid();
    }
    if (strcmp(name, "RANDOM") == 0)
    {
        set_val_random();
    }
    struct variable *var = shelldon.var;
    while (var)
    {
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
char *get_var_at_index(int i)
{
    if (i >= shelldon.len_list_args)
        return "";
    return shelldon.list_args[i];
}
