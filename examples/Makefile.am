EXTRA_DIST = test.conf simple.conf reread.conf wincfgtest.conf \
 wincfgtest.c wincfgtest.dev inc.conf
noinst_PROGRAMS = cfgtest simple reread
INCLUDES = -I$(top_srcdir)/src
AM_LDFLAGS = -L../src/
LDADD = -lconfuse
CLEANFILES=*~ \#*\#
LIBS = $(LTLIBINTL)
