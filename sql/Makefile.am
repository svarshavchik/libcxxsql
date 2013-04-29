#
# Copyright 2013 Double Precision, Inc.
# See COPYING for distribution information.
#

@LIBCXX_AM@

AM_CPPFLAGS=-I . -I $(srcdir) -I $(srcdir)/../includes -I ../includes @ODBC_CFLAGS@

BUILT_SOURCES=x
CLEANFILES=x

LINKTYPE=

x:
	ln -s $(srcdir)/../includes x

lib_LTLIBRARIES=libcxxsql.la
libcxxsql_la_SOURCES=\
	connection.C \
	env.C \
	exception.C \
	gettext_in.h \
	sql_internal.H

libcxxsql_la_LIBADD=-lcxx @ODBC_LIBS@
libcxxsql_la_LDFLAGS=-version-info @VERSION_INFO@ $(LINKTYPE)

noinst_PROGRAMS=testenv

testenv_SOURCES=testenv.C
testenv_LDADD=libcxxsql.la
testenv_LDFLAGS=$(LINKTYPE)

