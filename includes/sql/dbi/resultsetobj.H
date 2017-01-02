/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_dbi_resultsetobj_H
#define x_sql_dbi_resultsetobj_H

#include <map>
#include <vector>
#include <list>
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
		~aliasesObj();

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

	//! The HAVING constraint. The top level constraint node is an AND clause.
	ref<constraintObj::andObj> having_constraint;

	//! The GROUP BY
	std::list<std::string> group_by_list;

	//! The ORDER BY
	std::list<std::string> order_by_list;

	//! Bind the resultset's row, and any prefetched rows

	class bindrow_all : public bindrow {

	public:
		//! Rows to bind
		std::vector<bindrowimpl> rows;

		//! Invoke bind() in all rows
		void bind(bind_factory &factory) override;

		//! Constructor
		bindrow_all();

		//! Destructor
		~bindrow_all();
	};

public:
	//! Constructor
	resultsetObj(//! The connection object
		     const connection &connArg,

		     //! By default, for a new resultset, we create a new
		     //! aliases table. When a new join gets added, we pass
		     //! the existing aliases object
		     const ref<aliasesObj> &aliasesArg=ref<aliasesObj>::create());

	//! Destructor
	~resultsetObj();

	//! Return the name of the underlying table
	virtual const char *get_table_name() const=0;

	//! Return the table's alias in the query
	virtual std::string get_table_alias() const=0;

	//! Primary key column names in this table, null terminated list
	virtual const char * const *get_primary_key_columns() const=0;

	//! Serial primary key column name(s) in this table, null terminated list
	virtual const char * const *get_serial_key_columns() const=0;

	//! Set the maximum number of rows in the resultset
	void limit(size_t maxrowsArg)
	{
		maxrows=maxrowsArg;
	}
	class rowObj;

	//! Constraint the resultset

	template<typename ...Args>
	inline void search(Args && ...args);

	//! Add HAVING

	template<typename ...Args>
	inline void having(Args && ...args);

	//! Add ORDER BY
	template<typename ...Args>
	inline void order_by(const std::string &col, Args && ...args)
	{
		order_by_list.push_back(col);
		order_by(std::forward<Args>(args)...);
	}

	//! No more ORDER BY
	inline void order_by()
	{
	}

	//! Add ORDER BY
	template<typename ...Args>
	inline void group_by(const std::string &col, Args && ...args)
	{
		group_by_list.push_back(col);
		group_by(std::forward<Args>(args)...);
	}

	//! No more GROUP BY
	inline void group_by()
	{
	}

	//! Construct SQL

	void get_select_sql(std::ostream &o) const;

	//! Construct SQL

	//! \internal
	void get_select_sql(std::ostream &o,
			    std::vector<std::string> &columns)
		const LIBCXX_HIDDEN;

	//! Execute an UPDATE statement
	template<typename ...Args>
	inline size_t update(Args && ...args) const;

	//! Remove prefix from column list

	//! \internal
	static void remove_prefix(std::vector<std::string> &fields,
				  const std::string &alias) LIBCXX_INTERNAL;

	//! Construct the SET part of an UPDATE

	static void add_update_set(std::ostream &o,
				   const std::vector<std::string> &fields,
				   const std::vector<std::string> &placeholders
				   ) LIBCXX_HIDDEN;

	//! Format joins into an SQL
	static void get_join_sql(std::ostream &o,
				 const joinlist_t &joinlist) LIBCXX_HIDDEN;

private:

	//! Collecting UPDATE arguments

	//! No more arguments.
	size_t do_update(const ref<constraintObj::andObj> &set) const;
public:
	//! Add WHERE clause

	//! \internal
	void add_where(std::ostream &) const LIBCXX_INTERNAL;

protected:
	//! Called by begin() in subclass to prepare and execute a SELECT

	statement execute_search_sql(size_t limitvalue) const;

public:
	//! Joins added to this resultset

	//! For each added join we record:
	//!
	//! - the SQL for that join
	//! - the join object for that join, from which we obtain secondary
	//! join.

	joinlist_t joinlist;

	//! Add a join to the resultset

	void addjoin(const char *jointype, const ref<joinBaseObj> &join,
		     std::initializer_list<const char *> &&columns);

	//! Add joins to this resultset

	void get_join_sql(std::ostream &o) const;

	//! Invoke prefetch_column_list() for all the joins

	void join_prefetch_column_list(std::vector<std::string> &list) const;

	//! Invoke bind_prefetched_row() for all the joins

	void join_bind_prefetched_row(std::vector<bindrowimpl> &list) const;

	//! Resultset implementation subclass forwards this to its create_column_list()
	virtual void resultset_create_column_list(std::vector<std::string>
						  &columns) const=0;

protected:

	//! Subclass collects values, calls do_insert() to figure out what to do

	//! \internal
	//! Used by INSERT processing.

	void do_insert(std::ostream &o,
		       const ref<constraintObj::andObj> &insert_constraints,
		       const ref<constraintObj::andObj> &where_constraints,
		       const ref<constraintObj::andObj> &values);

	//! INSERT statement being created here.
	std::stringstream insert_sql;

	//! INSERT/SELECT constraint created here.
	constraintptr insert_select_constraint;
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
	~rowObj();

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

	//! Update this row
	virtual bool update()=0;
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

protected:

	//! Whether to prefetch the columns from this join

	bindrowimplptr prefetch_flag;

	//! Prefetch a row from this join, install a handler for it.

	inline void install_prefetch(const bindrowimplptr &prefetch_flagArg)
	{
		prefetch_flag=prefetch_flagArg;
	}
public:

	//! Constructor

	joinBaseObj();

	//! Destructor
	~joinBaseObj();

        //! Return the real table's name
	virtual const char *get_table_name() const=0;

	//! Return the table's alias in the query
	virtual std::string get_table_alias() const=0;

	//! Column names in this table, null terminated list
	virtual const char * const *get_table_columns() const=0;

	//! Assemble aliased column list for this resultset only

	//! Calls get_table_columns() and get_table_alias(), creates "alias.name" for each
	//! column, appends it to the given column list

	void create_columns_list(std::vector<std::string> &list) const;

	//! Building joins for the resultset
	virtual void get_join_sql_recursive(std::ostream &o) const=0;

	//! If \c prefetch_flag, then call create_columns_list().

	//! In all cases, call prefetch_column_list_recursive() afterwards.

	void prefetch_column_list(std::vector<std::string> &list) const;

	//! Invoke prefetch_column_list() in subjoins

	virtual void prefetch_column_list_recursive(std::vector<std::string>
						    &list) const=0;

	//! If \c prefetch_flag then add it to the list.

	//! In all cases, call bind_prefetched_recursive() afterwards.

	void bind_prefetched(std::vector<bindrowimpl> &list);

	//! Invoke bind_prefetched() in subjoins

	virtual void bind_prefetched_recursive(std::vector<bindrowimpl> &) const=0;
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
