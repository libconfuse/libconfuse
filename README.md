libConfuse
==========
[![Travis Status][]][Travis] [![Coverity Status][]][Coverity Scan]

> **Note:** libConfuse was called libcfg before, but was changed to not
>           confuse with other similar libraries.

Table of Contents
-----------------

* [Introduction](#introduction)
* [Examples](#examples)
* [Building](#building)
* [Documentation](#documentation)


Introduction
------------

libConfuse is a configuration file parser library, licensed under the
terms of the [ISC license][1], written in C.  It supports sections and
(lists of) values, as well as other features such as single/double
quoted strings, environment variable expansion, functions and nested
include statements.  Values can be strings, integers, floats, booleans,
and sections.

The goal is not to be _the_ configuration file parser library with a
gazillion of features.  Instead, it aims to be easy to use and quick to
integrate with your code.

Please ensure you download a <ins>versioned archive</ins>:

<https://github.com/martinh/libconfuse/releases/>


Examples
--------

* [simple.c](examples/simple.c) and [simple.conf](examples/simple.conf)
  shows how to use the "simple" versions of options
* [cfgtest.c](examples/cfgtest.c) and [test.conf](examples/test.conf)
  show most of the features of confuse, including lists and functions


Building
--------

libConfuse employs the GNU configure and build system.  To list available
build options, start by unpacking the tarball:

    tar xf confuse-3.2.2.tar.xz
    cd confuse-3.2.2/
    ./configure --help

For most users the following commands configures, builds and installs the
library to `/usr/local/`:

    ./configure && make -j9
    sudo make install

See the INSTALL file for the full installation instructions.

When checking out the code from GitHub, use <kbd>./autogen.sh</kbd> to
generate a `configure` script.  This means you also need the following
tools:

* autoconf
* automake
* libtool
* gettext
* autopoint
* flex

To build the documentation you also need the following tools:

* doxygen
* xmlto

This is an optional step, so you must build it explicitly from
its directory:

    cd doc/
    make documentation


Documentation
-------------

For the time being, the following documentation is published at the
[old homepage](http://www.nongnu.org/confuse/), but also distributed
with the source:

* [API reference/manual](http://www.nongnu.org/confuse/manual/) (generated with doxygen)
* [Tutorial](http://www.nongnu.org/confuse/tutorial-html/) (a work in progress)
* [ChangeLog](ChangeLog.md) (check what's new!)


[1]:                http://en.wikipedia.org/wiki/ISC_license
[2]:                https://github.com/martinh/libconfuse/issues
[Travis]:           https://travis-ci.org/troglobit/libconfuse
[Travis Status]:    https://travis-ci.org/troglobit/libconfuse.png?branch=master
[Coverity Scan]:    https://scan.coverity.com/projects/6674
[Coverity Status]:  https://scan.coverity.com/projects/6674/badge.svg
