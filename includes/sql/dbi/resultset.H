/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_dbi_resultset_H
#define x_sql_dbi_resultset_H

#include <x/sql/dbi/resultsetobj.H>
#include <x/sql/dbi/resultsetfwd.H>
#include <x/sql/dbi/constraint.H>
#include <x/sql/statement.H>

namespace LIBCXX_NAMESPACE {
	namespace sql {
		namespace dbi {
#if 0
		};
	};
};
#endif

//! Base class for \ref resultset "SQL resultsets".

class resultsetBase : public ptrrefBase {
};

template<typename ...Args>
inline void resultsetObj::search(Args && ...args)
{
	where->add(std::forward<Args>(args)...);
}

template<typename ...Args>
inline void resultsetObj::having(Args && ...args)
{
	having_constraint->add(std::forward<Args>(args)...);
}

#if 0
{
#endif
};

template<typename ArgType>
void bindrow::consecutive::bind(ArgType &&arg)
{
	factory.bind(column, std::forward<ArgType>(arg));
	++column;
}

#if 0
{
	{
#endif
	};
};
#endif
