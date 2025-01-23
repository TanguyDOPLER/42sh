#include <string.h>
#include <unistd.h>

#include "builtins.h"

int builtin_dot(char *path)
{
    size_t i = 0;
    while (path[i] != '\0' && path[i] != '/') // on test si '/' est dans path
        i++;
    if (i < strlen(path))
        return exec_for_builtin_dot(path); // si oui on fait simple
    // on execute le fichier en considerant path comme juste

    char *temp_path = strtok(getenv("PATH"), ":");
    // sinon on va chercher dans la variable path
    char *final_path =
        calloc(strlen(temp_path) + strlen(path) + 2, sizeof(char));
    final_path = strcpy(final_path, temp_path);
    final_path = strcat(final_path, "/");
    final_path = strcat(final_path, path);
    // on fusionne $PATH et path pour former un chemin
    while (final_path != NULL)
    {
        if (access(final_path, F_OK) == 0) // on teste si c un fichier valide
            return exec_for_builtin_dot(final_path); // si oui on execute
        temp_path = strtok(NULL, ":");
        final_path = realloc(final_path,
                             (strlen(temp_path) + strlen(path)) * sizeof(char));
        final_path = strcpy(final_path, temp_path);
        // sinon on passe au path suivant
        final_path = strcat(final_path, "/");
        final_path = strcat(final_path, path);
    }
    fprintf(stderr, "dot : No file found");
    // on est dans le cas où on a rien trouver ici
    return 0;
}
