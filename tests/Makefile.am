TESTS=check_confuse list_plus_syntax section_title_dupes
check_PROGRAMS=$(TESTS)

check_confuse_SOURCES=check_confuse.c suite_single.c suite_dup.c suite_list.c suite_validate.c \
					  suite_func.c

ldadd_common=-L../src/ ../src/libconfuse.la $(LTLIBINTL)

check_confuse_INCLUDES=-I$(top_srcdir)/src
check_confuse_LDADD=$(ldadd_common)

list_plus_syntax_SOURCES=list_plus_syntax.c
list_plus_syntax_LDADD=$(ldadd_common)

section_title_dupes_SOURCES=section_title_dupes.c
section_title_dupes_LDADD=$(ldadd_common)

EXTRA_DIST=a.conf b.conf
CLEANFILES=*~ \#*\#

if cc_is_gcc
AM_CFLAGS=-Wall -Werror
endif

celan: clean

