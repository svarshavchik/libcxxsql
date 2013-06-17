/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_dbi_resultsetobj_H
#define x_sql_dbi_resultsetobj_H

#include <map>
#include <vector>
#include <iostream>
#include <x/obj.H>
#include <x/mpobj.H>
#include <x/sql/connectionfwd.H>
#include <x/sql/connectionobj.H>
#include <x/sql/statementobj.H>
#include <x/sql/dbi/resultsetfwd.H>
#include <x/sql/dbi/constraintfwd.H>
#include <x/sql/dbi/constraintobj.H>

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

	//! Set by limit()

	size_t maxrows;

public:
	//! Throws an exception, used by maybe() when multiple rows are returned.
	static void multiple_rows() __attribute__((noreturn));

	//! Throws an exception, used by only() when no row is returnd.
	static void no_rows() __attribute__((noreturn));

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

	//! Current constraint. The top level constraint node is an AND clause.

	ref<constraintObj::andObj> where;

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

	//! Set the maximum number of rows in the resultset
	void limit(size_t maxrowsArg)
	{
		maxrows=maxrowsArg;
	}
	class rowObj;

	//! Constraint the resultset

	template<typename ...Args>
	inline void search(Args && ...args);

protected:
	//! Called by begin() in subclass to prepare and execute a SELECT

	statement execute_search_sql(size_t limitvalue,
				     const std::vector<std::string> &columns)
		const;

	//! Joins added to this resultset

	//! For each added join we record:
	//!
	//! - the SQL for that join
	//! - the join object for that join, from which we obtain secondary
	//! join.

	std::list<std::pair<std::string, ref<joinBaseObj>>> joinlist;

	//! Add a join to the resultset

	void addjoin(const char *jointype, const ref<joinBaseObj> &join,
		     std::initializer_list<const char *> &&columns);

	//! Add joins to this resultset

	void get_join_sql(std::ostream &o) const;
};

//! A row returned by a resultset

class resultsetObj::rowObj : virtual public obj {

protected:
	//! The underlying connection

	connection conn;

public:
	//! Constructor

	rowObj(const connection &connArg);

	//! Destructor
	~rowObj() noexcept;

	//! Template class used by a subclass to declare a field

	//! The template parameter is the datatype for the field's value.

	template<typename field_type> class field;

	//! Template class used by a subclass to cache a joined row
	template<typename row_type> class cached_row_container {
	public:
		//! The cached row pointer.
		row_type row;

		//! Whether the row has been cached
		bool cached;

		cached_row_container() : cached(false) {}
	};

	//! A mutex-protected cached joined row

	template<typename row_type>
	using cached_row=mpobj<cached_row_container<row_type>>;
};


//! A field in a row that does not have a NULL indicator.

template<typename field_type>
class resultsetObj::rowObj::field {

public:
	//! The current value
	std::pair<field_type, bitflag> scalar;

	//! The original value
	std::pair<field_type, bitflag> original;

	friend class rowObj;

	//! Constructor
	field() : scalar(std::make_pair(field_type(), 1)), original(scalar)
	{
	}

	//! Getter
	const field_type &value() const { return scalar.first; }

	//! Whether it's null, it cannot be
	bool isnull() const { return scalar.second; }

	//! Setter
	void value(const field_type &newValue)
	{
		scalar.first=newValue;
		scalar.second=0;
	}

	//! Setter
	void value(field_type &&newValue)
	{
		scalar.first=std::move(newValue);
		scalar.second=0;
	}

	//! Set to a null value
	void value(std::nullptr_t dummy)
	{
		scalar.second=1;
	}

	//! Whether this field has been updated.

	bool updated() const
	{
		return scalar.second != original.second ||
			scalar.first != original.first;
	}
};

//! Base class for resultset subclass's join implementors

class joinBaseObj : virtual public obj {

public:

	//! Constructor

	joinBaseObj();

	//! Destructor
	~joinBaseObj() noexcept;

        //! Return the real table's name
	virtual const char *get_table_name() const=0;

	//! Return the table's alias in the query
	virtual std::string get_table_alias() const=0;

	//! Column names in this table, null terminated list
	virtual const char * const *get_table_columns() const=0;

	//! Assemble aliased column list

	//! Calls get_table_columns() and get_table_alias(), creates "alias.name" for each
	//! column, appends it to the given column list

	void create_columns_list(std::vector<std::string> &list) const;

	//! Building joins for the resultset
	virtual void get_join_sql_recursive(std::ostream &o) const=0;

};

#if 0
{
#endif
};

template<typename ...Args>
inline void statementObj::bind_all(bindrow &resultset_row,
				   Args && ...args)
{
	bind_factory factory(*this);

	resultset_row.bind(factory);

	bind_all(std::forward<Args>(args)...);
}

#if 0
{
	{
#endif
	};
};
#endif
