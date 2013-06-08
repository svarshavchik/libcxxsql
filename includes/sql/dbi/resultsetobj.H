/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_dbi_resultsetobj_H
#define x_sql_dbi_resultsetobj_H

#include <map>
#include <x/obj.H>
#include <x/sql/connectionfwd.H>
#include <x/sql/connectionobj.H>

namespace LIBCXX_NAMESPACE {
	namespace sql {
		namespace dbi {
#if 0
		};
	};
};
#endif

//! An SQL resultset.

class resultsetObj : virtual public obj {

protected:
	//! Underlying connection
	connection conn;

public:
	//! A container for keeping track of all tables in a join.

	//! This is used in order to keep track of every table that's involved
	//! in a join, so that if the table is joined a second time, we pick
	//! a new alias for it.

	class aliasesObj : virtual public obj {

		//! Count each table's appearance in a join.
		std::map<std::string, size_t> counter;

	public:
		//! Constructor
		aliasesObj();

		//! Destructor
		~aliasesObj() noexcept;

		//! Get the alias for a new table in a join.

		//! We take the table's name, and return its alias. The first
		//! time we see the table, the alias is the same. The next
		//! time it's [table]_2, and so on.
		//!
		//! Now, in case someone is foolish enough to have a real
		//! table named 'foo_2', what we do is strip off any trailing
		//! digits and underscores, when taking the table's name,
		//! and treat that as the real table.
		std::string get_alias(const std::string &table_name);
	};

protected:
	//! Keep track of all tables in a join
	ref<aliasesObj> aliases;

public:
	//! Constructor
	resultsetObj(//! The connection object
		     const connection &connArg,

		     //! By default, for a new resultset, we create a new
		     //! aliases table. When a new join gets added, we pass
		     //! the existing aliases object
		     const ref<aliasesObj> &aliasesArg=ref<aliasesObj>::create());

	//! Destructor
	~resultsetObj() noexcept;

	//! Return the name of the underlying table
	virtual const char *get_table_name() const=0;

	//! Return the table's alias in the query
	virtual std::string get_table_alias() const=0;

private:
	//! Column names in the table, null terminated list
	virtual const char * const *get_table_columns() const=0;
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
