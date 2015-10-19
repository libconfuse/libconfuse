dnl Process this file with -*- autoconf -*- to produce a configure script.

AC_PREREQ(2.50)

AC_INIT(libConfuse, 2.8, https://github.com/martinh/libconfuse/issues, confuse)
AC_CONFIG_AUX_DIR(support)
AM_MAINTAINER_MODE
AM_INIT_AUTOMAKE([foreign dist-xz])
AM_SILENT_RULES([no])

AM_CONFIG_HEADER(config.h)
AC_CONFIG_SRCDIR(src/confuse.c)

# Checks for programs.
AC_PROG_CC
AM_PROG_AR
AM_PROG_LEX

LT_INIT([win32-dll])

# optional building of examples:
AC_ARG_ENABLE([examples],
	[AC_HELP_STRING([--disable-examples], [don't build examples in examples])],
	[], [enable_examples=yes])
AM_CONDITIONAL([ENABLE_EXAMPLES], [test "$enable_examples" = yes])

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.16.1])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([unistd.h])
AC_CHECK_HEADERS([sys/stat.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_CHECK_FUNCS([strcasecmp strdup strndup setenv unsetenv _putenv])

AC_CONFIG_FILES([Makefile \
		 src/Makefile \
		 examples/Makefile \
                 po/Makefile.in \
		 m4/Makefile \
		 tests/Makefile \
		 doc/Makefile \
                 doc/Doxyfile \
		 libconfuse.pc \
		 libconfuse.spec])
AC_OUTPUT

