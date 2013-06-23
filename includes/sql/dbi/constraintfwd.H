/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_dbi_constraintfwd_H
#define x_sql_dbi_constraintfwd_H

#include <x/ptrfwd.H>
#include <x/vectorfwd.H>
#include <string>

namespace LIBCXX_NAMESPACE {

	namespace sql {

		//! Use raw, unquoted SQL, instead of a placeholder.

		//! However, the raw, unquoted SQL can have its own placeholders
		//! instead.

		//! See \ref dbi_constraint "constraint".

		class text {
		public:
			//! The string

			std::string s;

			//! Forward constructor parameters

			template<typename ...Args> inline
			explicit text(Args && ...args)
				: s(std::forward<Args>(args)...)
			{
			}
		};

		namespace dbi {
#if 0
		};
	};
};
#endif

class constraintObj;
class constraintBase;

//! \anchor dbi_constraint A reference to an SQL constraint object

//! \code
//! int key;
//! INSERT_LIBX_NAMESPACE::sql::connection conn;
//!
//! // ...
//!
//! INSERT_LIBX_NAMESPACE::sql::dbi::constraint where=INSERT_LIBX_NAMESPACE::sql::dbi::constraint::create("a", "=", key);
//!
//! std::ostringstream o;
//!
//! o << "SELECT * FROM memos WHERE " << where;
//!
//! auto stmt=conn->prepare(o.str());
//! stmt->execute(constraint);
//!
//! \endcode
//!
//! The literal SQL constructed in this example is
//! "SELECT * FROM memos WHERE a = ?" (the actual SQL literal might have a few
//! extra set of parenthesis that are not shown here, for clarity).
//! \c INSERT_LIBX_NAMESPACE::sql::dbi::constraint is a reference to
//! a \ref ref "reference-counted object" that represents an SQL constraint
//! that's typically found in a WHERE clause.
//!
//! Passing the \c INSERT_LIBX_NAMESPACE::sql::dbi::constraint to
//! execute() supplies the values for the \c ? placeholder parameters in the
//! constructed constraint. The constructor saves the passed value internally,
//! and provides it as a parameter to execute(). Values can be of any type
//! that execute() supports.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::sql::dbi::constraint::create("a", "=", key,
//!                                 "b", "!=", value);
//! \endcode
//!
//! This constructs "A = ? AND b != ?". create() takes a variadic list of
//! constraints, that get joined together by \c AND.
//!
//! \code
//! auto constraint=INSERT_LIBX_NAMESPACE::sql::dbi::AND("a", "=", key, "b", "!=", value);
//!
//! auto constraint=INSERT_LIBX_NAMESPACE::sql::dbi::OR("a", "=", key, "b", "!=", value);
//! \endcode
//!
//! AND() is equivalent to instantiating a new constraint. \c OR() uses
//! \c OR clauses instead of \c AND clauses. The second example constructs
//! "a = ? OR b != ?". They can be combined accordingly:
//!
//! \code
//! auto constraint=INSERT_LIBX_NAMESPACE::sql::dbi::AND("key", "=", 4,
//!                                  INSERT_LIBX_NAMESPACE::sql::dbi::OR("category", "=", "memo",
//!                                                  "category", "=", "note"),
//!                                  "discarded", "=", 0);
//! \endcode
//!
//! This constructs a "(key = ? AND (category = ? OR category = ?)
//! AND discarded = ?)" constraint.
//!
//! \code
//! auto constraint=INSERT_LIBX_NAMESPACE::sql::dbi::NOT("category", ">", 0,
//!                                  "category", "<", 10);
//! \endcode
//!
//! NOT() is equivalent to AND(), inside an SQL "NOT". This example constructs
//! "NOT (category > ? AND category < ?)" SQL.
//!
//! Giving \c nullptr for a value creates SQL that tests for a \c NULL:
//!
//! \code
//! auto constraint=INSERT_LIBX_NAMESPACE::sql::dbi::AND("category", ">", 0,
//!                                  "discarded", "=", nullptr);
//! \endcode
//!
//! This example constructs "CATEGORY > ? AND discarded IS NULL" SQL.
//! Using "!=" instead of "=" produces an "IS NOT NULL" instead, and all other
//! comparison strings result in generating a dummy "1=0" constraint, which
//! always evaluates to a logical false.
//!
//! \code
//! std::vector<int> codes={1,5};
//!
//! auto constraint=INSERT_LIBX_NAMESPACE::sql::dbi::AND("category", ">", 0,
//!                                  "code", "=", codes);
//! \endcode
//!
//! Comparing something using "=" against a \c std::vector results in an
//! "IN" SQL. The above example constructs
//! "category > ? AND code IN (?, ?)", with the number of placeholders
//! depending on the size of the vector. The constraint makes a copy of the
//! vector (which may or may not be a problem), and passes its values to
//! execute(), for the placeholders.
//!
//! The "!=" comparator creates a "NOT IN" SQL. Any other comparator throws
//! an exception.
//!
//! An empty vector constructs substitute SQL that returns the appropriate
//! value. Using "=" to compare anything to an empty list constructs
//! a "1=0" SQL, that evaluates to a logical false. Comparing "x" against an
//! empty list using "!=" constructs a "x IS NOT NULL" SQL. The logical
//! result of comparing that a non-NULL value does not exist in an empty list
//! is always true, and any comparison that involves a NULL value is always
//! false.
//!
//! \code
//! auto codes=INSERT_LIBX_NAMESPACE::vector<int>::create();
//!
//! auto constraint=INSERT_LIBX_NAMESPACE::sql::dbi::AND("category", ">", 0,
//!                                  "code", "=", codes);
//! \endcode
//!
//! An \ref vector "INSERT_LIBX_NAMESPACE::vector" may be used in place of
//! \c std::vector. This avoids the need to copy an entire
//! \c std::vector inside the constraint, for a subsequent delivery to
//! execute(). However, no locking takes place here, and the vector should
//! not be modified between the time the literal SQL gets prepared, and
//! the subsequent call to execute().
//!
//! \code
//! INSERT_LIBX_NAMESPACE::sql::dbi::constraint::create("a", INSERT_LIBX_NAMESPACE::sql::text("="), "MAX(b)");
//! \endcode
//!
//! Wrapping the comparison string into a \c INSERT_LIBX_NAMESPACE::sql:text
//! has the effect of inserting the raw SQL into the constraint. This
//! example generates the "a = MAX(b)" SQL. Without
//! \c INSERT_LIBX_NAMESPACE::sql:text the result would've been
//! "a = ?", with "MAX(b) provided as the placeholder's value.
//!
//! Passing a std::list<std::string> value enables manual placeholders in the
//! comparison SQL:
//!
//! \code
//! INSERT_LIBX_NAMESPACE::sql::dbi::constraint::create("a", INSERT_LIBX_NAMESPACE::sql::text("="),
//!     std::list<std::string>({
//!         "currval(pg_get_serial_sequence(?, ?))",
//!         table_name, column
//!     }));
//! \endcode
//!
//! This generates the SQL "a = currval(pg_get_serial_sequence(?, ?))", with
//! the values for the two placeholders coming from the table_name and column
//! parameters. This is a specialized syntax that's used internally by dbi,
//! and is of little use in general.

typedef ref<constraintObj, constraintBase> constraint;

//! A nullable pointer reference to an \ref dbi_constraint "SQL constraint".

typedef ptr<constraintObj, constraintBase> constraintptr;

//! A reference to a constant \ref dbi_constraint "SQL constraint".

typedef const_ref<constraintObj, constraintBase> const_constraint;

//! A nullable pointer reference to a constant \ref dbi_constraint "SQL constraint".

typedef const_ptr<constraintObj, constraintBase> const_constraintptr;

#if 0
{
	{
		{
#endif
		}
	}
}
#endif
