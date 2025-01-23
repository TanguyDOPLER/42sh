lib_LIBRARIES = libparser.a

libparser_a_SOURCES =  \
  parser.c \
  pars_command.c \
  pars_elem.c \
  pars_ope.c \
  pars_redir.c \
  pars_shell_com.c \
  parser.h

libparser_a_CPPFLAGS = -I$(top_srcdir)/src

libparser_a_CFLAGS = -g -O0 -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = libparser.a
