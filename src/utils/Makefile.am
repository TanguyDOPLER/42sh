lib_LIBRARIES = libutils.a

libutils_a_SOURCES =  \
  variable.c \
  variable.h \
  shelldon.c \
  shelldon.h \
  function.c \
  function.h

libutils_a_CPPFLAGS = -I$(top_srcdir)/src

libutils_a_CFLAGS =-O0 -g -fsanitize=address -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = libutils.a
