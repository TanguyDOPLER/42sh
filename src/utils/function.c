#include "function.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "shelldon.h"

// ajoute (ou écrase si elle existe) la fonction
// dans shelldon.fonction
int function_add(char *name, struct ast *ast)
{
    struct function *f = shelldon.function;
    // cas où shelldon.function ne contient encore aucune variable
    if (f == NULL)
    {
        shelldon.function = malloc(sizeof(struct function));
        if (shelldon.function == NULL)
        {
            free(name);
            ast_free(ast);
            return 2;
        }
        shelldon.function->name = name;
        shelldon.function->ast = ast_copy(ast);
        shelldon.function->next = NULL;
        return 0;
    }
    // sinon on parcours toutes les fonctions pour voir si celle qu'on
    // tente d'ajouter n'existe pas déjà
    while (f->next != NULL && strcmp(f->name, name) != 0)
        f = f->next;
    // si elle existe, on change juste sa définition
    if (strcmp(f->name, name) == 0)
    {
        free(name);
        ast_free(f->ast);
        f->ast = ast_copy(ast);
    }
    else // sinon on en crée une nouvelle
    {
        struct function *new_f = malloc(sizeof(struct function));
        if (new_f == NULL)
        {
            free(name);
            ast_free(ast);
            return 2;
        }
        new_f->name = name;
        new_f->ast = ast_copy(ast);
        new_f->next = NULL;
        f->next = new_f;
    }
    return 0;
}

// free toutes les variables enregistrées
void function_destroy(struct function *function)
{
    while (function != NULL)
    {
        struct function *tmp = function;
        function = function->next;

        if (tmp->name != NULL)
            free(tmp->name);

        if (tmp->ast != NULL)
            ast_free(tmp->ast);

        free(tmp);
    }
}

// retourne NULL si name n'est pas un nom de fonction
// ou un ast non null si elle existe
struct ast *is_function(char *name)
{
    for (struct function *f = shelldon.function; f != NULL; f = f->next)
    {
        if (strcmp(f->name, name) == 0)
            return f->ast;
    }
    return NULL;
}
