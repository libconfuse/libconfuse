EXTRA_DIST        = a.conf b.conf spdir check_confuse.h

TESTS             =
TESTS            += suite_single
TESTS            += suite_dup
TESTS            += suite_func
TESTS            += suite_list
TESTS            += suite_validate
TESTS            += list_plus_syntax
TESTS            += section_title_dupes
TESTS            += single_title_sections
TESTS            += section_remove
TESTS            += quote_before_print
TESTS            += include
TESTS            += searchpath
TESTS            += env

check_PROGRAMS    = $(TESTS)

DEFS              = -DSRC_DIR='"$(srcdir)"'
LDADD             = -L../src ../src/libconfuse.la $(LTLIBINTL)
CLEANFILES        = *~


