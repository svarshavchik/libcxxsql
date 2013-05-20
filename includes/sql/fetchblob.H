/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_fetchblob_H
#define x_sql_fetchblob_H

#include <x/sql/fetchblobobj.H>
#include <x/sql/fetchblobfwd.H>
#include <x/ref.H>
#include <x/refiterator.H>

namespace LIBCXX_NAMESPACE {
	namespace sql {
#if 0
	};
};
#endif

//! Base class for \ref fetchblob "fetched blobs".

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::sql::fetchblob<char_type>
//!
template<typename char_type> class fetchblobBase {

public:

	//! Implement a custom create() method for creating \ref fetchblob "fetched blobs".

	template<typename ptrrefType> class objfactory {

	public:

		//! The custom create function.

		template<typename Factory>
		static inline ptrrefType create(Factory &&factory)
		{
			return ref<fetchblobimplObj<char_type, Factory>>
				::create(std::forward<Factory>(factory));
		}
	};
};

#if 0
{
	{
#endif
	}
}
#endif
