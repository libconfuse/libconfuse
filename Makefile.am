## Process this file with automake to produce Makefile.in
## $Id$

AUTOMAKE_OPTIONS = foreign
CLEANFILES=*~ \#*\#
EXTRA_DIST=Doxyfile.in Doxyfile libconfuse.spec.in libconfuse.spec confuse-config.in
SUBDIRS = m4 po src examples tests

bin_SCRIPTS=confuse-config

doc: doc/html/index.html
doc/html/index.html: src/confuse.h
	doxygen

celan: clean

ACLOCAL_AMFLAGS = -I m4

BORLAND_FILES=cfgtest.bpf cfgtest.bpr config.h confuse.bpg libConfuse.bpf libConfuse.bpr
DEVCPP_FILES=cfgtest.dev config.h libConfuse.dev
MSVC6_FILES=config.h libConfuse.dsp libConfuse.dsw unistd.h
MSVS_FILES=config.h msvs.net.sln
MSVS_CFGTEST_FILES=cfgtest.vcproj
MSVS_LIBCONFUSE_FILES=libConfuse.vcproj

DOC_FILES=doxygen-footer.html doxygen-header.html doxygen.css
DOC_HTML_FILES=annotated.html confuse_8h.html examples.html globals.html \
  simple_8c-example.html unioncfg__value__t.html cfgtest_8c-example.html \
  doxygen.css files.html index.html structcfg__opt__t.html \
  confuse_8h-source.html doxygen.png functions.html reread_8c-example.html \
  structcfg__t.html
DOC_MAN_MAN3_FILES=cfg_opt_t.3 cfg_t.3 cfg_value_t.3 confuse.h.3

dist-hook: doc
	mkdir $(distdir)/windows
	mkdir $(distdir)/windows/borland
	mkdir $(distdir)/windows/devcpp
	mkdir $(distdir)/windows/msvc6
	mkdir $(distdir)/windows/msvc6/libConfuse
	mkdir $(distdir)/windows/msvs.net
	mkdir $(distdir)/windows/msvs.net/cfgtest
	mkdir $(distdir)/windows/msvs.net/libConfuse
	mkdir $(distdir)/doc
	mkdir $(distdir)/doc/html
	mkdir $(distdir)/doc/man
	mkdir $(distdir)/doc/man/man3

	for f in $(BORLAND_FILES); do \
		cp -f $(top_srcdir)/windows/borland/$$f $(distdir)/windows/borland/; done
	for f in $(DEVCPP_FILES); do \
		cp -f $(top_srcdir)/windows/devcpp/$$f $(distdir)/windows/devcpp/; done
	for f in $(MSVS_FILES); do \
		cp -f $(top_srcdir)/windows/msvs.net/$$f $(distdir)/windows/msvs.net/; done
	for f in $(MSVS_CFGTEST_FILES); do \
		cp -f $(top_srcdir)/windows/msvs.net/cfgtest/$$f $(distdir)/windows/msvs.net/cfgtest/; done
	for f in $(MSVS_LIBCONFUSE_FILES); do \
	   cp -f $(top_srcdir)/windows/msvs.net/libConfuse/$$f $(distdir)/windows/msvs.net/libConfuse; done
	for f in $(MSVC6_FILES); do \
	   cp -f $(top_srcdir)/windows/msvc6/libConfuse/$$f $(distdir)/windows/msvc6/libConfuse; done

	for f in $(DOC_FILES); do \
		cp -f $(top_srcdir)/doc/$$f $(distdir)/doc/; done
	for f in $(DOC_HTML_FILES); do \
		cp -f $(top_srcdir)/doc/html/$$f $(distdir)/doc/html/; done
	for f in $(DOC_MAN_MAN3_FILES); do \
		cp -f $(top_srcdir)/doc/man/man3/$$f $(distdir)/doc/man/man3/; done

dist-extra: dist
	tar zxf $(PACKAGE)-$(VERSION).tar.gz
	rm -f $(PACKAGE)-$(VERSION).zip
	zip -9 -r $(PACKAGE)-$(VERSION).zip $(PACKAGE)-$(VERSION)
	rm -f $(PACKAGE)-$(VERSION).tar.bz2
	tar jcf $(PACKAGE)-$(VERSION).tar.bz2 $(PACKAGE)-$(VERSION)
	rm -rf $(PACKAGE)-$(VERSION)

rpm: Makefile
	$(MAKE) dist
	rpm -ta --clean $(PACKAGE)-$(VERSION).tar.gz
