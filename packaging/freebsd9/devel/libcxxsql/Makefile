# New ports collection makefile for: libcxx
# Date created:		June 1, 2013
# Whom:                 Sam Varshavchik <mrsam@courier-mta.com>
# $FreeBSD$

PORTNAME=		libcxxsql
PORTVERSION=		0.4
CATEGORIES=		devel

MAINTAINER=		mrsam@courier-mta.com
COMMENT=		LibCXX SQL library

LIB_DEPENDS=		cxx:${PORTSDIR}/devel/libcxx \
			gmpxx:${PORTSDIR}/math/gmp \
			odbc:${PORTSDIR}/databases/unixODBC

BUILD_DEPENDS=          gmp>=5.1:${PORTSDIR}/math/gmp

USE_BZIP2=1
USE_GMAKE=yes
GNU_CONFIGURE=1
USE_PKGCONFIG=both

.include <bsd.port.mk>
