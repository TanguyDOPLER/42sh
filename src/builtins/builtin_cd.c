#define _POSIX_C_SOURCE 200908L
#include <linux/limits.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "builtins.h"

char *add_slash_at_end(char *str)
{ // add a '/' at the end if there is not
    if (str == NULL)
        return NULL;
    int count = strlen(str);
    if (str[count - 1] != '/')
    {
        str = realloc(str, (strlen(str) + 2) * sizeof(char));
        str[count] = '/';
        str[count + 1] = '\0';
    }
    return str;
}

static char *canonic_shape(char *path)
{
    if (path == NULL || strlen(path) == 0)
    {
        return NULL; // Retourner NULL si le chemin est vide
    }

    // Tableau pour contenir le chemin canonique
    char *canonical = calloc(sizeof(path) + 2, sizeof(char));
    if (!canonical)
    {
        perror("malloc failed");
        return NULL;
    }
    if (path[0] == '/')
        canonical[0] = '/';

    // Temporaire pour gérer chaque composant du chemin
    char *token;
    char *path_copy = strdup(path); // Créer une copie du chemin
    char *rest = path_copy; // Pointeur pour parcourir la chaîne

    // Traiter chaque composant du chemin
    while ((token = strtok_r(rest, "/", &rest)) != NULL)
    {
        if (strcmp(token, ".") == 0 || token[0] == '\0')
        {
            // Ignorer les '.' ou les éléments vides (//)
            continue;
        }
        else if (strcmp(token, "..") == 0)
        {
            // Traiter le cas ".." en remontant d'un répertoire
            size_t len = strlen(canonical);
            if (len > 1)
            {
                // Si ce n'est pas la racine, on enlève le dernier répertoire
                // ajouté
                canonical[len - 1] = '\0';
                while (len > 1 && canonical[len - 2] != '/')
                {
                    canonical[len - 2] = '\0';
                    len--;
                }
            }
            else
            {
                // Si nous sommes déjà à la racine, ne rien faire
                continue;
            }
        }
        else
        {
            // Ajouter le composant normal au chemin
            if (strlen(canonical) > 0)
            {
                canonical = add_slash_at_end(canonical);
            }
            canonical =
                realloc(canonical,
                        (strlen(canonical) + strlen(token) + 1) * sizeof(char));
            canonical = strcat(canonical, token);
        }
    }

    // Ajouter un '/' à la fin si le chemin est un répertoire (non racine)
    if (canonical[0] != '\0' && canonical[strlen(canonical) - 1] != '/')
    {
        // On est à un répertoire normal
        if (access(canonical, F_OK) == 0)
        {
            canonical =
                realloc(canonical, (strlen(canonical) + 2) * sizeof(char));
            canonical = strcat(canonical, "/");
        }
    }

    // Nettoyer et retourner le chemin canonique
    free(path_copy);
    free(path);
    return canonical;
} // 38

char *rule_five(char *temp, char *directory_operand)
{
    char *final_path = NULL;
    struct stat path_stat;
    while (temp != NULL) // boucle while for every path of CDPATH; Rule 5
    {
        temp = add_slash_at_end(temp); // adding '/' at the end
        final_path = realloc(
            final_path,
            sizeof(temp) + sizeof(directory_operand)); // allocation de memoire
        final_path = strcat(
            temp,
            directory_operand); // concatenation CDPATH + directory_operand
        if (stat(final_path, &path_stat) == 0
            && S_ISDIR(path_stat.st_mode)) // test si c'est un répertoire valide
        {
            return final_path; // go to step 7 in this case
        }
        temp = strtok(NULL, ":"); // next pathname of CDPATH
    }
    // si on a tj rien ici, on doit tester avec "./"
    final_path = realloc(
        final_path,
        3 + strlen(directory_operand)); // allocation de memoire : "./" = 3
    final_path = strcpy(final_path, "./");
    final_path =
        strcat(final_path,
               directory_operand); // concatenation CDPATH + directory_operand
    if (stat(final_path, &path_stat) == 0
        && S_ISDIR(path_stat.st_mode)) // test si c'est un répertoire valide
    {
        return final_path; // go to step 7 in this case
    }
    free(final_path);
    return NULL;
} // 26

