dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(timeit, 0.9.10, http://freshmeat.net/projects/timeit)

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

AC_ISC_POSIX
AC_PROG_CXX
AM_PROG_CC_STDC
AC_HEADER_STDC


dnl ***************************************************************************
dnl Internatinalization
dnl ***************************************************************************
GETTEXT_PACKAGE=AC_PACKAGE_NAME
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [GETTEXT package name])
AM_GLIB_GNU_GETTEXT
IT_PROG_INTLTOOL([0.35.0])

PROGRAMNAME_LOCALEDIR=[${datadir}/locale]
AC_SUBST(PROGRAMNAME_LOCALEDIR)

AM_PROG_LIBTOOL



PKG_CHECK_MODULES(TIMEIT, [gtkmm-2.4 >= 2.8 sqlite3 xscrnsaver])
AC_SUBST(TIMEIT_CFLAGS)
AC_SUBST(TIMEIT_LIBS)

AC_CONFIG_MACRO_DIR([m4])
AX_BOOST_BASE([1.33])


AC_OUTPUT([
Makefile
src/Makefile
po/Makefile.in
doc/Makefile
doc/html/Makefile
doc/html/C/Makefile
images/Makefile
])
