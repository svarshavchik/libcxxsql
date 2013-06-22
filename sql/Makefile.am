#
# Copyright 2013 Double Precision, Inc.
# See COPYING for distribution information.
#

@LIBCXX_AM@

include $(shell pwd)/../includes/libcxxsql.mk

AM_CPPFLAGS=-I . -I $(srcdir) -I $(srcdir)/../includes -I ../includes @ODBC_CFLAGS@

BUILT_SOURCES=x
CLEANFILES=x

LINKTYPE=

x:
	ln -s $(srcdir)/../includes x

lib_LTLIBRARIES=libcxxsql.la
libcxxsql_la_SOURCES=\
	connection.C \
	dbi_constraint.C \
	dbi_flavor.C \
	dbi_resultset.C \
	env.C \
	exception.C \
	gettext_in.h \
	fetchblob.C \
	insertblob.C \
	newstatement.C \
	statement.C \
	sql_internal.H \
	types.C \
	getinfo_internal.H \
	sql_internal_type_list.H

libcxxsql_la_LIBADD=-lcxx @ODBC_LIBS@
libcxxsql_la_LDFLAGS=-version-info @VERSION_INFO@ $(LINKTYPE)

noinst_PROGRAMS=testenv testcursor testdbi testschema

testcursor_SOURCES=testcursor.C
testcursor_LDADD=libcxxsql.la
testcursor_LDFLAGS=$(LINKTYPE)

testenv_SOURCES=testenv.C
testenv_LDADD=libcxxsql.la
testenv_LDFLAGS=$(LINKTYPE)

testdbi_SOURCES=testdbi.C
testdbi_LDADD=libcxxsql.la
testdbi_LDFLAGS=$(LINKTYPE)

$(call SCHEMA_GEN,testschematbl)

$(call SCHEMA_GEN,exampleschema1)

$(call SCHEMA_GEN,exampleschema2)

testschema_SOURCES=testschema.C
testschema_LDADD=libcxxsql.la
testschema_LDFLAGS=$(LINKTYPE)

check-am: $(noinst_PROGRAMS)
	./testenv
	test -f private.mysql || exit 0; ./testenv --connect "`cat private.mysql`"
	test -f private.postgres || exit 0; ./testenv --connect "`cat private.postgres`"
	test -f private.mysql || exit 0; ./testcursor --connect "`cat private.mysql`" --skip-fetch-vector-blobs
	test -f private.postgres || exit 0; ./testcursor --connect "`cat private.postgres`"
	test -f private.mysql || exit 0; ./testdbi --connect "`cat private.mysql`"
	test -f private.postgres || exit 0; ./testdbi --connect "`cat private.postgres`"
	test -f private.mysql || exit 0; ./testschema --connect "`cat private.mysql`"
	test -f private.postgres || exit 0; ./testschema --connect "`cat private.postgres`"
