lib_LIBRARIES = liblexer.a

liblexer_a_SOURCES =  \
  lexer.c \
  lexer.h \
  token.h 

liblexer_a_CPPFLAGS = -I$(top_srcdir)/src

liblexer_a_CFLAGS =-O0 -g -fsanitize=address -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = liblexer.a
