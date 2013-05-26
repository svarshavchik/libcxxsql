/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_sql_statementfwd_H
#define x_sql_statementfwd_H

#include <x/ptrfwd.H>
#include <x/vector.H>

namespace LIBCXX_NAMESPACE {

	namespace sql {
#if 0
	};
};
#endif

class statementObj;
class statementBase;

//! A row bookmark

//! Before executing the query, use
//! \ref newstatementObj::option "newstatement->option"("BOOKMARKS", "ON")
//! to enable bookmarks for this query.
//!
//! This is an opaque object that represents a row bookmark.
//! This is optionally passed as the first value to
//! \ref statementObj::fetch "statement->fetch"() or, a
//! \c std::vector<INSERT_LIBX_NAMESPACE::sql::bookmark> gets passed as the
//! first value to
//! \ref statementObj::fetch_vectors "statement->fetch_vectors"(); in all cases
//! without a column name or number identifier. An opaque bookmark for each
//! row gets placed into the bookmark object, and can be used to scroll to
//! that bookmark, using
//! \ref fetch::atbookmark "INSERT_LIBX_NAMESPACE::sql::fetch::atbookmark"().

class bookmark : public vectorptr<unsigned char> {

public:
	//! Constructor
	bookmark();

	//! Destructor
	~bookmark() noexcept;

	//! Used by fetch() to load the bookmark
	bookmark &operator=(vectorptr<unsigned char> &&);

	using vectorptr<unsigned char>::operator*;
	using vectorptr<unsigned char>::operator->;

};

//! Last optional argument to \ref statementObj::fetch "statement->fetch"() or \ref statementObj::fetch_vectors "statement->fetch_vectors"().

//! Specifies fetch direction. Don't construct this yourself, let the
//! \ref fetch "fetch class" do it for you.

class fetch_orientation {

public:
	//! Orientiation code
	const int orientation;

	//! Offset
	const long long offset;

	//! Bookmark
	const bookmark * const bookmarkptr;

	//! Internal constructor
	constexpr fetch_orientation(int orientation_value,
				    long long offset_value,
				    const bookmark *bookmarkptr_value)
		: orientation(orientation_value),
		  offset(offset_value),
		  bookmarkptr(bookmarkptr_value)
	{
	}
};

//! Fetch direction/orientation.

//! Pass this as the last argument to
//! \ref statementObj::fetch "statement->fetch"() or
//! \ref statementObj::fetch_vectors "statement->fetch_vectors"(),
//! specifying the fetch direction, for cursor types that support scrolling.

class fetch {

public:

	//! Fetch the next set of rows (the default)
	static const fetch_orientation next;

	//! Fetch the previous set of rows
	static const fetch_orientation prior;

	//! Fetch the first set of rows
	static const fetch_orientation first;

	//! Fetch the last set of rows
	static const fetch_orientation last;

	//! \c INSERT_LIBX_NAMESPACE::sql::fetch::absolute(n) - fetch rows starting at row \#n (0-based)

	class absolute : public fetch_orientation {
	public:
		//! Constructor
		absolute(long long pos);
	};

	//! \c INSERT_LIBX_NAMESPACE::sql::fetch::relative(n) - fetch rows starting at row +/-n from the current row

	class relative : public fetch_orientation {
	public:
		//! Constructor
		relative(long long pos);
	};

	//! \c INSERT_LIBX_NAMESPACE::sql::fetch::atbookmark(n) - fetch rows starting at the given bookmark.

	class atbookmark : public fetch_orientation {

		//! Saved bookmark
		bookmark bookmarkSave;
	public:
		//! Constructor
		atbookmark(//! Saved bookmark
			   const bookmark &bookmarksaveArg,

			   //! Optional +/- offset # of rows from the bookmark
			   long long offset=0);
	};
};

//! A prepared SQL statement

