#
# Copyright 2012-2020 Double Precision, Inc.
# See COPYING for distribution information.

AC_PREREQ([2.71])
AC_INIT([libcxxsql],[0.6.1],[mrsam@courier-mta.com])
AC_CANONICAL_TARGET

AC_CONFIG_SRCDIR([includes/sql/env.H])
AC_CONFIG_HEADERS([libcxx_config.h])
AM_INIT_AUTOMAKE([dist-bzip2 foreign -Wno-portability])

# Checks for programs.
AC_PROG_CXX
LT_INIT
AM_GNU_GETTEXT([external])
AC_LANG(C++)

# VERSION_INFO is:
#
#   libtool's -version-info
#
# ABIVER is, basically, the major version, the ABI version.
#
#   $libdir/libcxxsql-$ABIVER-$LIBCXX_VERSION
#
#   $includedir/libcxxsql-$ABIVER-$LIBCXX_VERSION
#
#   $datadir/libcxxsql-$ABIVER-$LIBCXX_VERSION
#
# Our package $VERSION is:
#
#   $libdir/pkgconfig/libcxxsql-$VERSION.$LIBCXX_VERSION.pc
#   $datadir/aclocal/libcxxsql-$VERSION-$LIBCXX_VERSION.m4
#
ABIVER=0.06
AC_SUBST(ABIVER)
SOVERSION=6
AC_SUBST(SOVERSION)
VERSION_INFO=${SOVERSION}:0:0
AC_SUBST(VERSION_INFO)

XGETTEXT_EXTRA_OPTIONS="--keyword=_txt --keyword=_txtw --keyword=_txtn:1,2 --keyword=_txtnw:1,2 \`cd \$(top_srcdir); echo sql/*.C\`"

# Checks for libraries.

AC_ARG_WITH(pkgconfigdir,[AS_HELP_STRING([--with-pkgconfigdir=dir],[Specify where pkgconfig configuration file gets installed])],
	[PKGCONFIGDIR="$withval"],
	[
for d in /usr/lib64/pkgconfig /usr/lib/$host_cpu-$host_os/pkgconfig \
    /usr/lib/pkgconfig /usr/local/libdata/pkgconfig
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
LDFLAGS="$LDFLAGS -z relro -z now"
export LDFLAGS

if pkg-config --exists odbc
then
	:
else
	AC_MSG_ERROR([unixODBC package not found])
fi

ODBC_CFLAGS="`pkg-config --variable=build_cflags odbc`"
ODBC_LIBS="`pkg-config --libs odbc`"
AC_SUBST(ODBC_LIBS)

save_LIBS="$LIBS"
save_CPPFLAGS="$CPPFLAGS"

LIBS="$LIBX $ODBC_LIBS"
CPPFLAGS="$CPPFLAGS $ODBC_CPPFLAGS"

AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <sql.h>
#include <odbcinst.h>

]], [[
   SQLHENV h;

   SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &h);
]])],[
   :
],[
   AC_MSG_ERROR([Cannot compile a test unixODBC program, make sure you have unixODBC development files installed. You can find exact compiler errors in config.log.])
])
AC_SUBST(ODBC_CFLAGS)

LIBS="$save_LIBS"
CPPFLAGS="$save_CPPFLAGS"

test "x$prefix" = xNONE && prefix=$ac_default_prefix
test "x$exec_prefix" = xNONE && exec_prefix='${prefix}'
eval "prefix=\"$prefix\""
eval "exec_prefix=\"$exec_prefix\""
eval "sbindir=\"$sbindir\""
eval "httportmapd=\"$sbindir/httportmapd\""

AC_SUBST(httportmapd)
AC_SUBST(LIBCXX_NAMESPACE,x)
AC_CONFIG_FILES([Makefile po/Makefile.in
		 Doxyfile
		 libcxxsql.m4
		 includes/Makefile
		 includes/sql/Makefile
		 includes/sql/dbi/Makefile
		 includes/buildschema.xsl
		 includes/libcxxsql.mk
		 packaging/fedora/libcxxsql.spec
		 sql/Makefile
		 debuild
		 ])
AC_OUTPUT
