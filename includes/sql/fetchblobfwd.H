/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_fetchblobfwd_H
#define x_sql_fetchblobfwd_H

#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {

	namespace sql {
#if 0
	};
};
#endif

template<typename char_type> class fetchblobObj;
template<typename char_type> class fetchblobBase;

class fetchcloseObj;

//! Fetch a blob
//!
//! \code
//! std::string blob;
//!
//! statement->fetch("memo_column",
//!                  INSERT_LIBX_NAMESPACE::sql::fetchblob<char>::create([&blob]
//!                      (size_t rownum)
//!                      {
//!                          return std::back_insert_iterator<std::string>(blob);
//!                      }));
//!
//! std::string blobs[4];
//!
//! statement->fetch_vectors(4, "memo_column",
//!                  INSERT_LIBX_NAMESPACE::sql::fetchblob<char>::create([&blob]
//!                      (size_t rownum)
//!                      {
//!                          return std::back_insert_iterator<std::string>(blobs[rownum]);
//!                      }));
//! \endcode
//!
//! Fetch an arbitrarily large blob column by passing an
//! \c INSERT_LIBX_NAMESPACE::sql::fetchblob<char_type> instead of an lvalue
//! to fetch(), or an lvalue to a vector to fetch_vectors().
//!
//! The parameter to create() is an output iterator factory. This is a
//! lambda or a functor that takes a single
//! \c size_t parameter and returns an output iterator over \c char or
//! \c unsigned \c char values, that matches
//! \c fetchblob's template parameter.
//!
//! Use \c INSERT_LIBX_NAMESPACE::sql::fetchblob<char> with text blobs, and
//! \c INSERT_LIBX_NAMESPACE::sql::fetchblob<unsigned char> with binary blobs.
//! The implementation semantics of text and binary blobs are database
//! driver-specific. Support depends on the database driver. Some drivers
//! may or may not support fetching more than one blob, or fetching a vector
//! of blobs.
//!
//! The fetched blob gets written to the output iterator that's returned
//! from the lambda/functor. fetch() always invokes the lambda functor with
//! the parameter of 0. fetch_vectors() invokes the lambda/functor for each
//! row where the blob is not NULL, passing the 0-based row number as
//! a parameter.
//!
//! \code
//! std::string blob;
//!
//! std::pair<INSERT_LIBX_NAMESPACE::sql::fetchblob<char>, INSERT_LIBX_NAMESPACE::sql::bitflag> blob_fetch=
//!   std::make_pair(INSERT_LIBX_NAMESPACE::sql::fetchblob<char>::create([&blob]
//!                      (size_t rownum)
//!                      {
//!                          return std::back_insert_iterator<std::string>(blob);
//!                      }, 0);
//!
//!
//! statement->fetch("memo_column", blob_fetch);
//!
//! std::pair<INSERT_LIBX_NAMESPACE::sql::fetchblob<char>, std::vector<INSERT_LIBX_NAMESPACE::sql::bitflag>> blob_vector_fetch=
//!   std::make_pair(INSERT_LIBX_NAMESPACE::sql::fetchblob<char>::create([&blob]
//!                      (size_t rownum)
//!                      {
//!                          return std::back_insert_iterator<std::string>(blob);
//!                      }, std::vector<INSERT_LIBX_NAMESPACE::sql::bitflag>());
//!
//! statement->fetch_vectors(4, "memo_column", blob_vector_fetch);
//! \endcode
//!
//! Pair \c INSERT_LIBX_NAMESPACE::sql::fetchblob<char> with an
//! \c INSERT_LIBX_NAMESPACE::sql::bitflag, for fetch(), and with
//! a vector of them, for fetch_vectors(), in order to retrieve NULL indicator
//! flags explicitly.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::sql::fetchblob<char>::create([&blob]
//!     (size_t rownum)
//!     {
//!        return std::make_pair(std::back_insert_iterator<std::string>(blob),
//!            []
//!            (std::back_insert_iterator<std::string> iter)
//!            {
//!                *iter++='\n';
//!            });
//!     }
//! \endcode
//!
//! The lambda/functor can return a std::pair instead of a single iterator
//! value. The first value in the pair is the output iterator factory. The
//! second value in the pair is another lambda/functor, which takes an
//! output iterator value for a parameter. This is the "finish" lambda/functor.
//!
//! The finish lambda/functor gets invoked after the entire contents of the
//! blob get written to the output iterator, and it receives the value of the
//! output iterator, after iterating over the entire blob, as its parameter.
//! This finish lambda/functor is the means of obtaining the final output
//! iterator value after iterating over the entire contents of the blob.

template<typename char_type>
using fetchblob=ref<fetchblobObj<char_type>, fetchblobBase<char_type>>;

//! A nullable pointer reference to an \ref fetchblob "SQL fetchblob".

template<typename char_type>
using fetchblobptr=ptr<fetchblobObj<char_type>, fetchblobBase<char_type>>;

//! A reference to a constant \ref fetchblob "SQL fetchblob".

template<typename char_type>
using const_fetchblob=const_ref<fetchblobObj<char_type>, fetchblobBase<char_type>>;

//! A nullable pointer reference to a constant \ref fetchblob "SQL fetchblob".

template<typename char_type>
using const_fetchblobptr=const_ptr<fetchblobObj<char_type>, fetchblobBase<char_type>>;

//! Close a blob that was just fetched

//! \internal
//!
typedef ref<fetchcloseObj> fetchclose;

//! A nullable pointer reference to a \ref fetchclose "fetchclose handler".

//! \internal
//!
typedef ptr<fetchcloseObj> fetchcloseptr;

#if 0
{
	{
#endif
	}
}
#endif
