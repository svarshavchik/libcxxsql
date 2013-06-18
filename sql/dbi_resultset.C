/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/sql/dbi/resultset.H"
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
	  where(ref<constraintObj::andObj>::create())
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

statement resultsetObj::execute_search_sql(size_t limitvalue,
					   const std::vector<std::string>
					   &columns) const
{
	std::ostringstream o;

	const char *sep="SELECT ";

	for (const auto &col:columns)
	{
		o << sep << col;
		sep=", ";
	}
	o << " FROM " << get_table_name() << " AS " << get_table_alias();

	get_join_sql(o);

	if (!where->empty())
	{
		o << " WHERE ";
		where->get_select_sql(o);
	}

	statement stmt=conn->prepare(o.str());

	stmt->limit(limitvalue);
	stmt->execute(constraint(where));
	return stmt;
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
	for (const auto &join:joinlist)
	{
		o << " " << join.first;
		join.second->get_join_sql_recursive(o);
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
