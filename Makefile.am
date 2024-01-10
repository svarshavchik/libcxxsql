#
# Copyright 2013 Double Precision, Inc.
# See COPYING for distribution information.
#

AUTOMAKE_OPTIONS=dist-bzip2 -Wno-portability
SUBDIRS= po includes sql

define reverse
$(if $(1), $(call reverse,$(wordlist 2,$(words $(1)),$(1))) $(firstword $(1)))
endef

$(shell :)DIST_SUBDIRS=$(call reverse,$(SUBDIRS))

DISTCHECK_CONFIGURE_FLAGS=--with-pkgconfigdir='$$(prefix)/local/pkgconf'
EXTRA_DIST=config.rpath DoxygenLayout.xml \
	COPYING \
	debuild \
	packaging/fedora/libcxxsql.spec \
	packaging/debian/changelog \
	packaging/debian/control \
	packaging/debian/copyright \
	packaging/debian/lintian-overrides.allow-errors \
	$(patsubst $(srcdir)/%,%,$(wildcard $(srcdir)/packaging/debian/*.lintian-overrides)) \
	packaging/debian/rules \
	packaging/debian/source/format

DOCS_SRC = $(shell find includes -name '*.H' -print)

docs: html/index.html html/book.css html/logo.png html/frame.js html/icon.gif $(patsubst %,html/%,$(notdir $(wildcard $(srcdir)/docbook/*.png)))
	chcon -R -t httpd_sys_content_t html 2>/dev/null || true
	test -d ~/www/hostrocket || exit 0; set -e; \
		rm -rf ~/www/hostrocket/libcxx.org/sql; \
		rsync -a -X html/. ~/www/hostrocket/libcxx.org/sql

html/%.png: html/index.html docbook/%.png
	$(INSTALL) -m 0644 $(srcdir)/docbook/$(notdir $@) html

html/book.css: html/index.html docbook/book.css
	$(INSTALL) -m 0644 $(srcdir)/docbook/book.css html/book.css

html/frame.js: html/index.html docbook/frame.js
	$(INSTALL) -m 0644 $(srcdir)/docbook/frame.js html/frame.js

html/icon.gif: html/index.html docbook/icon.gif
	$(INSTALL) -m 0644 $(srcdir)/docbook/icon.gif html/icon.gif

html/index.html: html/ref/index.html $(wildcard docbook/*.xml) docbook/xmllinks.xsl
	mkdir -p docbook
	xsltproc $(srcdir)/docbook/xmllinks.xsl xml/index.xml >docbook/entities
	rm -rf html.tmp
	mkdir -p html.tmp
	xsltproc --nonet -o html.tmp/ --xinclude \
		--stringparam use.id.as.filename 1 \
		--stringparam html.stylesheet book.css \
		--stringparam root.filename 'toc' \
		--stringparam generate.id.attributes 1 \
		--param local.l10n.xml 'document("'`pwd`/docbook/l10.xml'")' \
		$(srcdir)/docbook/docbook.xsl $(srcdir)/docbook/book.xml
	elinks -dump -no-numbering -no-references html.tmp/INSTALL.html >INSTALL.tmp
	mv -f INSTALL.tmp INSTALL
	mkdir html.tmp/ref
	ln html/ref/* html.tmp/ref
	rm -rf html
	mv html.tmp html

html/ref/index.html: $(DOCS_SRC) Doxyfile header.html
	rm -rf html/ref
	mkdir -p html/ref
	doxygen; c=$$?;	perl -p -i -e 's/INSERT_LIBX_NAMESPACE/x/g' html/ref/*.html; exit $$c

header.html: Doxyfile
	rm -rf header.html.tmp
	mkdir -p header.html.tmp
	cd header.html.tmp && doxygen -w html header.html footer.html doxygen.css
	perl -p -i -e 's@<head>@<head><link rel="icon" href="/icon.gif" type="image/gif"/>@;s@\$$projectbrief@<a href="/">\$$projectbrief</a>@;' header.html.tmp/header.html
	mv -f header.html.tmp/header.html .
	rm -rf header.html.tmp

.PHONY: rpm

dist-hook:
	chmod +x $(distdir)/debuild

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
	sh $$HOME/bin/rpmrepos.sh

.PHONY: deb

deb: dist
	rm -rf deb
	mkdir deb
	cp libcxxsql-@VERSION@.tar.bz2 deb
	cd deb; set -e; tar xf libcxxsql-@VERSION@.tar.bz2; \
	    cd libcxxsql-@VERSION@; \
                RELEASE=`date '+%Y%m%d%H%M%S'` \
		./debuild -us -uc
	rm -rf deb.new; mv deb/*/deb deb.new
	rm -rf deb; mv deb.new deb
	$$HOME/bin/debrepos.sh

pkgconfdir=@PKGCONFIGDIR@

pkgconf_DATA=libcxxsql-@ABIVER@.@LIBCXX_VERSION@.pc

libcxxsql-@ABIVER@.@LIBCXX_VERSION@.pc: Makefile
	echo "prefix=${prefix}" >libcxxsql.pc.tmp
	echo "exec_prefix=${exec_prefix}" >>libcxxsql.pc.tmp
	echo "datadir=${datadir}" >>libcxxsql.pc.tmp
	echo "pkgdatadir=${pkgdatadir}-@ABIVER@-@LIBCXX_VERSION@" >>libcxxsql.pc.tmp
	echo "libdir=${libdir}/libcxxsql-@ABIVER@-@LIBCXX_VERSION@" >>libcxxsql.pc.tmp
	echo "includedir=${includedir}" >>libcxxsql.pc.tmp
	echo "" >>libcxxsql.pc.tmp
	echo "Name: LibCXX SQL Library" >>libcxxsql.pc.tmp
	echo "Description: LibCXX SQL Library" >>libcxxsql.pc.tmp
	echo "Version: @VERSION@.@LIBCXX_VERSION@" >>libcxxsql.pc.tmp
	echo "Cflags: -I$(includedir)/libcxxsql-@ABIVER@-@LIBCXX_VERSION@" >>libcxxsql.pc.tmp
	echo 'Libs: -L${libdir}/libcxxsql-@ABIVER@-@LIBCXX_VERSION@ -Wl,-rpath -Wl,${libdir}/libcxxsql-@ABIVER@-@LIBCXX_VERSION@' >>libcxxsql.pc.tmp
	mv -f libcxxsql.pc.tmp libcxxsql-@ABIVER@.@LIBCXX_VERSION@.pc

aclocaldir=@datadir@/aclocal
aclocal_DATA=libcxxsql-@VERSION@-@LIBCXX_VERSION@.m4

libcxxsql-@VERSION@-@LIBCXX_VERSION@.m4: libcxxsql.m4
	rm -f libcxxsql-@VERSION@-@LIBCXX_VERSION@.m4
	ln libcxxsql.m4 libcxxsql-@VERSION@-@LIBCXX_VERSION@.m4

CLEANFILES=$(aclocal_DATA) $(pkgconf_DATA)

ACLOCAL_AMFLAGS = -I m4

upload:
	git tag -s -a @PACKAGE@/@VERSION@/`date +%Y%m%d%H%M%S` -m 'Tag version @VERSION@'
	rm -f @PACKAGE@-@VERSION@.tar.bz2.sig; gpg --detach-sign --default-key mrsam@courier-mta.com @PACKAGE@-@VERSION@.tar.bz2; test -f @PACKAGE@-@VERSION@.tar.bz2.sig || exit 1
	pag upload @PACKAGE@-@VERSION@.tar.bz2
	pag upload @PACKAGE@-@VERSION@.tar.bz2.sig
