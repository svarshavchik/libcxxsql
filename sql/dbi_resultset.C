/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/sql/dbi/resultset.H"
#include "x/sql/dbi/constraint.H"
#include "x/sql/dbi/flavor.H"
#include "x/sql/connection.H"
#include "x/sql/statement.H"

#include "gettext_in.h"

#include <sstream>

namespace LIBCXX_NAMESPACE {
	namespace sql {

#if 0
	}
}
#endif

bindrowimplObj::bindrowimplObj()
{
}

bindrowimplObj::~bindrowimplObj() noexcept
{
}

namespace dbi {
#if 0
};
#endif

resultsetObj::bindrow_all::bindrow_all()
{
}

resultsetObj::bindrow_all::~bindrow_all() noexcept
{
}

void resultsetObj::bindrow_all::bind(bind_factory &factory)
{
	bindrow::consecutive row_factory(factory);

	for (const auto &row: rows)
	{
		row->bind(row_factory);
	}
}

resultsetObj::resultsetObj(const connection &connArg,
			   const ref<aliasesObj> &aliasesArg)
	: conn(connArg), maxrows(0), aliases(aliasesArg),
	  where(ref<constraintObj::andObj>::create()),
	  having_constraint(ref<constraintObj::andObj>::create())
{
}

resultsetObj::~resultsetObj() noexcept
{
}

resultsetObj::aliasesObj::aliasesObj()
{
}

resultsetObj::aliasesObj::~aliasesObj() noexcept
{
}

std::string resultsetObj::aliasesObj::get_alias(const std::string &table_name)
{
	auto b=table_name.begin(), e=table_name.end();

	while (b != e)
	{
		--e;
		if (*e != '_' && (*e < '0' || *e > '9'))
		{
			++e;
			break;
		}
	}

	std::string alias(b, e);

	auto iter=counter.insert(std::make_pair(alias, (size_t)0)).first;

	if (iter->second++ == 0)
		return alias;

	std::ostringstream o;

	o << alias << '_' << iter->second;
	return o.str();
}

statement resultsetObj::execute_search_sql(size_t limitvalue) const
{
	std::ostringstream o;

	auto insert_constraint=constraint::create();

	if (!insert_select_constraint.null())
	{
		// Need to do an INSERT before the SELECT

		o << insert_sql.rdbuf();
		insert_constraint=insert_select_constraint;
	}

	get_select_sql(o);

	statement stmt=conn->prepare(o.str());

	stmt->limit(limitvalue);
	stmt->execute(insert_constraint, constraint(where),
		      constraint(having_constraint));

	if (!insert_select_constraint.null())
		stmt->more(); // Skip the INSERT part

	return stmt;
}

void resultsetObj::get_select_sql(std::ostream &o) const
{
	std::vector<std::string> columns;

	resultset_create_column_list(columns);
	get_select_sql(o, columns);
}

void resultsetObj::get_select_sql(std::ostream &o,
				  std::vector<std::string> &columns) const
{
	join_prefetch_column_list(columns);

	const char *sep="SELECT ";

	for (const auto &col:columns)
	{
		o << sep << col;
		sep=", ";
	}
	o << " FROM " << get_table_name() << " AS " << get_table_alias();

	get_join_sql(o);

	add_where(o);

	sep=" GROUP BY ";

	for (const auto &g:group_by_list)
	{
		o << sep << g;
		sep=", ";
	}

	if (!having_constraint->empty())
	{
		o << " HAVING ";
		having_constraint->get_sql(o);
	}

	sep=" ORDER BY ";

	for (const auto &ob:order_by_list)
	{
		o << sep << ob;
		sep=", ";
	}
}

void resultsetObj::add_where(std::ostream &o) const
{
	if (where->empty())
		return;

	o << " WHERE ";
	where->get_sql(o);
}

void resultsetObj::multiple_rows()
{
	throw EXCEPTION(_TXT(_txt("SELECT returned multiple rows when only one was expected")));
}

