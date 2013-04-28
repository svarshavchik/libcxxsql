/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_envobj_H
#define x_sql_envobj_H

#include <x/obj.H>
#include <map>

namespace LIBCXX_NAMESPACE {

	//! Access SQL sources via unixODBC

	//! This namespace provides classes for accessing any SQL database
	//! supported by unixODBC.
	//!
	//! Constructing an
	//! \ref env "INSERT_LIBX_NAMESPACE::sql::env" initializes the default
	//! SQL database connection environment.

	namespace sql {
#if 0
	};
};
#endif

//! SQL environment handle

class envObj : virtual public obj {

public:
	//! Constructor
	envObj();

	//! Destructor
	~envObj() noexcept;

	//! Return available data sources
	virtual void get_data_sources(std::map<std::string, std::string>
				      &sources) const=0;

};

#if 0
{
	{
#endif
	}
}
#endif
