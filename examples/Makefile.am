EXTRA_DIST = test.conf simple.conf reread.conf wincfgtest.conf \
 wincfgtest.c wincfgtest.dev inc.conf
noinst_PROGRAMS = cfgtest simple reread
INCLUDES = -I$(top_srcdir)/src
AM_LDFLAGS = -L../src/
cfgtest_LDADD = ../src/libconfuse.la
simple_LDADD = ../src/libconfuse.la
reread_LDADD = ../src/libconfuse.la
CLEANFILES=*~ \#*\#
LIBS = $(LTLIBINTL)
