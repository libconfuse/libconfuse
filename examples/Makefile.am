EXTRA_DIST = simple.conf reread.conf wincfgtest.conf \
 wincfgtest.c wincfgtest.dev
noinst_PROGRAMS = simple reread ftpconf
INCLUDES = -I$(top_srcdir)/src
AM_LDFLAGS = -L../src/
simple_LDADD = ../src/libconfuse.la
reread_LDADD = ../src/libconfuse.la
ftpconf_LDADD = ../src/libconfuse.la
CLEANFILES=*~ \#*\#
LIBS = $(LTLIBINTL)
