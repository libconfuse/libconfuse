TESTS = suite_single suite_dup suite_func suite_list \
	suite_validate list_plus_syntax section_title_dupes \
	single_title_sections quote_before_print
check_PROGRAMS=$(TESTS)

ldadd_common=-L../src/ ../src/libconfuse.la $(LTLIBINTL)

suite_single_SOURCES=suite_single.c
suite_single_LDADD=$(ldadd_common)

suite_dup_SOURCES=suite_dup.c
suite_dup_LDADD=$(ldadd_common)

suite_list_SOURCES=suite_list.c
suite_list_LDADD=$(ldadd_common)

suite_func_SOURCES=suite_func.c
suite_func_LDADD=$(ldadd_common)

suite_validate_SOURCES=suite_validate.c
suite_validate_LDADD=$(ldadd_common)

list_plus_syntax_SOURCES=list_plus_syntax.c
list_plus_syntax_LDADD=$(ldadd_common)

section_title_dupes_SOURCES=section_title_dupes.c
section_title_dupes_LDADD=$(ldadd_common)

single_title_sections_SOURCES=single_title_sections.c
single_title_sections_LDADD=$(ldadd_common)

quote_before_print_SOURCES=quote_before_print.c
quote_before_print_LDADD=$(ldadd_common)

EXTRA_DIST=a.conf b.conf
CLEANFILES=*~ \#*\#

if cc_is_gcc
AM_CFLAGS=-Wall -Werror
endif

celan: clean