//! This is a \ref ref "reference to a reference-counted object" that represents
//! a prepared SQL statement. It gets created by invoking prepare() on an
//! \ref connection "connection handle".
//!
//! \code
//! stmt->execute();
//! \endcode
//!
//! Execute the SQL statement, creating a new resultset if the SQL statement
//! returns one.
//!
//! \code
//! size_t params=stmt->num_params()
//!
//! stmt->execute("Title");
//! \endcode
//!
//! execute() takes a variadic list of parameters, which specify the values
//! for placeholders in the prepared SQL statement. num_params() returns the
//! number of placeholders. The number of parameters given to execute()
//! must match the number of placeholders in the SQL statement.
//!
//! The value of each placeholder can be specified as:
//!
//! - any natural numeric value; ints, floats, etc...
//!
//! - use \c nullptr to specify a \c NULL value
//!
//! - a literal character string or a \c std::string
//!
//! - a \c std::list<T>, where T is any time that's otherwise accepted
//! by execute() except for \c nullptr (like \c int or \c std::string)
//! takes the values from the list, as if each value in the vector was
//! given as an execute() parameter.
//!
//! - a \c std::list<std::pair<T, INSERT_LIBX_NAMESPACE::sql::bitflag>> is
//! processed like the values
//! for \c std::list<T> for those values where the second of the pair,
//! the INSERT_LIBX_NAMESPACE, is false. When the it's true, the corresponding
//! parameter value is a \c nullptr.
//!
//! \code
//! const std::vector<INSERT_LIBX_NAMESPACE::sql::statement::base::parameter> &cols=get_parameters();
//! \endcode
//!
//! Enumerates the expected \ref statementObj::parameter "parameter"s
//! (may not be supported by all database drivers, most drivers just tell
//! you how many parameters are needed).
//!
//! \code
//! std::vector<std::string> titles={"Hare Brush", "Hare Tonic"};
//! std::pair<std::vector<double>,
//!           std::vector<INSERT_LIBX_NAMESPACE::sql::bitflag>> prices={
//!      std::vector<double>({0.99, 0}),
//!      std::vector<INSERT_LIBX_NAMESPACE::sql::bitflag>({0, 1});
//!
//! std::vector<INSERT_LIBX_NAMESPACE::sql::bitflag> status;
//! status.resize(2);
//!
//! auto stmt=conn->prepare("insert into videos(titles, prices) values(?, ?)");
//!
//! try {
//!    stmt->execute_vector(status, titles, prices);
//! } catch (...) {
//!    // ...
//! }
//!
//! if (!(status[0] & 1) ||
//!     !(status[1] & 1))
//!     // ...
//! \endcode
//!
//! execute_vector() executes the statement with multiple sets of parameters.
//! This is usually used to bulk-insert rows into a table.
//! The parameter values are given as vectors. Each vector specifies the
//! value for the corresponding parameter in each row. It's a column vector.
//!
//! All vectors, including the first parameter, must be of the same size.
//! The first parameter is a status vector, of
//! INSERT_LIBX_NAMESPACE::sql::bitflag. The lowest bit indicates whether the
//! row was deemed to have been processed succesfully. The possible numerical
//! values are:
//!
//! - 0: unspecified error
//!
//! - 1: row processed
//!
//! - 2: row was not processed as a result of another error, some database
//! drivers stop when an error occured processing one row, and will not process
//! the remaining rows
//!
//! - 3: row processed, with modifications (usually a truncated string value,
//! with some database drivers)
//!
//! The remaining parameters are either vectors; a \c std::pair of vectors;
//! or a \c std::list of vectors or \c std::pair of vectors.
//! The second vector in the pair is always a vector of
//! INSERT_LIBX_NAMESPACE::sql::bitflags. A non-zero value specifies a NULL
//! value for the corresponding row's value of its column parameter. A zero
//! takes the value for that row from the corresponding value in the first
//! vector. Specifying just a vector, instead of a \c std::pair of vectors,
//! specifies non-NULL values for all rows of that column.
//!
//! \code
//! size_t ncols=stmt->size();
//! \endcode
//!
//! size() gives the number of columns in the resultset, or 0 if the SQL
//! statement did not return a resultset.
//!
//! \code
//! const std::vector<INSERT_LIBX_NAMESPACE::sql::statement::base::column> cols=stmt->get_columns();
//!
//! const INSERT_LIBX_NAMESPACE::sql::statement::base::columnmap_t &cols_by_name=stmt->get_columnmap();
//! \endcode
//!
//! get_columns() returns a vector that describes the columns in the resultset.
//! \ref statementObj::column "INSERT_LIBX_NAMESPACE::sql::statement::base::column"
//! contains various metadata that describe the type of each column in the
//! resultset. get_columnmap() returns a
//! \ref statementObj::columnmap_t "INSERT_LIBX_NAMESPACE::sql::statement::base::columnmap_t"
//! which is a \c std::multimap keyed by a case-insensitive column name, with
//! the \c INSERT_LIBX_NAMESPACE::sql::statement::base::column as the value.
//! Use this to look up a column by name, with
//! \c INSERT_LIBX_NAMESPACE::sql::statement::base::column's
//! \c column_number giving the column's number in the resultset.
//! This is a \c std::multimap in case because, depending on the database
//! server, there may be more than one column in the resultset with the same
//! name.
//!
//! \code
//! std::string book_name;
//! std::pair<float, INSERT_LIBX_NAMESPACE::bitflag> book_price;
//!
//! while (stmt->fetch(0, book_name,
//!                    1, book_price))
//! {
//!      // ...
//! }
//! \endcode
//!
//! Each call to fetch() returns the next row in the resultset. The values in
//! each row are placed into the given variables or objects. fetch()
//! takes the list of values as parameters. Each value is specified
//! by giving its column number, and an lvalue, which fetch() sets to the
//! value for that column.
//!
//! A \c std::pair lvalue places the column's value into pair.first, with
//! a NULL value indicator into pair.second, which must be a
//! \c INSERT_LIBX_NAMESPACE::sql::bitflag; this
//! gets set to logical \c true if the value is \c NULL, and false if the
//! column's value is not \c NULL.
//!
//! \code
//! while (stmt->fetch("book_name", book_name,
//!                    "book_price", book_price))
//! {
//!      // ...
//! }
//! \endcode
//!
//! Each column can also be specified by name. A call to fetch() can contain
//! a mixture of columns specified by number or name.
//!
//! \note
//! Column numbers are zero-based. The first column in the resultset is
//! column #0.
//!
//! \code
//! std::map<std::string, std::string> params;
//!
//! params.insert({"book_name", ""});
//! params.insert({"book_author", ""});
//!
//! std::map<std::string, std::pair<float, bool> > params2;
//!
//! params2.insert({"book_price", {0, false}});
//!
//! while (stmt->fetch(params, params2))
//! {
//!      // ...
//! }
//! \endcode
//!
//! The list of parameters can be prepared separately, as a \c std::map
//! which is keyed by a column name or number. The value of the map
//! is either the column's value, or
//! a \c std::pair of the column's value and
//! a \c INSERT_LIBX_NAMESPACE::sql::bitflag \c NULL indicator.
//! Note that all columns in a single map, necessarily, must be the same type,
//! but fetch() can receive multiple maps, with different types.
//!
//! fetch() handles values that are bools, shorts, ints, longs, long longs,
//! including their unsigned varieties, floats and doubles. A \c std::string
//! accepts character string column values.
//!
//! Depending on the database driver, a blob is handled by passing
//! an \ref fetchblob "INSERT_LIBX_NAMESPACE::sql::fetchblob<char_type>"
//! in place of an lvalue.
//!
//! \code
//! std::pair<std::vector<std::string>,
//!           std::vector<INSERT_LIBX_NAMESPACE::sql::bitflag>> titles;
//!
//! size_t cnt=stmt->fetch_vectors(1000, "title", titles);
//! \endcode
//!
//! The first parameter to fetch_vectors() is a positive number of rows,
//! the remaining parameters are similar to fetch()'s,
//! except that each fetched value is std::pair of two vectors, with the
//! value for that column, in each row, placed into the first vector,
//! and the corresponding null flag into the second
//! \c std::vector<INSERT_LIBX_NAMESPACE::sql::bitflag> vector.
//!
//! \code
//! std::vector<std::string> titles;
//!
//! size_t cnt=stmt->fetch_vectors(1000, "title", titles);
//! \endcode
//!
//! Specify a single vector, instead of a pair of vectors, if the null flags
//! are not required.
//!
//! fetch_vectors() resizes each vector parameter to the row count, and
//! fetches up to the given number of rows, placing the value in each column,
//! and the null flag indicator (if specified),
//! into the corresponding number of vectors, and returns the number of rows
//! fetched, which may be less than requested (and 0 when there are no more
//! rows to fetch).
//!
//! The \c bitflag vector should be treated as an equivalent of a \c bool
//! vector, for checking whether the corresponding value of that column in the
//! row is \c NULL.
//!
//! \code
//! std::map<std::string, std::pair<std::vector<std::string>, std::vector<INSERT_LIBX_NAMESPACE::bitflag>>> columns;
//!
//! // ...
//!
//! size_t cnt=stmt->fetch_vectors(1000, columns);
//! \endcode
//!
//! This is similar to indirectly specifying the fetched columns via a map,
//! with fetch(). Each map's value is a vector of column types, and
//! corresponding \c NULL indicators. Again, the \c std::pair and the
//! second NULL indication vector, are optional.
//!
//! \code
//! std::pair<std::vector<std::string>
//!           std::vector<INSERT_LIBX_NAMESPACE::sql::bitflag> titles;
//!
//! stmt->fetch_vectors_all(1000,
//!    [&]
//!    (size_t cnt)
//!    {
//!        // ...
//!    }, "title", titles);
//! \endcode
//!
//! fetch_vector_all() is similar to fetch_vectors(), except that the first
//! parameter is the row count, the second parameter is a lambda, and
//! the remaining parameters are the same as fetch_vectors().
//!
//! fetch_vectors_all() calls fetch_vectors() repeatedly, until it returns 0.
//! With scrolling cursors, this is equivalent to specifying
//! \c INSERT_LIBX_NAMESPACE::sql::fetch::next.
//! After each call, except for the last one that returns 0, the lambda
//! gets invoked, with the return value from fetch_vectors() as its sole
//! parameter.
//!
//! The lambda cannot resize any vectors being fetched.
//!
//! \par Scrolling cursors
//!
//! A scrolling cursor takes an optional last argument to fetch() that
//! specifies the scroll direction:
//!
//! \code
//! float price;
//! std::vector<float> prices;
//!
//! stmt->fetch("prices", price, INSERT_LIBX_NAMESPACE::sql::fetch::next);
//!
//! size_t cnt=stmt->fetch_vectors(1000, "prices", prices, INSERT_LIBX_NAMESPACE::sql::fetch::next);
//! \endcode
//!
//! This is the default action, if omitted, that fetches the next row or
//! vector of rows from the cursor.
//!
//! \code
//! float price;
//! std::vector<float> prices;
//!
//! stmt->fetch("prices", price, INSERT_LIBX_NAMESPACE::sql::fetch::prior);
//!
//! size_t cnt=stmt->fetch_vectors(1000, "prices", prices, INSERT_LIBX_NAMESPACE::sql::fetch::prior);
//! \endcode
//!
//! \c INSERT_LIBX_NAMESPACE::sql::fetch::prior fetches the previous row or
//! vector of rows from the cursor.
//!
//! \code
//! float price;
//! std::vector<float> prices;
//!
//! stmt->fetch("prices", price, INSERT_LIBX_NAMESPACE::sql::fetch::first);
//!
//! size_t cnt=stmt->fetch_vectors(1000, "prices", prices, INSERT_LIBX_NAMESPACE::sql::fetch::first);
//! \endcode
//!
//! \c INSERT_LIBX_NAMESPACE::sql::fetch::first fetches the first row or vector
//! of rows from the cursor.
//!
//! \code
//! float price;
//! std::vector<float> prices;
//!
//! stmt->fetch("prices", price, INSERT_LIBX_NAMESPACE::sql::fetch::last);
//!
//! size_t cnt=stmt->fetch_vectors(1000, "prices", prices, INSERT_LIBX_NAMESPACE::sql::fetch::last);
//! \endcode
//!
//! \c INSERT_LIBX_NAMESPACE::sql::fetch::last fetches the last row or vector of
//! rows from the cursor.
//!
//! \code
//! float price;
//! std::vector<float> prices;
//!
//! stmt->fetch("prices", price, INSERT_LIBX_NAMESPACE::sql::fetch::absolute(100));
//!
//! size_t cnt=stmt->fetch_vectors(1000, "prices", prices, INSERT_LIBX_NAMESPACE::sql::fetch::absolute(0));
//! \endcode
//!
//! \c INSERT_LIBX_NAMESPACE::sql::fetch::absolute(n) fetches a row or vector of
//! rows starting with the given absolute row number from the cursor.
//! The row numbers are 0-based.
//!
//! \code
//! float price;
//! std::vector<float> prices;
//!
//! stmt->fetch("prices", price, INSERT_LIBX_NAMESPACE::sql::fetch::relative(-1));
//!
//! size_t cnt=stmt->fetch_vectors(1000, "prices", prices, INSERT_LIBX_NAMESPACE::sql::fetch::relative(10));
//! \endcode
//!
//! \c INSERT_LIBX_NAMESPACE::sql::fetch::relative(n) fetches a row or vector of
//! rows starting with the given offset from the start of the current resultset
//! row or row vector.
//!
//! \code
//! float price;
//! std::vector<float> prices;
//!
//! INSERT_LIBX_NAMESPACE::sql::bookmark bookmark;
//!
//! std::vector<INSERT_LIBX_NAMESPACE::sql::bookmark> bookmarks;
//!
//! auto stmt=conn->create_newstatement("BOOKMARKS", "ON", "CURSOR_TYPE", "STATIC")->execute("SELECT prices FROM books order by stock_id");
//!
//! stmt->fetch(bookmark, "price", price);
//!
//! stmt->fetch_vectors(100, bookmarks, "price", price);
//!
//! stmt->fetch("prices", price, INSERT_LIBX_NAMESPACE::sql::fetch::atbookmark(bookmark, -1));
//!
//! size_t cnt=stmt->fetch_vectors(1000, "prices", prices, INSERT_LIBX_NAMESPACE::sql::fetch::atbookmark(bookmark));
//! \endcode
//!
//! \ref bookmark "INSERT_LIBX_NAMESPACE::sql::bookmark" is an opaque object
//! that stores a row's "bookmark". It's optionally passed as the first
//! lvalue parameter to fetch(); or a vector of bookmarks gets passed as the
//! first vector to fetch_fectors(), which puts a bookmark for each row in the
//! fetched vector into the corresponding bookmark vector.
//!
//! \c INSERT_LIBX_NAMESPACE::sql::fetch::atbookmark fetches a row or vector of
//! rows starting with the previously-obtained bookmark row, with an optional
//! relative offset.
//!
//! \par Positioned UPDATEs and DELETEs
//!
//! \code
//! stmt->modify_fetched_row(0, "UPDATE memos SET memo_text=?", "New memo text");
//! \endcode
//!
//! UPDATEs or DELETEs the contents of one row in the last resultset returned
//! from fetch() or fetch_vectors().
//! The first parameter is a 0-based row number, which is always zero with
//! single row resultsets obtained from fetch.
//! Specify a 0-based row number to reference the appropriate row
//! in a multi-result resultset from fetch_vectors().
//! The second parameter is an
//! \c UPDATE or \c DELETE, with
//! the remaining parameters, if any, specifying any parameter values;
//! any parameter value that's accepted by
//! execute.
//!
//! \par SQL batches, row counts
//!
//! \code
//! size_t row_count=stmt->row_count();
//!
//! bool flag=stmt->more();
//! \endcode
//!
//! row_count() returns the number of rows modified by the executed non-SELECT
//! SQL statement (an UPDATE or a DELETE). When the executed SQL contains more
//! than one statement, the first call to row_count(), fetch(), or
//! fetch_vectors() reports the results of the first statement in the batch.
//! Use more() to advance to the next statement in the batch(). Afterwards,
//! row_count(), fetch(), and fetch_vectors() use the results of the next
//! statement in the batch.
//!
//! more() returns \c true if the statement succeeds, or \c false if there
//! are no more statements in the batch. Semantics of error handling depend
//! on the database driver. Some database drivers throw an exception from
//! execute() if any statement in the batch had an error. Other database
//! drivers throw an exception from execute() if the first statement in the
//! batch had an error, and more() throws an exception if the next statement
//! in the batch had an error. If the executed statement contains a batch,
//! this means that a thrown exception from execute() (and perhaps
//! execute_vector() as well) and more() still leaves the statement object
//! in a usable state for additional calls to more().
//!
//! It also means that the SQL batch must be explicitly prepare()d, in order
//! to obtain the statement handle; then executed. Invoking
//! execute() on a connection object, with an implicit prepare() means that
//! the statement handle will not be available if execute() throws an
//! exception.

typedef ref<statementObj, statementBase> statement;

//! A nullable pointer reference to an \ref statement "SQL statement".

typedef ptr<statementObj, statementBase> statementptr;

//! A reference to a constant \ref statement "SQL statement".

typedef const_ref<statementObj, statementBase> const_statement;

//! A nullable pointer reference to a constant \ref statement "SQL statement".

typedef const_ptr<statementObj, statementBase> const_statementptr;

#if 0
{
	{
#endif
	}
}
#endif
