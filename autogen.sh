#!/bin/sh

echo -n "Running aclocal..."
aclocal-1.7 || exit
echo " done"

echo -n "Running autoheader..."
autoheader2.50 || exit
echo " done"

echo -n "Running autoconf..."
autoconf2.50 || exit
echo " done"

echo -n "Running automake..."
automake-1.7 || exit
echo " done"

echo "Running configure (without arguments)..."
./configure $*
