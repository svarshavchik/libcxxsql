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

namespace LIBCXX_NAMESPACE {
	namespace sql {
		namespace dbi {
#if 0
		};
	};
};
#endif

//! Select DBI SQL flavor

class LIBCXX_HIDDEN flavorObj : virtual public obj {

public:
	//! Constructor
	flavorObj();

	//! Destructor
	~flavorObj() noexcept;

	//! Build an UPDATE with joins

	virtual void update_with_joins(//! Format SQL here
				       std::ostream &o,

				       //! The UPDATE resultset
				       const resultsetObj &rs,

				       //! Fields to update
				       std::vector<std::string> &fields,

				       //! Placeholder for values
				       std::vector<std::string> &placeholders)
		const=0;
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
