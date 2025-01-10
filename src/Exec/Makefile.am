lib_LIBRARIES = libexec.a

libexec_a_SOURCES =  \
  exec.c \
  exec.h 

libexec_a_CPPFLAGS = -I$(top_srcdir)/src

libexec_a_CFLAGS = -g -O0 -fsanitize=address -std=c99 -Werror -Wall -Wextra -Wvla -pedantic

noinst_LIBRARIES = libexec.a
