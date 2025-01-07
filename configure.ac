AC_INIT([42sh], [1.0], [mehdi.denguir@epita.fr])
AM_INIT_AUTOMAKE([subdir-objects] [foreign])
AM_SILENT_RULES([yes])
AM_PROG_AR
AC_PROG_RANLIB
AC_PROG_CC
AX_COMPILER_FLAGS([], [], [], [-g -fsanitize -Wall -Wextra -Werror -Wvla -pedantic -std=c99])
AC_CONFIG_FILES([
                    Makefile
                    src/Makefile
                    src/lexer/Makefile
                ])
AC_OUTPUT