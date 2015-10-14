Change Log
==========

All notable changes in libConfuse are documented in this file.


[v2.8][] - [UNRELEASED]
-----------------------

### Changes

* Support for specifying a searchpath for `cfg_parse()`, by J.J. Green
* Restore build of shared library by default, by Nathan Phillip Brink
* Added German translation, contributed by Chris Leick, Aurelien Jarno,
  and Tux^verdreifelt.
* Document `CFG_SIMPLE_STR` for doxygen, by Nathan Phillip Brink
* Update ISC license to 2007 version, by Joachim Nilsson
* Write files in a Bourne shell compatible way, by Alvaro G. M
* Fix mid-string environment variable substitution, by Frank Hunleth

### Fixes
* Various ISO C90 and `-ansi` fixes by Carlo Marcelo Arenas Belon
* Fix C++ compiler warnings for `const` strings, by Craig McQueen
* Fix `make distcheck` and out-of-source builds, by Nathan Phillip Brink
* Fix missing `.gitignore` files, by Carlo Marcelo Arenas Belon
* Fix `CFG_SIMPLE_INT` on 64-bit systems, by Carlo Marcelo Arenas Belon
* Coding style cleanup by J.J. Green
* Fix issue #27: searchpath free problems.  Fix to new feature
  introduced in this release cycle.


[v2.7][] - 2010-02-20
---------------------

### Changes

* Expose `cfg_setopt()` function in public API, suggested by Daniel Pocock
* Add doxygen documentation for `cfg_setopt()`, by Daniel Pocock
* Don't fail on compiler warnings, remove `-Werror`, by Martin Hedenfalk
* Avoid aborting processing of the configuration buffer after returning
  from include processing.  By Carlo Marcelo Arenas Belon
* Make building of examples optional.

### Fixes

* Fix user defined error callbacks, by Martin Hedenfalk
* Include `locale.h`, required by `setlocale()`, patch by Diego Petteno
* Check for `inet_ntoa()` in external library, needed for tests to pass
  on Solaris, patch by Diego Petteno
* Fixes for build warnings, by Martin Hedenfalk and Carlo Marcelo Arenas Belon
* Fix segfault when processing a buffer that called `cfg_include()`, patch
  submitted by Carlo Marcelo Arenas Belon
* Fix lexer match problem with unquoted strings ending in a slash.  Fixed by
  Martin Hedenfalk, reported by Sylvain Bertrand.


[v2.6][] - 2007-10-13
---------------------

### Changes

* added French translation contributed by Matthieu Sion
* added build script and instructions for compiling with Mingw under
  Windows (contributed by Matthieu Sion)
* now accepts a simplified list append syntax:
    
    option += "value"
      instead of
    option += {"value"}
    
* added flag `CFGF_NO_TITLE_DUPES`: multiple section titles must be
  unique (duplicates raises an error, only applies to sections)
  (suggested by Brian Fallik)
* remove obsolete `confuse-config` script in favour of `pkg-config`
* windows build files now only in separate zip distribution

### Fixes

* fixed rpm builds, patch by Dan Lipsitt
* always installs `pkg-config` .pc script
* fixed a bug reported by Josh Kropf with single sections with titles
* added patch that escapes values with quotes and backslashes when printing.
* fixed a memory leak in default values for string lists,
  reported by Vineeth Neelakant.


[v2.5][] - 2004-10-17
---------------------

### Changes

* added flag `CFGF_NODEFAULT` (option has no default value)
* added a tutorial
* updated autoconf stuff, libconfuse installs with appropriate suffix now
* added data file for `pkg-config` (try `pkg-config --libs libconfuse`)
* updated `confuse-config` script (now only installed if `pkg-config` not found)
* added `cfg_name()` and `cfg_opt_name()` functions

### Fixes

* fixed `cfg_set_validate_func()` for sections, using the "|" syntax


[v2.4][] - 2004-08-09
---------------------

### Changes

* added option type `CFGT_PTR` as a user-defined type

### Fixes

* fixed building of shared libraries


[v2.3][] - 2004-05-22
---------------------

### Changes

* options passed to `cfg_init()` are now dynamically duplicated, so it
  is no longer necessary to declare the `cfg_opt_t array` static
* added tests using 'check' (a unit testing framework for C)
* added config script `confuse-config`

### Fixes

* fixes compilation errors with gcc < 3.3


[v2.2][] - 2003-09-25
---------------------

### Changes