void resultsetObj::no_rows()
{
	throw EXCEPTION(_TXT(_txt("SELECT returned no rows when a row was expected")));
}

resultsetObj::rowObj::rowObj(const connection &connArg) : conn(connArg)
{
}

resultsetObj::rowObj::~rowObj() noexcept
{
}

//////////////////////////////////////////////////////////////////////////////

joinBaseObj::joinBaseObj()
{
}

joinBaseObj::~joinBaseObj() noexcept
{
}

void joinBaseObj::create_columns_list(std::vector<std::string> &list)
	const
{
	auto col=get_table_columns();
	auto alias=get_table_alias() + ".";

	while (*col)
	{
		list.push_back(alias + *col);
		++col;
	}
}

void joinBaseObj::prefetch_column_list(std::vector<std::string> &list) const
{
	if (!prefetch_flag.null())
		create_columns_list(list);
	prefetch_column_list_recursive(list);
}

void joinBaseObj::bind_prefetched(std::vector<bindrowimpl> &list)
{
	if (!prefetch_flag.null())
	{
		list.push_back(prefetch_flag);
		prefetch_flag=bindrowimplptr();
	}

	bind_prefetched_recursive(list);
}

void resultsetObj::join_prefetch_column_list(std::vector<std::string> &list)
	const
{
	for (const auto &join:joinlist)
		join.second->prefetch_column_list(list);
}

void resultsetObj::join_bind_prefetched_row(std::vector<bindrowimpl> &list)
	const
{
	for (const auto &join:joinlist)
		join.second->bind_prefetched(list);
}

void resultsetObj::addjoin(const char *jointype, const ref<joinBaseObj> &join,
			   std::initializer_list<const char *> &&columns)
{
	std::ostringstream o;

	std::string me=get_table_alias();
	std::string other=join->get_table_alias();
	const char *andstr="";

	o << jointype << " " << join->get_table_name() << " AS "
	  << other << " ON ";

	for (auto b=columns.begin(), e=columns.end(); b != e; )
	{
		o << andstr << me << "." << *b; ++b;
		o << "=" << other << "." << *b; ++b;
		andstr=" AND ";
	}
	joinlist.push_back(std::make_pair(o.str(), join));
}

// Called from execute_search_sql()

// Adds joins added here, recursively invokes get_join_sql() on them.

void resultsetObj::get_join_sql(std::ostream &o) const
{
	get_join_sql(o, joinlist);
}

void resultsetObj::get_join_sql(std::ostream &o,
				const joinlist_t &joinlist)
{
	for (const auto &join:joinlist)
	{
		o << " " << join.first;
		join.second->get_join_sql_recursive(o);
	}
}

/////////////////////////////////////////////////////////////////////////////

size_t resultsetObj::do_update(const ref<constraintObj::andObj> &set) const
{
	std::vector<std::string> fields;
	std::vector<std::string> placeholders;
	std::vector<const_constraint> dummy;

	// Retrieve what to set, and to which.

	set->get_sql(fields, placeholders, dummy);

	if (fields.empty())
		throw EXCEPTION(_TXT(_txt("Empty column list passed to update()")));

	if (!having_constraint->empty() ||
	    !group_by_list.empty() ||
	    !order_by_list.empty())
		throw EXCEPTION(_TXT(_txt("An UPDATE resultset cannot have HAVING, GROUP BY, or ORDER BY")));

	std::ostringstream o;

	if (joinlist.empty())
	{
		remove_prefix(fields, get_table_alias());

		o << "UPDATE " << get_table_name();

		add_update_set(o, fields, placeholders);
		add_where(o);
	}
	else
	{
		conn->flavor()->update_with_joins(o, *this,
						  fields, placeholders);
	}

	statement stmt=conn->prepare(o.str());

	stmt->execute(constraint(set), constraint(where));

	return stmt->row_count();
}

void resultsetObj::remove_prefix(std::vector<std::string> &fields,
				 const std::string &alias)
{
	std::string prefix=alias + ".";

	for (auto &field:fields)
	{
		if (field.substr(0, prefix.size()) == prefix)
			field=field.substr(prefix.size());
	}
}

