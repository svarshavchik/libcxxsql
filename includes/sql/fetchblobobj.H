/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_fetchblobobj_H
#define x_sql_fetchblobobj_H

#include <x/obj.H>
#include <x/refiteratorfwd.H>
#include <x/ptrfwd.H>
#include <x/sql/fetchblobfwd.H>
#include <type_traits>

namespace LIBCXX_NAMESPACE {
	namespace sql {
#if 0
	};
};
#endif

//! Close callback for the fetch blob output iterator

//! close() gets invoked just after fetch() finishes iterating the output
//! iterator over the blob's entire contents.
//!
class fetchcloseObj : virtual public obj {

public:
	//! Constructor
	fetchcloseObj();

	//! Destructor
	~fetchcloseObj();

	virtual void close(); //!< Default no-op implementation.
};

//! Invoke a fetch blob output close callback

//! Subclass of fetchcloseObj() stashes away the close callback that the
//! fetch blob factory returns, and invokes it upon close().

template<typename callback_type,
	 typename iter_type> class fetchcloseimplObj : public fetchcloseObj {

	//! The stashed callback
	callback_type callback;

	//! The stashed output iterator

	//! This is an ret to an \ref outputrefiteratorTupleObj "outputrefiteratorTupleObj".

	iter_type iter;

public:

	//! Constructor

	fetchcloseimplObj(const callback_type &callbackArg,
			  const iter_type &iterArg)
		: callback(callbackArg), iter(iterArg)
	{
	}

	//! Destructor
	~fetchcloseimplObj()
	{
	}

	//! Invoke callback
	void close()
	{
		callback(std::get<0>(iter.get()->iters));
	}
};

//! The fetch blob factory returns only a new output iterator.

//! We'll supply a dummy finish/close functor/lambda.

template<typename ret_type> class fetch_return_value_impl {

 public:

	//! Supply a default close callback.

	template<typename char_type>
	static inline
	std::pair<refobjiterator<ref<outputrefiteratorObj<char_type>>
				 >, fetchclose>	value(ret_type &&ret)
	{
		return std::make_pair(make_outputrefiterator<char_type>
				      (std::move(ret)),
				      fetchclose::create());
	}
};

//! The fetch blob factory returns an output iterator and a close callback

template<typename first_pair_type, typename second_pair_type>
class fetch_return_value_impl<std::pair<first_pair_type,
					second_pair_type>> {

 public:

	//! Stash away the close callback into a fetchclose object.

	template<typename char_type>
		static inline
		std::pair<refobjiterator<ref<outputrefiteratorObj<char_type>>
					 >, fetchclose>
		value(std::pair<first_pair_type, second_pair_type> &&ret)
	{
		auto refiterator=make_outputrefiterator<char_type>(ret.first);

		return std::make_pair(refiterator,
				      ref<fetchcloseimplObj<second_pair_type,
				      decltype(refiterator)>>
				      ::create(ret.second, refiterator));
	}
};

//! Base class that wraps the fetch blob factory.

template<typename char_type>
class fetchblobObj : virtual public obj {

public:
	//! Constructor
	fetchblobObj() {}

	//! Destructor
	~fetchblobObj() {}

	//! Create an output iterator for a given row, and the close function.

	virtual std::pair<refobjiterator<ref<outputrefiteratorObj<char_type>>
					 >, fetchclose> create(size_t rownum)=0;
};

//! Fetch blob factory implementation.

//! create() by stashing away the parameter to
//! \c INSERT_LIBX_NAMESPACE::sql::fetchblob<char_type>::create(),
//! and implementing fetchblobObj::create() by invoking the stashed output
//! iterator factory.

template<typename char_type, typename factory_type>
class fetchblobimplObj : public fetchblobObj<char_type> {

	//! Remove any reference from the fetch blob factory type
	typedef typename std::remove_reference<factory_type>::type noref;

	//! Remove any const/volatile qualifier from the blob factory type

	typedef typename std::remove_cv<noref>::type nocv;

	//! The stashed factory.
	nocv factory;

public:

	//! Constructor
	fetchblobimplObj(factory_type &&factoryArg)
		: factory(std::forward<factory_type>(factoryArg))
	{
	}

	//! Destructor
	~fetchblobimplObj()
	{
	}

	//! Invoke the fetch blob factory.

	std::pair<refobjiterator<ref<outputrefiteratorObj<char_type>>
				 >, fetchclose> create(size_t rownum) override
	{
		return fetch_return_value_impl<decltype(factory(rownum))>
			::template value<char_type>(factory(rownum));
	}
};

#if 0
{
	{
#endif
	}
}
#endif
