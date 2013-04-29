/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_connectionobj_H
#define x_sql_connectionobj_H

#include <x/obj.H>
#include <map>

namespace LIBCXX_NAMESPACE {
	namespace sql {
#if 0
	};
};
#endif

//! SQL connection

class connectionObj : virtual public obj {

public:
	//! Constructor
	connectionObj();

	//! Destructor
	~connectionObj() noexcept;

	//! Disconnect

	//! Use this to explicitly disconnect.
	virtual void disconnect()=0;
};

#if 0
{
	{
#endif
	}
}
#endif
