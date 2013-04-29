/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_env_H
#define x_sql_env_H

#include <x/sql/envobj.H>
#include <x/sql/envfwd.H>
#include <x/ref.H>

namespace LIBCXX_NAMESPACE {
	namespace sql {
#if 0
	};
};
#endif

//! Base class for \ref env "environment handles".

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::sql::env::base.

class envBase {

public:

	//! Private constructor

	static env create();

	//! Object factory with the create() method.

	template<typename ptrrefType> class objfactory {
	public:

		//! Forward create() to the private constructor

		template<typename... Args_t>
		static inline ptrrefType
		create(Args_t &&...args)
		{
			return envBase::create(std::forward<Args_t>
					       (args)...);
		}
	};

	//! Convenience typedef

	typedef envObj::arglist_t arglist_t;
};

#if 0
{
	{
#endif
	}
}
#endif
