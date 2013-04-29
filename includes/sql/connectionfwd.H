/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_connectionfwd_H
#define x_sql_connectionfwd_H

#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {

	namespace sql {
#if 0
	};
};
#endif

class connectionObj;
class connectionBase;

//! A connection to an SQL database

//! This is a \ref ref "reference to a reference-counted object" that represents
//! an SQL connection. It gets created by invoking connect() on an
//! \ref env "environment handle".

typedef ref<connectionObj, connectionBase> connection;

//! A nullable pointer reference to an \ref connection "SQL connection".

typedef ptr<connectionObj, connectionBase> connectionptr;

//! A reference to a constant \ref connection "SQL connection".

typedef const_ref<connectionObj, connectionBase> const_connection;

//! A nullable pointer reference to a constant \ref connection "SQL connection".

typedef const_ptr<connectionObj, connectionBase> const_connectionptr;

#if 0
{
	{
#endif
	}
}
#endif
