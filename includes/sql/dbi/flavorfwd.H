/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_dbi_flavorfwd_H
#define x_sql_dbi_flavorfwd_H

#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {

	namespace sql {
		namespace dbi {
#if 0
		};
	};
};
#endif

class flavorObj;

//! SQL flavor

//! Installed into an individual \ref connection "connection". Implements
//! database-specific SQL dialects.

typedef ref<flavorObj> flavor;

//! A nullable pointer reference to an \ref flavor "SQL flavor".

typedef ptr<flavorObj> flavorptr;

//! A reference to a constant \ref flavor "SQL flavor".

typedef const_ref<flavorObj> const_flavor;

//! A nullable pointer reference to a constant \ref flavor "SQL flavor".

typedef const_ptr<flavorObj> const_flavorptr;

#if 0
{
	{
		{
#endif
		}
	}
}
#endif
