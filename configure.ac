dnl Process this file with -*- autoconf -*- to produce a configure script.
dnl $Id$

AC_PREREQ(2.50)

AC_INIT(libConfuse, 2.3, confuse-devel@nongnu.org, confuse)
AC_CONFIG_AUX_DIR(support)
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(config.h)
AC_CONFIG_SRCDIR(src/confuse.c)

# Checks for programs.
AC_PROG_CC
AM_PROG_LEX
AC_ENABLE_SHARED(no)
AC_PROG_LIBTOOL
# Checks for libraries.

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION(0.14.1)

AM_PATH_CHECK(, [AC_SUBST([check_confuse], ['check_confuse${EXTEXT}'])], [AC_WARN([Check not found, tests will not be built])])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
# AC_FUNC_MALLOC
# AC_FUNC_VPRINTF
AC_CHECK_FUNCS([strcasecmp strdup strndup])

AC_CONFIG_FILES([Makefile Doxyfile src/Makefile examples/Makefile \
 po/Makefile.in m4/Makefile tests/Makefile libconfuse.spec confuse-config])
AC_OUTPUT

