dnl Process this file with -*- autoconf -*- to produce a configure script.
dnl $Id$

AC_PREREQ(2.50)

AC_INIT(libConfuse, 1.2.4, mhe@home.se, confuse)
dnl AC_INIT_SRCDIR(src/confuse.c)
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(config.h)

# Checks for programs.
AC_PROG_CC
AM_PROG_LEX
AC_ENABLE_SHARED(no)
AC_PROG_LIBTOOL
# Checks for libraries.

AM_GNU_GETTEXT([external])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
# AC_FUNC_MALLOC
# AC_FUNC_VPRINTF
AC_CHECK_FUNCS([strcasecmp strdup])

AC_CONFIG_FILES([Makefile Doxyfile src/Makefile examples/Makefile po/Makefile.in m4/Makefile doc/Makefile])
AC_OUTPUT
