EXTRA_DIST        = a.conf b.conf spdir check_confuse.h
TESTS             = suite_single suite_dup suite_func suite_list	\
		    suite_validate list_plus_syntax section_title_dupes	\
		    single_title_sections quote_before_print include	\
		    searchpath env
check_PROGRAMS    = $(TESTS)

DEFS              = -DSRC_DIR='"$(srcdir)"'
LDADD             = -L../src ../src/libconfuse.la $(LTLIBINTL)
CLEANFILES        = *~


