lib_LTLIBRARIES = libconfuse.la

libconfuse_la_SOURCES = confuse.c lexer.l
libconfuse_la_LIBADD = $(LTLIBINTL)
libconfuse_la_CPPFLAGS = -D_GNU_SOURCE
# -no-undefined is required for windows DLL support
libconfuse_la_LDFLAGS = $(AM_LDFLAGS) -no-undefined -version-info 0:0:0

include_HEADERS = confuse.h

datadir = @datadir@
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

LIBS = @LIBS@

CLEANFILES=*~ \#*\#
AM_LFLAGS=-Pcfg_yy -olex.yy.c
lexer.c: lexer.l

celan: clean
