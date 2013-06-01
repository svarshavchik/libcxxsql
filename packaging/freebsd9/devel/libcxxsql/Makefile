# New ports collection makefile for: libcxx
# Date created:		June 1, 2013
# Whom:                 Sam Varshavchik <mrsam@courier-mta.com>
# $FreeBSD$

PORTNAME=		libcxxsql
PORTVERSION=		0.1
CATEGORIES=		devel

MAINTAINER=		mrsam@courier-mta.com
COMMENT=		LibCXX SQL library

LIB_DEPENDS=		cxx:${PORTSDIR}/devel/libcxx \
			odbc:${PORTSDIR}/databases/unixODBC


USE_BZIP2=1
USE_GCC=4.7+
USE_GMAKE=yes
GNU_CONFIGURE=1
USE_PKGCONFIG=both

.include <bsd.port.mk>
