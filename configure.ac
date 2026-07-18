# Process this file with -*- autoconf -*- to produce a configure script.
# The bundled autogen.sh script does this and also calls automake.
AC_PREREQ([2.69])

AC_INIT([libConfuse], [3.4-dev], [https://github.com/libconfuse/libconfuse/issues], [confuse])
AC_CONFIG_AUX_DIR([support])
AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE([foreign dist-xz])
AM_SILENT_RULES([no])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([src/confuse.c])

# Checks for programs.
AC_PROG_CC
AM_PROG_AR
AC_PROG_LEX([noyywrap])

LT_INIT([win32-dll])

# optional building of examples:
AC_ARG_ENABLE([examples],
	[AS_HELP_STRING([--disable-examples], [don't build examples in examples])],
	[], [enable_examples=yes])
AM_CONDITIONAL([ENABLE_EXAMPLES], [test "$enable_examples" = yes])

# 0.19.6 enables AM_GNU_GETTEXT_REQUIRE_VERSION whixh is required
# for use with support compiling if gettext 0.20 or later is found.
# Thus a minimum of Ubuntu 16.04, Debian Stretch, CentOS7, or RHEL7, 
# is now require.
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.19.6])
AM_GNU_GETTEXT_REQUIRE_VERSION([0.19.6])

# Checks for OS settings.
AC_SYS_LARGEFILE

# Checks for header files.
AC_CHECK_HEADERS([unistd.h string.h strings.h sys/stat.h windows.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_CHECK_FUNCS([fmemopen funopen reallocarray strcasecmp strdup strndup setenv unsetenv _putenv])

# Set conditional includes in Makefile.am
AM_CONDITIONAL(MISSING_FMEMOPEN, [test "x$ac_cv_func_fmemopen" = "xno"])
AM_CONDITIONAL(MISSING_REALLOCARRAY, [test "x$ac_cv_func_reallocarray" = "xno"])
AM_CONDITIONAL(WINDOWS_BUILD, [test "x$ac_cv_header_windows_h" = "xyes"])

# Files to generate
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

