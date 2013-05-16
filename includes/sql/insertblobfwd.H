/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_insertblobfwd_H
#define x_sql_insertblobfwd_H

#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {

	namespace sql {
#if 0
	};
};
#endif

class insertblobObj;
class insertblobBase;

//! Insert a blob parameter

//! \code
//! auto fd=INSERT_LIBX_NAMESPACE::fd::open("blob.txt", O_RDONLY);
//!
//! int memo_id;
//!
//! conn->execute("INSERT INTO memos(memo_id, memo_txt) VALUES (?, ?)",
//!     memo_id,
//!     INSERT_LIBX_NAMESPACE::sql::insertblob::create(INSERT_LIBX_NAMESPACE::inputiter(fd), INSERT_LIBX_NAMESPACE::inputiter()));
//! \endcode
//!
//! \c INSERT_LIBX_NAMESPACE::insertblob's create() method takes a pair
//! of iterators for a sequence that defines the contents of a blob. The
//! resulting \c INSERT_LIBX_NAMESPACE::sql::insertblob can be used in place
//! of any parameter to execute().
//!
//! \code
//! auto fd=INSERT_LIBX_NAMESPACE::fd::open("blob.txt", O_RDONLY);
//!
//! int memo_id;
//!
//! std::pair<INSERT_LIBX_NAMESPACE::sql::insertblob, INSERT_LIBX_NAMESPACE::sql::bitflag> memo_blob=std::make_pair(
//!     INSERT_LIBX_NAMESPACE::sql::insertblob::create(INSERT_LIBX_NAMESPACE::fdinputiter(fd), INSERT_LIBX_NAMESPACE::fdinputiter()), 0);
//!
//! std::pair<INSERT_LIBX_NAMESPACE::sql::insertblob, INSERT_LIBX_NAMESPACE::sql::bitflag> memo_null_blob=std::make_pair(
//!     INSERT_LIBX_NAMESPACE::sql::insertblob::create(), 1);
//!
//! conn->execute("INSERT INTO memos(memo_id, memo_txt) VALUES (?, ?)",
//!     memo_id,
//!     memo_blob);
//! \endcode
//!
//! Similar to regular execute() parameters,
//! a \c std::pair of a \c INSERT_LIBX_NAMESPACE::sql:insertblob and
//! an \c INSERT_LIBX_NAMESPACE::sql::bitflag specifies a possibly NULL value
//! for the parameter. Use create() with no parameters to create an
//! \c INSERT_LIBX_NAMESPACE::sql:insertblob for an empty character sequence,
//! and set the \c INSERT_LIBX_NAMESPACE::sql::bitflag in order to specify
//! a NULL value.

typedef ref<insertblobObj, insertblobBase> insertblob;

//! A nullable pointer reference to an \ref insertblob "SQL insertblob".

typedef ptr<insertblobObj, insertblobBase> insertblobptr;

//! A reference to a constant \ref insertblob "SQL insertblob".

typedef const_ref<insertblobObj, insertblobBase> const_insertblob;

//! A nullable pointer reference to a constant \ref insertblob "SQL insertblob".

typedef const_ptr<insertblobObj, insertblobBase> const_insertblobptr;

//! Bit indicator typedef

//! This is used for a NULL value flag indicator, also for bit database fields.
//! We can't use a bool because of the \c std::vector<bool>
//! clusterfsck.
//!
//! This is also used to store the return status of individual rows in a
//! vector execute.

typedef unsigned char bitflag;

#if 0
{
	{
#endif
	}
}
#endif
