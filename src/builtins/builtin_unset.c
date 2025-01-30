#include <stdio.h>
#include <string.h>

#include "builtins.h"

int builtin_unset(int argc, char **argv) {
  if (argc == 3) // "unset" + "name of variable" + "null"
    variable_pop(argv[1]);
  else {
    int i = 1;
    if (argv[1][0] == '-') {
      if (argv[1][1] != 'v' && argv[1][1] != 'f') {
        fprintf(stderr, "builtin_unset : wrong option\n");
        return 2;
      }
      i++;
    }
    for (; i < argc - 1; i++) {
      variable_pop(argv[i]);
    }
  }
  return 0;
}