void resultsetObj::add_update_set(std::ostream &o,
				  const std::vector<std::string> &fields,
				  const std::vector<std::string> &placeholders)
{
	const char *pfix=" SET ";

	auto placeholder=placeholders.begin();

	for (const auto &field:fields)
	{
		o << pfix << field << "=" << *placeholder;
		++placeholder;
		pfix=",";
	}
}

/////////////////////////////////////////////////////////////////////////////

void resultsetObj::do_insert(std::ostream &o,
			     const ref<constraintObj::andObj> &insert_constraints,
			     const ref<constraintObj::andObj> &where_constraints,
			     const ref<constraintObj::andObj> &values)
{
	std::vector<std::string> fields;
	std::vector<std::string> placeholders;
	std::vector<const_constraint> individual_constraints;

	if (!having_constraint->empty() ||
	    !group_by_list.empty() ||
	    !order_by_list.empty())
		throw EXCEPTION(_TXT(_txt("An INSERT resultset cannot have HAVING, GROUP BY, or ORDER BY")));

	// First, turn search constraints into values

	where->get_sql(fields, placeholders, individual_constraints);
	values->get_sql(fields, placeholders, individual_constraints);

	remove_prefix(fields, get_table_alias());

	const char * const *primary_keys=get_primary_key_columns();
	const char * const *serial_keys=get_serial_key_columns();

	// Check for duplicates, verify primary keys.

	{
		std::set<std::string> dupe;

		for (const auto &field:fields)
			if (!dupe.insert(field).second)
				throw EXCEPTION(gettextmsg
						(_TXT(_txt("%1% is specified "
							   "more than once "
							   "in INSERT")),
						 field));

		// It's OK if SERIAL primary key(s) is/are missing, we'll take
		// care of them later.
		for (auto p=serial_keys; *p; ++p)
			dupe.insert(*p);

		for (auto p=primary_keys; *p; ++p)
			if (dupe.find(*p) == dupe.end())
				throw EXCEPTION(gettextmsg
						(_TXT(_txt("Primary key %1% "
							   "not specified "
							   "in INSERT")),
						 *p));
	}

	o << "INSERT INTO " << get_table_name();

	if (fields.empty()) // Must be primary key autoincrement
	{
		o << " VALUES(" << conn->flavor()->default_value_serial()
		  << "); ";
	}
	else
	{
		const char *pfix="(";

		for (const auto &field:fields)
		{
			o << pfix << field;
			pfix=",";
		}
		o << ") VALUES(";
		pfix="";

		for (const auto &placeholder:placeholders)
		{
			o << pfix << placeholder;
			pfix=",";
		}
		o << "); ";
	}

	// Placeholders for VALUES() portion.

	insert_constraints->insert(insert_constraints->end(),
				   individual_constraints.begin(),
				   individual_constraints.end());

	// Now, figure out the SELECT WHERE constraint

	// Figure out what serial keys we need to grab
	std::set<std::string> get_serial_keys;

	for (auto p=serial_keys; *p; ++p)
		get_serial_keys.insert(*p);

	for (const auto &field:fields)
		get_serial_keys.erase(field);

	// What's left is what we'll need

	if (!get_serial_keys.empty())
	{
		where_constraints->
			push_back(conn->flavor()->
				  get_inserted_serial(get_table_name(),
						      get_serial_keys)
				  );
	}

	std::set<std::string> primary_key_lookup;

	// Add the primary keys to the SELECT lookup, unless already
	// handled by serial key code, above.
	for (auto p=primary_keys; *p; ++p)
		if (get_serial_keys.find(*p) == get_serial_keys.end())
			primary_key_lookup.insert(*p);


	for (size_t i=0; i<fields.size(); ++i)
		if (primary_key_lookup.find(fields[i])
		    != primary_key_lookup.end())
		{
		
			where_constraints->push_back(individual_constraints[i]);
		}
}

#if 0
{
	{
		{
#endif
		};
	};
};
