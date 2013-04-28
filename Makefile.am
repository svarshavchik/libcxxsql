#
# Copyright 2013 Double Precision, Inc.
# See COPYING for distribution information.
#

AUTOMAKE_OPTIONS=dist-bzip2 -Wno-portability
SUBDIRS= po includes/sql sql

define reverse
$(if $(1), $(call reverse,$(wordlist 2,$(words $(1)),$(1))) $(firstword $(1)))
endef

$(shell :)DIST_SUBDIRS=$(call reverse,$(SUBDIRS))

DISTCHECK_CONFIGURE_FLAGS=--with-pkgconfigdir='$$(prefix)/local/pkgconf'
EXTRA_DIST=config.rpath m4/ChangeLog config.rpath \
	COPYING \
	packaging/fedora/libcxxsql.spec

.PHONY: rpm

rpm:
	make dist
	rm -rf rpm/SRPMS/*
	rm -rf rpm/RPMS/*
	rm -rf rpm/BUILD/*
	rm -rf rpm/BUILDROOT/*
	rpmbuild -ta --clean \
		--define "_topdir `pwd`/rpm" \
		--define '_rpmdir %{_topdir}/RPMS' \
		--define '_srcrpmdir %{_topdir}/SRPMS' \
		--define '_sourcedir %{_topdir}/SOURCES' \
		--define '_specdir %{_topdir}/SPECS' \
		--define '_builddir %{_topdir}/BUILD' \
		--define '_build_name_fmt %%{ARCH}/%%{NAME}-%%{VERSION}-%%{RELEASE}.%%{ARCH}.rpm' \
		--define '_tmppath %{_var}/tmp' \
		--define '__spec_prep_pre %{___build_pre}' \
		--define "extrarelease .`date '+%Y%m%d%H%M%S'`" @PACKAGE@-@VERSION@.tar.bz2
	sh $$HOME/repos.sh

pkgconfdir=@PKGCONFIGDIR@

pkgconf_DATA=libcxxsql.pc

libcxxsql.pc: Makefile
	echo "prefix=${prefix}" >libcxxsql.pc.tmp
	echo "exec_prefix=${exec_prefix}" >>libcxxsql.pc.tmp
	echo "datadir=${datadir}" >>libcxxsql.pc.tmp
	echo "pkgdatadir=${pkgdatadir}" >>libcxxsql.pc.tmp
	echo "libdir=${libdir}" >>libcxxsql.pc.tmp
	echo "includedir=${includedir}" >>libcxxsql.pc.tmp
	echo "" >>libcxxsql.pc.tmp
	echo "Name: LibCXX SQL Library" >>libcxxsql.pc.tmp
	echo "Description: LibCXX SQL Library" >>libcxxsql.pc.tmp
	echo "Version: @VERSION@" >>libcxxsql.pc.tmp
	echo "Requires: libcxx" >>libcxxsql.pc.tmp
	mv -f libcxxsql.pc.tmp libcxxsql.pc

CLEANFILES=libcxxsql.pc

ACLOCAL_AMFLAGS = -I m4

www: html
	rsync -a --delete-after html/. ~/www/www.courier-mta.org/libcxx/sql/.

tag:
	git tag -s -a @PACKAGE@/@VERSION@/`date +%Y%m%d%H%M%S` -m 'Tag version @VERSION@'

scp:
	rm -f @PACKAGE@-@VERSION@.tar.bz2.sig; gpg --detach-sign --default-key mrsam@courier-mta.com @PACKAGE@-@VERSION@.tar.bz2; test -f @PACKAGE@-@VERSION@.tar.bz2.sig || exit 1
	scp @PACKAGE@-@VERSION@.tar.bz2 @PACKAGE@-@VERSION@.tar.bz2.sig fedorahosted.org:libcxx
