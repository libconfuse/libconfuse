EXTRA_DIST      = simple.conf reread.conf ftp.conf test.conf
noinst_PROGRAMS = simple reread ftpconf cfgtest
AM_CPPFLAGS     = -I$(top_srcdir)/src
AM_LDFLAGS      = -L../src/
LIBS            = $(LTLIBINTL)
LDADD           = ../src/libconfuse.la
CLEANFILES      = *~ \#*\#
