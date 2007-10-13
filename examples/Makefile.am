EXTRA_DIST = simple.conf reread.conf wincfgtest.conf \
             wincfgtest.c wincfgtest.dev ftp.conf
noinst_PROGRAMS = simple reread ftpconf
INCLUDES = -I$(top_srcdir)/src
AM_LDFLAGS = -L../src/
LDADD = ../src/libconfuse.la
CLEANFILES=*~ \#*\#
LIBS = $(LTLIBINTL)
