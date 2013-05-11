/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_newstatementfwd_H
#define x_sql_newstatementfwd_H

#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {

	namespace sql {
#if 0
	};
};
#endif

class newstatementObj;

//! Set options for a new SQL statement, then prepare it.

//! \code
//! INSERT_LIBX_NAMESPACE::newstatement newstmt=conn->newstatement();
//!
//! INSERT_LIBX_NAMESPACE::statement newstmt->prepare("SELECT * from books");
//! \endcode
//!
//! \c newstatement collects options for a new SQL statement. Once the
//! options are set. prepare() prepares a new SQL statement for execution.

typedef ref<newstatementObj> newstatement;

//! A nullable pointer reference to an \ref newstatement "new SQL statement".

typedef ptr<newstatementObj> newstatementptr;

//! A reference to a constant \ref newstatement "new SQL statement".

typedef const_ref<newstatementObj> newconst_statement;

//! A nullable pointer reference to a constant \ref newstatement "new SQL statement".

typedef const_ptr<newstatementObj> newconst_statementptr;

#if 0
{
	{
#endif
	}
}
#endif
