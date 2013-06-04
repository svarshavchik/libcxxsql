#
# Copyright 2012-2013 Double Precision, Inc.
# See COPYING for distribution information.

AC_PREREQ(2.61)
AC_INIT(libcxxsql, 0.1, [mrsam@courier-mta.com])

AC_CONFIG_SRCDIR([includes/sql/env.H])
AC_CONFIG_HEADER([libcxx_config.h])
AM_INIT_AUTOMAKE([dist-bzip2 foreign -Wno-portability])

# Checks for programs.
AC_PROG_CXX
AC_PROG_LIBTOOL
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.18.1])
AC_LANG(C++)

XGETTEXT_EXTRA_OPTIONS="--keyword=_txt --keyword=_txtw --keyword=_txtn:1,2 --keyword=_txtnw:1,2 \`cd \$(top_srcdir); echo sql/*.C\`"

# Checks for libraries.

AC_ARG_WITH(pkgconfigdir,[AS_HELP_STRING([--with-pkgconfigdir=dir],[Specify where pkgconfig configuration file gets installed])],
	[PKGCONFIGDIR="$withval"],
	[
for d in /usr/lib64/pkgconfig /usr/lib/pkgconfig /usr/local/libdata/pkgconfig
do
	if test -d "$d"
	then
		PKGCONFIGDIR="$d"
		break
	fi
done
])

if test "$PKGCONFIGDIR" = ""
then
	AC_MSG_ERROR([Unable to find pkgconfig installation directory (use --with-pkgconfigdir to set manually)])
fi
AC_SUBST(PKGCONFIGDIR)

LIBCXX_INIT
CXXFLAGS="$CXXFLAGS -fvisibility-inlines-hidden"
export CXXFLAGS
LDFLAGS="$LDFLAGS -z relro"
export LDFLAGS

AC_PATH_PROG(ODBC_CONFIG,odbc_config,none)
if test "$ODBC_CONFIG" = "none"
then
	AC_MSG_ERROR([odbc_config not found, install unixODBC])
fi

ODBC_LIBS="`$ODBC_CONFIG --libs`"
AC_SUBST(ODBC_LIBS)

ODBC_CFLAGS="`$ODBC_CONFIG --cflags`"

save_LIBS="$LIBS"
save_CPPFLAGS="$CPPFLAGS"

LIBS="$LIBX $ODBC_LIBS"
CPPFLAGS="$CPPFLAGS $ODBC_CPPFLAGS"

AC_TRY_LINK([
#include <sql.h>
#include <odbcinst.h>

], [
   SQLHENV h;

   SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &h);
], [
   :
], [
   AC_MSG_ERROR([Cannot compile a test unixODBC program, make sure you have unixODBC development files installed. You can find exact compiler errors in config.log.])
])
AC_SUBST(ODBC_CFLAGS)

LIBS="$save_LIBS"
CPPFLAGS="$save_CPPFLAGS"

VERSION_INFO=0:0:0
AC_SUBST(VERSION_INFO)

test "x$prefix" = xNONE && prefix=$ac_default_prefix
test "x$exec_prefix" = xNONE && exec_prefix='${prefix}'
eval "prefix=\"$prefix\""
eval "exec_prefix=\"$exec_prefix\""
eval "sbindir=\"$sbindir\""
eval "httportmapd=\"$sbindir/httportmapd\""

AC_SUBST(httportmapd)
AC_CONFIG_FILES([Makefile
		 Doxyfile
                 po/Makefile.in
		 includes/sql/Makefile
		 includes/sql/dbi/Makefile
		 packaging/fedora/libcxxsql.spec
		 sql/Makefile
		 ])
AC_OUTPUT
