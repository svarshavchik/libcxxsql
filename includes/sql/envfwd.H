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

//! \anchor explicit_env An SQL environment handle.

//! This is a \ref ref "reference to a reference-counted object" that represents
//! an SQL environment.
//!
//! Constructing a new environment object initializes the default SQL
//! environment.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::sql::env envh=INSERT_LIBX_NAMESPACE::sql::env::create();
//!
//! std::map<std::string, std::string> sources;
//!
//! envh->get_data_sources(sources);
//!
//! std::map<std::string, std::string> drivers;
//!
//! envh->get_data_sources(drivers);
//! \endcode
//!
//! get_data_sources() returns a list of available databases that are
//! available from the ODBC connection manager. get_data_sources() populates
//! a map keyed by source name, with the value being the description of that
//! database source.
//!
//! get_drivers() returns a list of available ODBC drivers.
//! get_drivers() populates a map keyed by source name, with the value being
//! the description of that driver.
//!
//! \code
//! std::pair<INSERT_LIBX_NAMESPACE::sql::connection, std::string> res=env->connect("DSN=dev;UID=citizenkane");
//! \endcode
//!
//! connect() takes a connection specification, connects to the specified
//! database, and returns an
//! \ref sql::connection "INSERT_LIBX_NAMESPACE::sql::connection", a
//! reference-counted object representing the connection to the database,
//! and a fully-qualified connection string, which consists of the specified
//! connection string, plus any optional connection parameters with their
//! default values, so that using the fully-qualified connection string to
//! establish another database connection will create another connection
//! using the same exact connection parameters.
//!
//! connect() takes an optional second parameter that's one of the following
//! values:
//!
//! - \c INSERT_LIBX_NAMESPACE::sql::connect_flags::noprompt: if the
//! connection string is invalid or incomplete, the connection attempt fails,
//! this is the default value.
//!
//! - \c INSERT_LIBX_NAMESPACE::sql::connect_flags::complete: if the connection
//! string contains all required connection parameters, proceed and make
//! a connection attempt using default values of all unspecified, optional
//! parameters. Otherwise prompt for all unspecified connection parameters,
//! required an optional ones.
//!
//! - \c INSERT_LIBX_NAMESPACE::sql::connect_flags::complete_required:
//! if the connection string contains all required connection parameters,
//! proceed and make a connection attempt using default values of all
//! unspecified, optional parameters. Otherwise prompt for all unspecified
//! connection parameters, but only the required ones.
//!
//! - \c INSERT_LIBX_NAMESPACE::sql::connect_flags::prompt:
//! prompt for connection parameters, using the ones given in the connection
//! string, if any, as their default values.
//!
//! A failed connection results in a thrown exception. Prompting, if necessary
//! is carried out by loading the prompting library given in the
//! \c INSERT_LIBX_NAMESPACE::sql::uiprompt
//! \ref explicit_property_namespace "application property".
//!
//! \code
//! std::pair<INSERT_LIBX_NAMESPACE::sql::connection, std::string> res=env->connect("DSN", "dev", "UID", "citizenkane", INSERT_LIBX_NAMESPACE::sql::connect_flags::prompt);
//! \endcode
//!
//! Connection parameters can also be specified as a discrete list of
//! connection parameter name and value tuples, ending with an optional
//! \c INSERT_LIBX_NAMESPACE::sql::connect_flags value.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::sql::env::base::arglist_t args;
//! 
//! args.push_back({"DSN", "dev"});
//! args.push_back({"UID", "citizenkane"});
//!
//! std::pair<INSERT_LIBX_NAMESPACE::sql::connection, std::string> res=env->connect(args, INSERT_LIBX_NAMESPACE::sql::connect_flags::prompt);
//! \endcode
//!
//! Discrete parameter values can also come from an
//! \ref envObj::arglist_t "INSERT_LIBX_NAMESPACE::sql::env::base::arglist_t"
//! (which is nothing more than a \c std::list of a
//! \c std::string pair.
//!
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
