#!/bin/sh

echo -n "Running autopoint..."
autopoint && echo " done"
rm -f ABOUT-NLS

echo -n "Running aclocal..."
aclocal -I m4 || exit
echo " done"

echo -n "Running autoconf..."
autoconf || exit
echo " done"

echo -n "Running autoheader..."
autoheader || exit
echo " done"

echo -n "Running libtoolize..."
libtoolize --automake || glibtoolize --automake || exit
echo " done"

echo -n "Running automake..."
automake --add-missing || exit
echo " done"

echo "Running configure $*..."
./configure --enable-maintainer-mode $*

