/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_envfwd_H
#define x_sql_envfwd_H

#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {

	namespace sql {
#if 0
	};
};
#endif

class envObj;
class envBase;

//! An SQL environment handle

//! This is a \ref ref "reference to a reference-counted object" that represents
//! an SQL environment.
//!
//! Constructing a new environment object initializes the default SQL
//! environment.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::sql h=INSERT_LIBX_NAMESPACE::sql::env::create();
//!
//! std::map<std::string, std::string> sources;
//!
//! h->get_data_sources(sources);
//! \endcode
//!
//! get_data_sources() returns a list of available databases that are
//! available from the ODBC connection manager. get_data_sources() populates
//! a map keyed by source name, with the value being the description of that
//! database source.

typedef ref<envObj, envBase> env;

//! A nullable pointer reference to an \ref env "SQL environment".

typedef ptr<envObj, envBase> envptr;

//! A reference to a constant \ref env "SQL environment".

typedef const_ref<envObj, envBase> const_env;

//! A nullable pointer reference to a constant \ref env "SQL environment".

typedef const_ptr<envObj, envBase> const_envptr;

#if 0
{
	{
#endif
	}
}
#endif
