lib_LTLIBRARIES = libconfuse.la

libconfuse_la_SOURCES = confuse.c lexer.l
include_HEADERS = confuse.h

datadir = @datadir@
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

LIBS = @LIBINTL@ @LIBS@

celan: clean
