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
//! INSERT_LIBX_NAMESPACE::sql::newstatement newstmt=conn->create_newstatement();
//!
//! newstatement->option("CURSOR_TYPE", "STATIC");
//!
//! INSERT_LIBX_NAMESPACE::sql::statement newstmt->prepare("SELECT * from books");
//! \endcode
//!
//! A \ref connection "connection object"'s create_newstatement() method
//! returns this \ref ref "reference to a referance-counted" object that
//! sets non-default options for executing new SQL statements. Its prepare()
//! method returns a new prepared SQL statement, ready for execution, using
//! the set options.
//!
//! The \ref newstatementObj "newstatement object"
//! collects options for a new SQL statement. Once the
//! options are set. prepare() prepares a new SQL statement for execution.
//!
//! Options are specified via option() that takes the option name and value,
//! as strings.
//!
//! The available options are:
//!
//! - "CURSOR_TYPE": valid values are "FORWARD" (default), "STATIC",
//! "KEYSET(n)", where \c n is a number, and "DYNAMIC". This option specifies
//! the cursor type. Use "KEYSET(1000)" to specify a keyset cursor with 1000
//! rows. Not all database drivers support every type of a cursor.
//!
//! - "BOOKMARKS": set this option to "ON" to enable bookmarks in resultsets.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::sql::statement stmt=conn->create_newstatement("CURSOR_TYPE", "STATIC", "BOOKMARKS", "ON")->prepare("SELECT * from books");
//! \endcode
//!
//! A convenient shortcut is to enumerate the options directly, to the
//! variadic create_newstatement() method. This has the effect of constructing
//! a \c newstatement, setting the given options, one by one, and then
//! invoking prepare().
//!
//! \code
//! std::map<std::string, std::string> options;
//!
//! options["CURSOR_TYPE"]="KEYSET(1000)";
//! options["BOOKMARKS"]="ON";
//!
//! INSERT_LIBX_NAMESPACE::sql::statement stmt=conn->create_newstatement(options)->prepare("SELECT * from books");
//! \endcode
//!
//! Options can also be specified indirectly, via a \c std::map.

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
