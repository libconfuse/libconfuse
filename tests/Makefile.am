TESTS = suite_single suite_dup suite_func suite_list \
	suite_validate list_plus_syntax section_title_dupes \
	single_title_sections quote_before_print
check_PROGRAMS=$(TESTS)

LDADD=-L../src ../src/libconfuse.la $(LTLIBINTL)

EXTRA_DIST=a.conf b.conf check_confuse.h
CLEANFILES=*~

if cc_is_gcc
AM_CFLAGS=-Wall -Werror
endif

celan: clean

