
Building libConfuse on Windows with MSYS2/MINGW64
=================================================

libConfuse builds on Windows using its normal GNU autotools flow inside
an MSYS2 environment.  This is the tested, supported method -- the same
steps run on every commit, see the "msys2" job in the source tree's
.github/workflows/build.yml.

The build.bat and hand-written Makefile also in this directory target a
long-obsolete MinGW toolchain and do not build the bundled fmemopen() and
reallocarray() replacements that Windows needs, so linking against their
output fails with undefined references (see issue #172).  They are kept
only for historical reference -- use the steps below instead.


Prerequisites
-------------

1. Install MSYS2, see https://www.msys2.org/
2. Open the "MSYS2 MINGW64" shell (not the plain MSYS shell).
3. Install the build tools:

       pacman -S git gcc make autoconf automake libtool flex \
                 gettext-devel pkgconf doxygen


Build and install
-----------------

       ./autogen.sh
       ./configure --prefix=/mingw64
       make
       make check                 # optional, runs the unit tests
       make install

To skip the example programs, which only add libtool wrapper warnings
under recent GCC, configure with --disable-examples.

This installs the shared library libconfuse-*.dll and its import library,
the confuse.h header, and the libconfuse.pc pkg-config file, all under
/mingw64.


Using the library
-----------------

With pkg-config:

       gcc myprog.c $(pkg-config --cflags --libs libconfuse) -o myprog.exe

or directly:

       gcc -I/mingw64/include myprog.c -L/mingw64/lib -lconfuse -o myprog.exe
