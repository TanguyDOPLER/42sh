#include <stdio.h>

#include "builtins.h"

static void print_with_escapes(const char *str) {
  while (*str != '\0') {
    if (*str == '\\') {
      str++;
      switch (*str) {
      case 'n':
        putchar('\n');
        break;
      case 't':
        putchar('\t');
        break;
      case '\\':
        putchar('\\');
        break;
      default:
        putchar('\\');
        putchar(*str);
        break;
      }
    } else {
      putchar(*str);
    }
    str++;
  }
}

int is_not_option(char *argv) {
  if (!argv)
    return 1;
  for (int i = 1; argv[i] != '\0'; i++) {
    char c = argv[i];
    if (c != 'n' && c != 'e' && c != 'E')
      return 1;
  }
  return 0;
}

int builtin_echo(int argc, char *argv[]) {
  if (!argv)
    return 0;
  char print_newline = 1; // par défault, écrit un '\n'
  char interpret_escapes =
      0; // par défaut, n'interprète pas \n, \t et double backslash

  int i = 1; // indice 0 étant le nom de la commande (echo)

  while (i < argc && argv[i] != NULL && argv[i][0] == '-') {
    char *s = argv[i];
    if (is_not_option(s))
      break;
    int j = 1;
    while (s[j] != '\0') {
      if (s[j] == 'n') // option -n : empêche d'écrire un saut de ligne à
                       // la fin
      {
        print_newline = 0;
      } else if (s[j] == 'e') // option -e : interprète les
                              // \n, \t et double backslash
      {
        interpret_escapes = 1;
      } else if (s[j] == 'E') // option -E : n'interprète pas les
                              // \n, \t et double backslash
      {
        interpret_escapes = 0;
      } else {
        break; // option inconnue
      }
      j++;
    }
    i++;
  }

  // on écrit les arguments restant
  for (; i < argc; i++) {
    if (argv[i] == NULL)
      continue;
    if (interpret_escapes) {
      print_with_escapes(argv[i]);
    } else {
      fputs(argv[i], stdout);
    }
    if (i < argc - 1 && argv[i + 1] && argv[i + 1][0] != '\0') {
      putchar(' '); // on sépare chaque argument d'un espace
    }
  }

  if (print_newline)
    putchar('\n');

  fflush(stdout); // assure que la sortie est immédiatement écrite
  return 0;
}