* Allows more characters in an unquoted string (thanks Mike)
* added `cfg_opt_get` functions
* added `cfg_opt_size` function
* added support to print options to a file
* added print callback function per option
* simple options can be retrieved with the `cfg_get` functions (allows
  using the `cfg_print` function for simple values)
* added validating callback function per option


[v2.1][] - 2003-07-13
---------------------

### Changes

* Reversed logic in `cfg_getXXX` functions, they now abort if given an
  undeclared option name, and NULL/false if no value is set. Suggested
  by Ademar de Souza Reis Jr.
* Sections without `CFGF_MULTI` flag now have default values
* The `cfg_getXXX` functions now accept an extended syntax for the
  option name, try `cfg_getxxx(cfg, "sectionname|optionname")`.  This
  way one doesn't have to first get the section with `cfg_getsec()`.
* Added project files for MS Visual C++ 6.0
* Includes io.h on windows
* Setting a list to the empty list in the config file now possible.
* Appending to default values in a list is now OK.
* Hexadecimal escape sequences allowed in double-quoted strings
* Only include NLS support if gettext found in libc or preinstalled
* Documented the `cfg_setlist` and `cfg_addlist` functions
* The `cfg_opt_setxxx` functions no longer take a `cfg_t?` parameter (unused anyway)

### Fixes

* Fixed two more memory leaks. (`val->section` and `cfg->filename`)
* Fixed unterminated string bug in replacement strndup function
* Fixed initialization of default values for lists, when given a NULL
  string. Now initialized to the empty list. Noted by Juraj Variny.
* Corrected line number with multi-line quoted strings
* Fixed undetected `/*comment*/` (ie, without space between /* and the text)
* Forgot to `fclose()` include file after use, found by James Haley


v2.0 - 2003-04-29
-----------------

**NOTE:** Compatibility with earlier versions is broken!

### Changes

* Changed `cfg_flag_t` from `enum` to `int` (should now compile with C++)
* Variable number of arguments to functions: function types should no
  longer specify number of expected arguments in the initializer, the
  callback should instead check the `argc` variable.
* Added documentation for the value parsing callback
* Changed the definitions of `cfg_func_t` and `cfg_callback_t`, the cfg
  and option context are now both passed as parameters
* Added a bunch of `cfg_setXXX` functions to set option values after parsing
* Some types renamed for consistency (`cfgopt_t` to `cfg_opt_t`, `cfgval_t`
  to `cfg_value_t`, `cfgbool_t` to `cfg_bool_t`)
* `cfg_free_val()` renamed to `cfg_free_value()`
* Lexer symbols now uses prefix `cfg_` to ease linking with other lexers
* Sections with same title are always overwritten
* Lists can now have (complete) default values in the form of a string
  that is parsed in the same way as the config file (see doc + examples)
* Added support for building as a DLL on Windows
* Included project files for Borland C++ Builder 6.0
* Included project files for Dev-Cpp 5.0
* Included project files for MS Visual Studio
* Pre-built documentation now included in the source tarball

### Fixes

* Fixed the `cfg_tilde_expand` function
* Fixed and extended the example programs
* Forgot to close the file in `cfg_parse()`
* Memory leaks fixed (checked with valgrind)


v1.2.3 - 2002-12-18
-------------------

### Changes

* added callback support

### Fixes

* fixed segfault due to uninitialized user-defined error function


v1.2.2 - 2002-11-27
-------------------

### Changes

* changed name to libConfuse (libcfg was way too common)
* Don't build shared libraries by default (only static)
* More Swedish translations
* Implemented the `cfg_free()` function (previous versions had only a stub)
* New function: `cfg_free_val()`
* updated the manual


[UNRELEASED]: https://github.com/martinh/libconfuse/compare/v2.7...HEAD
[v2.8]: https://github.com/martinh/libconfuse/compare/v2.7...v2.8
[v2.7]: https://github.com/martinh/libconfuse/compare/v2.6...v2.7
[v2.6]: https://github.com/martinh/libconfuse/compare/v2.5...v2.6
[v2.5]: https://github.com/martinh/libconfuse/compare/v2.4...v2.5
[v2.4]: https://github.com/martinh/libconfuse/compare/v2.3...v2.4
[v2.3]: https://github.com/martinh/libconfuse/compare/v2.2...v2.3
[v2.2]: https://github.com/martinh/libconfuse/compare/v2.1...v2.2
[v2.1]: https://github.com/martinh/libconfuse/compare/v2.0...v2.1
