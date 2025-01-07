SUBDIRS = lexer

bin_PROGRAMS = 42sh

42sh_SOURCES = 42sh.c

42sh_CPPFLAGS = -I%D%

42sh_CFLAGS = -g -fsanitize=address -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

42sh_LDADD =  \
       lexer/liblexer.a