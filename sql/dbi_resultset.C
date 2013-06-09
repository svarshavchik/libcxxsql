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
		namespace dbi {
#if 0
		}
	}
};
#endif

resultsetObj::resultsetObj(const connection &connArg,
			   const ref<aliasesObj> &aliasesArg)
	: conn(connArg), aliases(aliasesArg),
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

statement resultsetObj::execute_search_sql() const
{
	std::ostringstream o;

	std::string table_alias=get_table_alias();

	const char *sep="SELECT ";

	for (auto col=get_table_columns(); *col; ++col)
	{
		o << sep << table_alias << "." << *col;
		sep=", ";
	}
	o << " FROM " << get_table_name() << " AS " << table_alias;

	if (!where->empty())
	{
		o << " WHERE ";
		where->get_select_sql(o);
	}

	statement stmt=conn->prepare(o.str());

	stmt->execute(constraint(where));
	return stmt;
}

resultsetObj::rowObj::rowObj(const connection &connArg) : conn(connArg)
{
}

resultsetObj::rowObj::~rowObj() noexcept
{
}

#if 0
{
	{
		{
#endif
		};
	};
};
