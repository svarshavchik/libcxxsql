/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_dbi_resultsetfwd_H
#define x_sql_dbi_resultsetfwd_H

#include <x/ptrfwd.H>
#include <iostream>

namespace LIBCXX_NAMESPACE {

	namespace sql {
		namespace dbi {
#if 0
		};
	};
};
#endif

class resultsetObj;
class resultsetBase;
class joinBaseObj;

//! An SQL resultset

typedef ref<resultsetObj, resultsetBase> resultset;

//! A nullable pointer reference to an \ref resultset "SQL resultset".

typedef ptr<resultsetObj, resultsetBase> resultsetptr;

//! A reference to a constant \ref resultset "SQL resultset".

typedef const_ref<resultsetObj, resultsetBase> const_resultset;

//! A nullable pointer reference to a constant \ref resultset "SQL resultset".

typedef const_ptr<resultsetObj, resultsetBase> const_resultsetptr;

#if 0
{
#endif
};

class bind_factory;

//! Private API used by resultsetObj to bind row's columns, for fetch()

class bindrow {

public:
	friend class dbi::resultsetObj;

	//! Called from fetch()

	virtual void bind(bind_factory &)=0;
};

#if 0
{
	{
#endif
	}
}
#endif