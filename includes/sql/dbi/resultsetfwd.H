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

//! The container for a list of joinlist

typedef std::list<std::pair<std::string, ref<joinBaseObj>>> joinlist_t;

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

	//! Bind consecutively-numbered columns

	class consecutive {

		//! The factory for binding fetch columns
		bind_factory &factory;

		//! Next column to bind.
		size_t column;

	public:
		//! Constructor

		consecutive(bind_factory &factoryArg)
			: factory(factoryArg), column(0)
		{
		}

		//! Bind the next column
		template<typename ArgType>
		void bind(ArgType &&arg);
	};

	//! Called from fetch()

	virtual void bind(bind_factory &)=0;
};

//! A reference-counted bindrow

class bindrowimplObj : virtual public obj {

public:
	//! Constructor
	bindrowimplObj();

	//! Destructor
	~bindrowimplObj();

	//! Bind rows
	virtual void bind(bindrow::consecutive &)=0;

	//! The row was succesfully fetched
	virtual void fetched()=0;
};

//! A reference to a reference-counted bindrow

typedef ref<bindrowimplObj> bindrowimpl;

//! A nullable pointer reference to a reference-counted bindrow

typedef ptr<bindrowimplObj> bindrowimplptr;

#if 0
{
	{
#endif
	}
}
#endif
