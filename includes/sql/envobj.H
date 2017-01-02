/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_envobj_H
#define x_sql_envobj_H

#include <x/obj.H>
#include <x/sql/connectionfwd.H>
#include <x/namespace.h>
#include <map>

namespace LIBCXX_NAMESPACE {

	//! Access SQL sources via unixODBC

	//! This namespace provides classes for accessing any SQL database
	//! supported by unixODBC.
	//!
	//! Constructing an
	//! \ref explicit_env "INSERT_LIBX_NAMESPACE::sql::env" initializes the default
	//! SQL database connection environment.

	namespace sql {
#if 0
	};
};
#endif

//! Flags for \ref explicit_env "env->connect()".

enum class connect_flags {
	//! Prompt for connection parameters
	prompt=0,

	//! Prompt if connection parameters are incomplete, else connect.
	complete=1,

        //! Prompt if connection parameters are incomplete, and only for necessary minimum parameters
	complete_required=2,

	//! Do not prompt, abort, if connection parameters are incomplete.
	noprompt=3,
		};

//! SQL environment handle

class envObj : virtual public obj {

public:
	//! Constructor
	envObj();

	//! Destructor
	~envObj();

	//! Return available data sources
	virtual void get_data_sources(std::map<std::string, std::string>
				      &sources) const=0;

	//! Return available drivers
	virtual void get_drivers(std::map<std::string, std::string>
				 &drivers) const=0;

	//! Set login timeout
	virtual void set_login_timeout(time_t)=0;

	//! Clear login timeout
	virtual void clear_login_timeout()=0;

	//! Create a connection

	//! The connection parameter is given as a single string.
	//! Connection flags default to \c noprompt.

	std::pair<connection, std::string>
	connect(const std::string &connection_parameters);

	//! Create a connection

	//! \overload
	//! The connection parameter is given as a single string, with
	//! explicit connection flags.

	virtual std::pair<connection, std::string>
	connect(const std::string &connection_parameters,
		connect_flags flags)=0;

	//! Specify connection parameters via a list

	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::sql::env::base::arglist_t;

	typedef std::list<std::pair<std::string, std::string>> arglist_t;

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
	connect(const arglist_t &args, Args ...more_args);

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
	connect(arglist_t &&args, Args ...more_args);

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
	connect(const std::string &argname,
		const std::string &argvalue, Args ...more_args);

private:

	//! End of variadic connect() parameter list

	//! \internal
	//! Default connection flag of \c noprompt.

	void process_connect_args(arglist_t &args,
				  connect_flags &flags)
	{
	}

	//! End of variadic connect() parameter list

	//! \internal
	//! Explicit connection flags are given.

	void process_connect_args(arglist_t &args,
				  connect_flags &flags,
				  connect_flags explicit_flags)
	{
		flags=explicit_flags;
	}

	//! Variadic parameter: explicit keyword name/value pair.

	//! \internal
	template<typename ...Args>
	void process_connect_args(arglist_t &args,
				  connect_flags &flags,
				  const std::string &argname,
				  const std::string &argvalue,
				  Args ...more_args)
	{
		args.push_back(std::make_pair(argname, argvalue));

		process_connect_args(args, flags,
				     std::forward<Args>(more_args)...);
	}

	//! Variadic parameter: a list of parameter name/value pairs.

	//! \internal
	template<typename ...Args>
	void process_connect_args(arglist_t &args,
				  connect_flags &flags,
				  const arglist_t &moreArgs,
				  Args ...more_args)
	{
		args.insert(args.end(), moreArgs.begin(), moreArgs.end());

		process_connect_args(args, flags,
				     std::forward<Args>(more_args)...);
	}

	//! Process accumulated parameter list as a connection string.

	//! \internal
	std::pair<connection, std::string>
	connect(const arglist_t &args,
		connect_flags flags);

};

#if 0
{
	{
#endif
	}
}
#endif
