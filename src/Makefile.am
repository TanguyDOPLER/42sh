SUBDIRS = ast \
       lexer \
       parser \
	   builtins \
	   Exec \
       utils

bin_PROGRAMS = 42sh
42sh_SOURCES = main.c
42sh_CPPFLAGS = -I%D%
42sh_CFLAGS = -g -O0 -fsanitize=address -std=c99 -Werror -Wall -Wextra -Wvla -pedantic
42sh_LDADD =  \
       lexer/liblexer.a \
       ast/libast.a \
       parser/libparser.a \
	   Exec/libexec.a \
	   builtins/libbuiltins.a \
       utils/libutils.a

check-local:
	./../tests/test_suit.sh





