## Process this file with automake to produce Makefile.in
## $Id$

AUTOMAKE_OPTIONS = foreign
CLEANFILES=*~ \#*\#
EXTRA_DIST=Doxyfile.in Doxyfile libconfuse.spec.in libconfuse.spec
SUBDIRS = m4 po src examples

doc: doc/html/index.html
doc/html/index.html: src/confuse.h
	doxygen

celan: clean

ACLOCAL_AMFLAGS = -I m4

BCB_FILES=README.txt cfgtest.bpf cfgtest.bpr config.h libConfuse.bpf \
  libConfuse.bpr test.conf wincfgtest.bpf wincfgtest.bpr wincfgtest.conf
DEV_CPP_FILES=cfgtest.dev config.h libConfuse.dev test.conf
MSVC_CFGTEST_FILES=cfgtest.ncb cfgtest.sln cfgtest.vcproj test.conf
MSVC_LIBCONFUSE_FILES=config.h libConfuse.sln libConfuse.vcproj
MSVC_WINCFGTEST_FILES=wincfgtest.conf wincfgtest.sln wincfgtest.vcproj
DOC_FILES=doxygen-footer.html doxygen-header.html doxygen.css
DOC_HTML_FILES=annotated.html confuse_8h.html examples.html globals.html \
  simple_8c-example.html unioncfg__value__t.html cfgtest_8c-example.html \
  doxygen.css files.html index.html structcfg__opt__t.html \
  confuse_8h-source.html doxygen.png functions.html reread_8c-example.html \
  structcfg__t.html
DOC_MAN_MAN3_FILES=cfg_opt_t.3 cfg_t.3 cfg_value_t.3 confuse.h.3

dist-hook: doc
	mkdir $(distdir)/BCB
	mkdir $(distdir)/dev-cpp
	mkdir $(distdir)/msvc
	mkdir $(distdir)/msvc/cfgtest
	mkdir $(distdir)/msvc/libConfuse
	mkdir $(distdir)/msvc/wincfgtest
	mkdir $(distdir)/doc
	mkdir $(distdir)/doc/html
	mkdir $(distdir)/doc/man
	mkdir $(distdir)/doc/man/man3

	for f in $(BCB_FILES); do cp -f $(top_srcdir)/BCB/$$f $(distdir)/BCB/; done
	for f in $(DEV_CPP_FILES); do \
		cp -f $(top_srcdir)/dev-cpp/$$f $(distdir)/dev-cpp/; done
	for f in $(MSVC_CFGTEST_FILES); do \
		cp -f $(top_srcdir)/msvc/cfgtest/$$f $(distdir)/msvc/cfgtest/; done
	for f in $(MSVC_LIBCONFUSE_FILES); do \
	   cp -f $(top_srcdir)/msvc/libConfuse/$$f $(distdir)/msvc/libConfuse; done
	for f in $(MSVC_WINCFGTEST_FILES); do \
	   cp -f $(top_srcdir)/msvc/wincfgtest/$$f $(distdir)/msvc/wincfgtest; done

	for f in $(DOC_FILES); do \
		cp -f $(top_srcdir)/doc/$$f $(distdir)/doc/; done
	for f in $(DOC_HTML_FILES); do \
		cp -f $(top_srcdir)/doc/html/$$f $(distdir)/doc/html/; done
	for f in $(DOC_MAN_MAN3_FILES); do \
		cp -f $(top_srcdir)/doc/man/man3/$$f $(distdir)/doc/man/man3/; done
