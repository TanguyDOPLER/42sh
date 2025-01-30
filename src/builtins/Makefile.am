lib_LIBRARIES = libbuiltins.a

libbuiltins_a_SOURCES =  \
  builtin_echo.c \
  builtin_false.c \
  builtin_true.c \
  builtin_exit.c \
  builtin_cd.c \
  builtin_dot.c \
  builtin_unset.c \
  builtin_export.c \
  builtin_continue.c \
  builtin_break.c \
  builtins.h 

libbuiltins_a_CPPFLAGS = -I$(top_srcdir)/src

libbuiltins_a_CFLAGS = -g -O0 -fsanitize=address -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = libbuiltins.a
