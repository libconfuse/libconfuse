EXTRA_DIST        = codedocs.css tutorial.md $(TUTORIAL_CSOURCES) \
		    Doxyfile.in Doxyfile

# Tutorial code listings, embedded in tutorial.md via Doxygen @include.
TUTORIAL_CSOURCES = listing1.c listing2.c listing3.c listing4.c \
		    listing5.c listing6.c listing7.c listing8.c

CLEANFILES        = *~ '\#*\#'

html/index.html: Doxyfile
	@doxygen

# Remove "Directory Reference", not needed
doxygen-manual: html/index.html ../src/confuse.h
	@find man -name '_home_*' -exec rm {} \;

documentation: doxygen-manual

dist-hook: doxygen-manual
	@cp -pr html $(distdir)
	@cp -pr man $(distdir)

# remove directories created by DoxyGen to pass distcheck
distclean-local:
	@rm -rfv html man
