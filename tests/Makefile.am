noinst_PROGRAMS=$(check_confuse)
EXTRA_PROGRAMS=check_confuse
check_confuse_SOURCES=check_confuse.c suite_single.c suite_dup.c suite_list.c
check_confuse_INCLUDES=-I$(top_srcdir)/src @CHECK_CFLAGS@
check_confuse_LDADD=-L../src/ @CHECK_LIBS@ ../src/.libs/libconfuse.a $(LTLIBINTL)

CLEANFILES=*~ \#*\#

