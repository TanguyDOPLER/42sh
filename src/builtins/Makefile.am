lib_LIBRARIES = libbuiltins.a

libbuiltins_a_SOURCES =  \
  builtin_echo.c \
  builtin_false.c \
  builtin_true.c \
  builtins.h 

libbuiltins_a_CPPFLAGS = -I$(top_srcdir)/src

libbuiltins_a_CFLAGS = -g -fsanitize=address -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = libbuiltins.a
