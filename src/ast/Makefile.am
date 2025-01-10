lib_LIBRARIES = libast.a

libast_a_SOURCES =  \
  ast.c \
  ast.h 

libast_a_CPPFLAGS = -I$(top_srcdir)/src

libast_a_CFLAGS = -g -O0 -fsanitize=address -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = libast.a
