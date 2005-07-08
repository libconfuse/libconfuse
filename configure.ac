dnl Process this file with -*- autoconf -*- to produce a configure script.
dnl $Id$

AC_PREREQ(2.50)

AC_INIT(libConfuse, 2.5, confuse-devel@nongnu.org, confuse)
AC_CONFIG_AUX_DIR(support)
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(config.h)
AC_CONFIG_SRCDIR(src/confuse.c)

# Checks for programs.
AC_PROG_CC
AM_PROG_LEX
AC_DISABLE_SHARED
AC_PROG_LIBTOOL
# Checks for libraries.

AM_CONDITIONAL([cc_is_gcc], test "$GCC" = "yes")

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

dnl pkgconfig/confuse-config.
dnl If pkgconfig is found, use it and disable confuse-config, otherwise do the
dnl opposite, unless the user overrides.

AC_ARG_ENABLE([pkgconfig],
  AC_HELP_STRING([--disable-pkgconfig],[disable pkgconfig data files (auto)]),
  [dopkgconfig="$enableval"], [dopkgconfig="maybe"])
if test "$dopkgconfig" = "maybe"; then
  AC_CHECK_PROG([PKGCONFIG], [pkg-config], [yes], [no])
else
  AC_MSG_CHECKING([whether pkgconfig should be used])
  PKGCONFIG="$dopkgconfig"
  AC_MSG_RESULT([$PKGCONFIG])
fi
AM_CONDITIONAL([HAVE_PKGCONFIG], [test "$PKGCONFIG" != "no"])

CONFUSE_VERSION="$VERSION"
CONFUSE_CPPFLAGS="-I$includedir"
CONFUSE_LIBS="-lconfuse $LTLIBINTL"
AC_CONFIG_LIBCONFIG_IN_STATIC
AC_CONFIG_LIBCONFIG_IN([confuse])

AC_CONFIG_FILES([Makefile doc/Doxyfile src/Makefile examples/Makefile libconfuse.pc \
                 po/Makefile.in m4/Makefile tests/Makefile doc/Makefile libconfuse.spec confuse-config])
AC_OUTPUT

