# vim:ft=make
EXTRA_DIST = doxygen-footer.html doxygen-header.html doxygen.css tutorial.xml \
			 $(TUTORIAL_CSOURCES) css.xsl tutorial.css

BUILT_SOURCES = tutorial-build.timestamp

#TUTORIAL_CSOURCES := $(shell grep 'ENTITY listing' tutorial.xml | sed 's/.*ENTITY \(.*\) SYSTEM.*/\1.c/')
TUTORIAL_CSOURCES = listing1.c listing2.c listing3.c listing4.c listing5.c listing6.c listing7.c
TUTORIAL_LISTINGS = $(TUTORIAL_CSOURCES:.c=.xml)

.c.xml:
	echo "<programlisting id=\"$*\"><![CDATA[" > $@
	cat $< | awk 'BEGIN {l=1} /.*/ {printf("%02d   %s\n", l, $$0); l++}' >> $@
	echo ']]></programlisting>' >> $@

tutorial-build.timestamp: tutorial.xml $(TUTORIAL_LISTINGS) css.xsl tutorial.css
	xmllint --valid --noout $< && xmlto -o tutorial-html --extensions -m css.xsl xhtml $<
	touch tutorial-build.timestamp
	cp -f tutorial.css tutorial-html/

tutorial.pdf: tutorial.xml $(TUTORIAL_LISTINGS)
	xmllint --valid --noout $< && xmlto pdf $<

html: tutorial-build.timestamp
pdf: tutorial.pdf

dist-hook:
	cp -pr html $(distdir)
	cp -pr man $(distdir)
	cp -pr tutorial-html $(distdir)

CLEANFILES=*~ '\#*\#' $(TUTORIAL_LISTINGS) tutorial-build.timestamp
DISTCLEANFILES=tutorial.pdf

celan: clean

