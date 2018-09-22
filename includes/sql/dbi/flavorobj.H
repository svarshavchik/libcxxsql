/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_dbi_flavorobj_H
#define x_sql_dbi_flavorobj_H

#include <x/obj.H>
#include <x/sql/dbi/flavorfwd.H>
#include <x/sql/dbi/constraintfwd.H>
#include <x/sql/dbi/resultsetfwd.H>

#include <iostream>
#include <list>
#include <vector>
#include <set>

namespace LIBCXX_NAMESPACE {
	namespace sql {
		namespace dbi {
#if 0
		};
	};
};
#endif

//! Select DBI SQL flavor

class flavorObj : virtual public obj {

public:
	//! Constructor
	flavorObj() LIBCXX_HIDDEN ;

	//! Destructor
	~flavorObj() LIBCXX_HIDDEN ;

	//! Build an UPDATE with joins

	virtual void update_with_joins(//! Format SQL here
				       std::ostream &o,

				       //! The UPDATE resultset
				       const resultsetObj &rs,

				       //! Fields to update
				       std::vector<std::string> &fields,

				       //! Placeholder for values
				       std::vector<std::string> &placeholders)
		const LIBCXX_HIDDEN =0;

	//! The datatype for auto_increment/serial field
	virtual const char *datatype_serial() LIBCXX_HIDDEN=0;

	//! The default value for auto_increment/serial field

	virtual const char *default_value_serial() LIBCXX_HIDDEN=0;

	//! Create constraint to return the value of just-inserted serial value(s)

	virtual constraint get_inserted_serial(//! INSERT table name
					       const char *table_name,

					       //! Serial key column(s)
					       const std::set<std::string> &columns)
		LIBCXX_HIDDEN=0;
};

#if 0
{
	{
		{
#endif
		};
	};
};
#endif
