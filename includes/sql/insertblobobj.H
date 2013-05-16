/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_insertblobobj_H
#define x_sql_insertblobobj_H

#include <x/obj.H>
#include <x/chrcasecmp.H>
#include <x/sql/connectionfwd.H>
#include <x/sql/newstatementfwd.H>
#include <vector>
#include <iterator>
#include <limits>
#include <unistd.h>

namespace LIBCXX_NAMESPACE {
	namespace sql {
#if 0
	};
};
#endif

//! An inserted blob

//! Base class for an interface to SQLPutData().
//! This object's create() method actually constructs a subclass that
//! implements the putdata() method.

class insertblobObj : virtual public obj {

public:
	//! Constructor
	insertblobObj();

	//! Destructor
	~insertblobObj() noexcept;

	//! Returns the internal type, char or binary
	virtual int datatype()=0;

	//! Returns the internal sql data type, char or binary
	virtual int sqltype()=0;

	//! Subclass returns the length of the blob, (size_t)-1 if unknown.

	virtual size_t blobsize()=0;

	//! Subclass fills the buffer with the next contents of the blob.
	virtual size_t fill(char *buffer, size_t bufsize)=0;

	//! blobsize is to big, throw an exception.

	static void blobtoobig() __attribute__((noreturn));
};

//! Implement datatype()

//! This template implements datatype() and sqltype(). The library implements
//! insertblobdatatypeObj<char> and insertblobdatatypeObj<unsigned char>
//! only, returning SQL_C_CHAR/SQL_C_BINARY and
//! SQL_C_LONGVARCHAR/SQL_C_LONGVARBINARY only.

template<typename char_type>
class insertblobdatatypeObj : public insertblobObj {

public:
	//! Constructor
	insertblobdatatypeObj();

	//! Destructor
	~insertblobdatatypeObj() noexcept;

	//! Implement datatype()
	int datatype() override;

	//! Implement sqltype()
	int sqltype() override;
};

//! Calculate the size of the blob, unless the iterators are input iterators

template<typename iterator_trait_type> class insertblob_length {

public:

	//! Default implementation of length() for non-input iterators

	template<typename iter_type>
	inline static size_t length(const iter_type &beg_iter,
				    const iter_type &end_iter)
	{
		auto dist=std::distance(beg_iter, end_iter);

		// Perform some sanity checks

		size_t s=dist;

		if ((decltype(dist))s != dist
		    || s >= ((size_t)1 <<
			     (std::numeric_limits<size_t>::digits-1)))
			insertblobObj::blobtoobig(); // Signed value overflow

		return s;
	}
};

//! Specialization -- cannot calculate blob size from input iterators

template<>
class insertblob_length<std::input_iterator_tag> {
public:

	//! Return (size_t)-1 -- size not available.

	template<typename iter_type>
	inline static size_t length(const iter_type &beg_iter,
				    const iter_type &end_iter)
	{
		return (size_t)-1;
	}
};

//! And the actual implementation.

template<typename iter_type, typename char_type>
class insertblobimplObj : public insertblobdatatypeObj<char_type> {

public:
	//! Current iterator value
	iter_type beg_iter;

	//! Ending iterator value
	iter_type end_iter;

	//! Constructor
	insertblobimplObj(const iter_type &beg_iterArg,
			  const iter_type &end_iterArg)
		: beg_iter(beg_iterArg),
		  end_iter(end_iterArg)
	{
	}

	//! Constructor
	insertblobimplObj(iter_type &&beg_iterArg,
			  iter_type &&end_iterArg)
		: beg_iter(std::move(beg_iterArg)),
		  end_iter(std::move(end_iterArg))
	{
	}

	//! Destructor
	~insertblobimplObj() noexcept
	{
	}

	//! Implement fill()

	size_t fill(char *buffer, size_t bufsize) override
	{
		size_t n=0;

		while (bufsize && beg_iter != end_iter)
		{
			++n;
			--bufsize;
			*buffer++ = *beg_iter++;
		}
		return n;
	}

	//! Implement blobsize()
	size_t blobsize() override
	{
		return insertblob_length<typename std::iterator_traits<
			iter_type>
					 ::iterator_category>
			::length(beg_iter, end_iter);
	}
};

#if 0
{
	{
#endif
	}
}
#endif
