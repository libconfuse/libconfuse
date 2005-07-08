TESTS=check_confuse
check_PROGRAMS=check_confuse

check_confuse_SOURCES=check_confuse.c suite_single.c suite_dup.c suite_list.c suite_validate.c \
					  suite_func.c
check_confuse_INCLUDES=-I$(top_srcdir)/src
check_confuse_LDADD=-L../src/ ../src/libconfuse.la $(LTLIBINTL)

EXTRA_DIST=a.conf b.conf
CLEANFILES=*~ \#*\#

if cc_is_gcc
AM_CFLAGS=-Wall -Werror
endif

celan: clean

