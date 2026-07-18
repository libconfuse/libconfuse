libConfuse
==========
[![Badge][]][ISC] [![Release Badge][]][Release] [![GitHub Status][]][GitHub] [![Coverity Status][]][Coverity Scan] [![CodeDocs Status][]][CodeDocs]

* [Introduction](#introduction)
* [Documentation](#documentation)
* [Examples](#examples)
* [Build & Install](#build--install)
* [Origin & References](#origin--references)

Introduction
------------

libConfuse is a configuration file parser library written in C.  It
supports sections and (lists of) values, as well as other features such
as single/double quoted strings, environment variable expansion,
functions and nested include statements.  Values can be strings,
integers, floats, booleans, and sections.

The goal is not to be _the_ configuration file parser library with a
gazillion of features.  Instead, it aims to be easy to use and quick to
integrate with your code.

> [!IMPORTANT]
> Please ensure you download a <ins>versioned archive</ins> from the
> official [releases page][3].

Documentation
-------------

* [API reference and tutorial][CodeDocs], regenerated on every commit

Examples
--------

* [simple.c](examples/simple.c) and [simple.conf](examples/simple.conf)
  shows how to use the "simple" versions of options
* [cfgtest.c](examples/cfgtest.c) and [test.conf](examples/test.conf)
  show most of the features of confuse, including lists and functions

Build & Install
---------------

libConfuse employs the GNU configure and build system.  To list available
build options, start by unpacking the tarball:

```bash
$ tar xf confuse-3.2.2.tar.xz
$ cd confuse-3.2.2/
$ ./configure --help
...
```

For most users the following commands configures, builds and installs the
library to `/usr/local/`:

```bash
$ ./configure && make -j9
...
$ sudo make install
$ sudo ldconfig
```

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

To build the documentation you also need the following tool:

* doxygen

This is an optional step, so you must build it explicitly from
its directory:

```bash
$ cd doc/
$ make documentation
...
```

Origin & References
-------------------

libConfuse was created by Martin Hedenfalk and released as open source
software under the terms of the [ISC license][1].  It was previously
called libcfg, but the name was changed to not confuse with other
similar libraries.  It is currently developed and maintained at GitHub.
Please use the [issue tracker][2] to report bugs and feature requests.

_If you can't convince, confuse._

[1]:                http://en.wikipedia.org/wiki/ISC_license
[2]:                https://github.com/libconfuse/libconfuse/issues
[3]:                https://github.com/libconfuse/libconfuse/releases/
[ISC]:              https://en.wikipedia.org/wiki/ISC_license
[Badge]:            https://img.shields.io/badge/License-ISC-blue.svg
[GitHub]:           https://github.com/libconfuse/libconfuse/actions/workflows/build.yml/
[GitHub Status]:    https://github.com/libconfuse/libconfuse/actions/workflows/build.yml/badge.svg
[Coverity Scan]:    https://scan.coverity.com/projects/6674
[Coverity Status]:  https://scan.coverity.com/projects/6674/badge.svg
[Release]:          https://github.com/libconfuse/libconfuse/releases
[Release Badge]:    https://img.shields.io/github/v/release/libconfuse/libconfuse
[CodeDocs]:         https://codedocs.xyz/libconfuse/libconfuse/
[CodeDocs Status]:  https://codedocs.xyz/libconfuse/libconfuse.svg
