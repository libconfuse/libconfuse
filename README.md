libConfuse
==========
[![Travis Status][]][Travis] [![Coverity Status][]][Coverity Scan]

Table of Contents
-----------------

* [Introduction](#introduction)
* [Examples](#examples)
* [Download](#download)
* [Building](#building)
* [Documentation](#documentation)
* [News](#news)


Introduction
------------

libConfuse is a configuration file parser library, licensed under the
terms of the [ISC license][1], written in C.  It supports sections and
(lists of) values (strings, integers, floats, booleans, and sections),
as well as some other features such as single/double-quoted strings,
environment variable expansion, functions and nested include statements.

The goal is not to be _the_ configuration file parser library with a
gazillion of features.  Instead, it aims to be easy to use and quick to
integrate with your code.

The library is available pre-built in many Linux and UNIX distributions.
See the following pages for information on how to download and build it
yourself:

* https://github.com/martinh/libconfuse/releases/

Please report bugs to the [issue tracker][2].  If you want to contribute
fixes or new features, see [CONTRIBUTING.md](CONTRIBUTING.md).

> **Note:** libConfuse was called libcfg before, but was changed to not
>           confuse with other similar libraries.

Examples
--------

Example configuration files:

* [test.conf](examples/test.conf) and the
  [source code](examples/cfgtest.c) shows most of the
  features of confuse, including lists and functions.
* [simple.conf](examples/simple.conf) shows how to use the
  "simple" versions of options. See the corresponding
  [source](examples/simple.c).


Building
--------

libConfuse employs the GNU configure and build system.  Simply enter
<kbd>./configure --help</kbd> to list available options and see the
INSTALL file for the full installation instructions.

When checking out the code from GitHub, use <kbd>./autogen.sh</kbd> to
generate a `configure` script.  This means you also need the following
tools:

* autoconf
* automake
* libtool
* autopoint
* flex
* doxygen


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
