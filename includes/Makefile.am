#
# Copyright 2013 Double Precision, Inc.
# See COPYING for distribution information.
#

SUBDIRS = sql

configdir=$(datadir)/libcxx
config_DATA=buildschema.xsl libcxxsql.mk 

dist-hook:
	rm -f $(patsubst %,$(distdir)/%,$(BUILT_SOURCES))

EXTRA_HEADERS=
