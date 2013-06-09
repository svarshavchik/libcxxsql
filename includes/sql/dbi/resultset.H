/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_dbi_resultset_H
#define x_sql_dbi_resultset_H

#include <x/sql/dbi/resultsetobj.H>
#include <x/sql/dbi/resultsetfwd.H>
#include <x/sql/dbi/constraint.H>

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

#if 0
{
	{
		{
#endif
		};
	};
};
#endif
