/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_connection_H
#define x_sql_connection_H

#include <x/sql/connectionobj.H>
#include <x/sql/connectionfwd.H>
#include <x/sql/env.H>
#include <x/ref.H>

namespace LIBCXX_NAMESPACE {
	namespace sql {
#if 0
	};
};
#endif

//! Base class for \ref env "environment handles".

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::sql::env::base.

class connectionBase : public ptrrefBase {

public:

};


//! Create a connection

//! \overload
//! The connection parameter is given as a single string, with
//! explicit connection flags.
//!
//! Variadic function, first parameter is a list of std::pairs
//! of parameter name/parameter value strings.
//!
//! The remaining parameters contain a mixture of: any additional
//! list, and an explicit name/value parameter pair.
//! The list gets optionally terminated by an explicit connection
//! flag, which otherwise defaults to \c noprompt.

template<typename ...Args>
std::pair<connection, std::string>
envObj::connect(const arglist_t &args, Args ...more_args)
{
	arglist_t argsCpy(args);
	connect_flags flags=connect_flags::noprompt;

	process_connect_args(argsCpy,
			     flags, std::forward<Args>(more_args)...);

	return connect(argsCpy, flags);
}

//! Create a connection

//! \overload
//! The connection parameter is given as a single string, with
//! explicit connection flags.
//!
//! Variadic function, first parameter is an rvalue list of std::pairs
//! of parameter name/parameter value strings.
//!
//! The remaining parameters contain a mixture of: any additional
//! list, and an explicit name/value parameter pair.
//! The list gets optionally terminated by an explicit connection
//! flag, which otherwise defaults to \c noprompt.

template<typename ...Args>
std::pair<connection, std::string>
envObj::connect(arglist_t &&args, Args ...more_args)
{
	arglist_t argsCpy(std::move(args));
	connect_flags flags=connect_flags::noprompt;

	process_connect_args(argsCpy,
			     flags,
			     std::forward<Args>(more_args)...);
	return connect(argsCpy, flags);
}

//! Create a connection

//! \overload
//! The connection parameter is given as a single string, with
//! explicit connection flags.
//!
//! Variadic function, first two parameters are explicit parameter
//! name/value pair.
//!
//! the remaining parameters contain a mixture of: a list of std::pairs
//! of parameter name/parameter value strings, or more
//! explicit name/value parameter pairs.
//! The list gets optionally terminated by an explicit connection
//! flag, which otherwise defaults to \c noprompt.

template<typename ...Args>
std::pair<connection, std::string>
envObj::connect(const std::string &argname,
		const std::string &argvalue, Args ...more_args)
{
	arglist_t argsCpy;
	connect_flags flags=connect_flags::noprompt;

	process_connect_args(argsCpy, flags, argname, argvalue,
			     std::forward<Args>(more_args)...);
	return connect(argsCpy, flags);
}

#if 0
{
	{
#endif
	}
}
#endif
