## Process this file with automake to produce Makefile.in
## $Id$

#AUTOMAKE_OPTIONS = foreign
CLEANFILES=*~ \#*\#
EXTRA_DIST= config.rpath Doxyfile.in
SUBDIRS = m4 po src examples doc

doc: doc/html/index.html
doc/html/index.html: src/confuse.h
	doxygen

celan: clean

ACLOCAL_AMFLAGS = -I m4