char *rule_nine(char *pwd_env, char *curpath)
{
    int index = 0;
    while (pwd_env[index] != '\0' && pwd_env[index] == curpath[index])
        index++;
    if (pwd_env[index] == '\0' && index > 2) // replace pwd by "./"
    {
        curpath[index - 1] = '/';
        curpath[index - 2] = '.';
        curpath = curpath + index - 2;
    }
    return curpath;
}

char *sheldon_maj(char *pwd_env, char *curpath)
{
    if (strlen(curpath) > PATH_MAX) // rule 9
    {
        curpath = rule_nine(pwd_env, curpath);
    }
    char *for_add_var = calloc(7, sizeof(char));
    for_add_var = strcat(for_add_var, "OLDPWD");
    variable_add(for_add_var, pwd_env); // mise à jour shelldone
    curpath = add_slash_at_end(curpath);
    for_add_var = calloc(4, sizeof(char));
    for_add_var = strcat(for_add_var, "PWD");
    variable_add(for_add_var, curpath); // pareil
    return curpath;
}

int if_for_rule_4(char *directory_operand)
{
    return !(directory_operand[0] == '.'
             && (directory_operand[1] == '\0'
                 || (directory_operand[1] == '.'
                     && (directory_operand[2] == '/'
                         || directory_operand[2] == '\0'))
                 || directory_operand[1] == '/'));
}

int builtin_cd(char *directory_operand)
{ // faut prendre le SCL pour comprendre les étapes ici
    char *home_env = getenv("HOME");
    char *pwd_env = add_slash_at_end(strdup(getenv("PWD")));
    char *curpath = calloc(strlen(directory_operand) + strlen(pwd_env) + 1, 1);
    if (directory_operand == NULL)
    {
        if (home_env == NULL) // rule 1: blk on dit erreur
            return 2;
        directory_operand = home_env; // rule 2
    }
    if (directory_operand[0] == '/') // rule 3
        curpath = strcpy(curpath, directory_operand); // and go rule 7
    else
    {
        if (if_for_rule_4(directory_operand)) // rule 4
        { // if pour detecter ".", "./", ".." et "../".
            // On rentre si directory_operand n'est pas un de ces cas là
            char *cdpath = getenv("CDPATH"); // value of CDPATH env var
            char *temp = NULL;
            if (cdpath != NULL)
                temp = strtok(cdpath, ":"); // stock each path of CDPATH;
            free(curpath);
            curpath = rule_five(temp, directory_operand); // rule 5
            if (curpath == NULL) // NULL si c'est pas un répertoire valide
            {
                fprintf(stderr, "cd : %s doesn't exist\n", directory_operand);
                free(pwd_env);
                return 2;
            }
        }
        else // rule 6
            curpath = strcpy(curpath, directory_operand);
    }
    if (curpath[0] != '/') // rule 7
    {
        char *temp = calloc(strlen(curpath) + strlen(pwd_env) + 1, 1);
        temp = strcpy(temp, pwd_env);
        temp = strcat(temp, curpath);
        free(curpath);
        curpath = temp;
        // memmove(curpath + strlen(pwd_env), curpath, strlen(curpath) + 1);
        // curpath = memcpy(curpath,pwd_env, strlen(pwd_env));
    }
    curpath = canonic_shape(curpath); // rule 8
    curpath = sheldon_maj(pwd_env, curpath);
    if (chdir(curpath) == -1) // change current directory
    {
        fprintf(stderr, "cd : %s doesn't exist\n", directory_operand);
        return 2;
    }
    setenv("OLDPWD", getenv("PWD"), 1); // change var OLDPWD
    setenv("PWD", curpath, 1); // change var PWD
    return 0;
} // 37
