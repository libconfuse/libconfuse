EXTRA_DIST = simple.conf reread.conf ftp.conf test.conf
noinst_PROGRAMS = simple reread ftpconf cfgtest
INCLUDES = -I$(top_srcdir)/src
AM_LDFLAGS = -L../src/
LDADD = ../src/libconfuse.la
CLEANFILES=*~ \#*\#
LIBS = $(LTLIBINTL)
