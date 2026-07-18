## Process this file with automake to produce Makefile.in

CLEANFILES       = *~ '\#*\#'
pkgconfigdir     = $(libdir)/pkgconfig
pkgconfig_DATA   = libconfuse.pc
doc_DATA         = README.md AUTHORS LICENSE ChangeLog.md
EXTRA_DIST       = README.md LICENSE ChangeLog.md libconfuse.spec.in libconfuse.spec

EXAMPLES         =
if ENABLE_EXAMPLES
  EXAMPLES      += examples
endif
SUBDIRS          = m4 po src $(EXAMPLES) tests doc

## Windows build files, for ZIP archive
BORLAND_FILES         = cfgtest.bpf cfgtest.bpr config.h confuse.bpg libConfuse.bpf libConfuse.bpr
DEVCPP_FILES          = cfgtest.dev config.h libConfuse.dev
MSVC6_FILES           = config.h libConfuse.dsp libConfuse.dsw unistd.h
MSVS_FILES            = config.h msvs.net.sln unistd.h
MSVS_CFGTEST_FILES    = cfgtest.vcproj
MSVS_LIBCONFUSE_FILES = libConfuse.vcproj
MINGW_FILES           = build.bat config.h Makefile

add-windows-files:
	@mkdir $(distdir)/windows
	@mkdir $(distdir)/windows/borland
	@mkdir $(distdir)/windows/devcpp
	@mkdir $(distdir)/windows/msvc6
	@mkdir $(distdir)/windows/msvc6/libConfuse
	@mkdir $(distdir)/windows/msvs.net
	@mkdir $(distdir)/windows/msvs.net/cfgtest
	@mkdir $(distdir)/windows/msvs.net/libConfuse
	@mkdir $(distdir)/windows/mingw
	@for f in $(BORLAND_FILES); do \
		cp -f $(top_srcdir)/windows/borland/$$f $(distdir)/windows/borland/; done
	@for f in $(DEVCPP_FILES); do \
		cp -f $(top_srcdir)/windows/devcpp/$$f $(distdir)/windows/devcpp/; done
	@for f in $(MSVS_FILES); do \
		cp -f $(top_srcdir)/windows/msvs.net/$$f $(distdir)/windows/msvs.net/; done
	@for f in $(MSVS_CFGTEST_FILES); do \
		cp -f $(top_srcdir)/windows/msvs.net/cfgtest/$$f $(distdir)/windows/msvs.net/cfgtest/; done
	@for f in $(MSVS_LIBCONFUSE_FILES); do \
	   cp -f $(top_srcdir)/windows/msvs.net/libConfuse/$$f $(distdir)/windows/msvs.net/libConfuse; done
	@for f in $(MSVC6_FILES); do \
	   cp -f $(top_srcdir)/windows/msvc6/libConfuse/$$f $(distdir)/windows/msvc6/libConfuse; done
	@for f in $(MINGW_FILES); do \
	   cp -f $(top_srcdir)/windows/mingw/$$f $(distdir)/windows/mingw; done
	@cp $(top_srcdir)/examples/wincfgtest.c $(distdir)/examples
	@cp $(top_srcdir)/examples/wincfgtest.conf $(distdir)/examples
	@cp $(top_srcdir)/examples/wincfgtest.dev $(distdir)/examples

unpack:
	@tar xfz $(distdir).tar.gz

zip: dist unpack add-windows-files
	@rm -f $(distdir).zip
	@zip -qr $(distdir).zip $(distdir)
	@rm -rf $(distdir)

rpm: Makefile
	@$(MAKE) dist
	@rpmbuild -ta --clean $(distdir).tar.gz

## ZIP files are reserved for the Windows distribution of libConfuse
dist-zip: zip
