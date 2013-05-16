/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_insertblob_H
#define x_sql_insertblob_H

#include <x/sql/insertblobobj.H>
#include <x/sql/insertblobfwd.H>
#include <x/ref.H>

namespace LIBCXX_NAMESPACE {
	namespace sql {
#if 0
	};
};
#endif

//! Base class for \ref insertblob "insert blobs".

//! Implements a create() method that constructs a template subclass tha
//! defines a blob using a beginning and an ending iterator.

class insertblobBase {

public:

	//! Create an empty blob

	//! \internal

	static ref<insertblobObj> createEmpty();

	//! Helper class figures out template type parameters

	//! \internal

	template<typename iter_type>
	class insertblob_iter_type {

	public:

		//! Strip any reference from the iterator type

		typedef typename std::remove_reference<iter_type>::type raw_t;

		//! Strip const/volatile

		typedef typename std::remove_cv<raw_t>::type iter_t;

		//! Helper class

		static iter_t dummy_get_iter();

		//! What the iterator iterates over

		typedef decltype(*dummy_get_iter()) iter_value_t;

		//! Strip reference

		typedef typename std::remove_reference<iter_value_t>
		::type value_cv_t;

		//! Strip const/volatile

		typedef typename std::remove_cv<value_cv_t>::type value_t;
	};

	//! Object factory

	template<typename ptrrefType> class objfactory {

	public:
		//! Define a new insert blob

		//! Takes a pair of iterators that specify the beginning and
		//! an ending iterator for an input sequence that defines the
		//! blob. The input sequence is either over chars or
		//! unsigned chars.

		template<typename iter_type,
			 typename char_type=
			 typename insertblob_iter_type<iter_type>::value_t>
		static inline ptrrefType create(iter_type &&beg_iter,
						iter_type &&end_iter)
		{
			return ref<insertblobimplObj<
					   typename insertblob_iter_type
					   <iter_type>::iter_t,
					   typename insertblob_iter_type
					   <iter_type>::value_t>>
				::create(std::forward<iter_type>(beg_iter),
					 std::forward<iter_type>(end_iter));
		}

		//! Define an empty sequence

		static inline ptrrefType create()
		{
			return insertblobBase::createEmpty();
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
