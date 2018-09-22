#
# Copyright 2013-2018 Double Precision, Inc.
# See COPYING for distribution information.
#

SUBDIRS = sql

configdir=$(datadir)/libcxxsql-@ABIVER@-@LIBCXX_VERSION@
config_DATA=buildschema.xsl libcxxsql.mk

dist-hook:
	rm -f $(patsubst %,$(distdir)/%,$(BUILT_SOURCES))

EXTRA_HEADERS=
