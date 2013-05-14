/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_connectionfwd_H
#define x_sql_connectionfwd_H

#include <x/ptrfwd.H>

namespace LIBCXX_NAMESPACE {

	namespace sql {
#if 0
	};
};
#endif

class connectionObj;
class connectionBase;

//! A connection to an SQL database

//! This is a \ref ref "reference to a reference-counted object" that represents
//! an SQL connection. It gets created by invoking connect() on an
//! \ref env "environment handle".
//!
//! \par Preparing a new statement
//!
//! \code
//! INSERT_LIBX_NAMESPACE::sql::connection conn=INSERT_LIBX_NAMESPACE::sql::env::create()->connect("DSN=dev");
//!
//! INSERT_LIBX_NAMESPACE::sql::statement stmt=conn->prepare("SELECT * from books WHERE title=?");
//!
//! std::string title;
//!
//! INSERT_LIBX_NAMESPACE::sql::statement stmt=conn->execute("SELECT * from books WHERE title=?", title);
//!
//! std::vector<std::string> titles;
//! std::vector<double> prices;
//! std::vector<INSERT_LIBX_NAMESPACE::sql::bitflag> status;
//! 
//! INSERT_LIBX_NAMESPACE::sql::statement stmt=conn->execute_vector("INSERT INTO books(title, prices) VALUES(?, ?)", status, titles, prices);
//! \endcode
//!
//! prepare() returns
//! a \ref statement "statement handle" that represents an SQL
//! statement. An exception gets thrown if there was an error in the SQL
//! statement's syntax.
//!
//! execute() is equivalent to calling prepare() with the first argument,
//! then execute() on the resulting statement handle, with any remaining
//! arguments.
//!
//! execute_vector() is equivalent to calling prepare() with the first argument,
//! then execute_vector() on the resulting statement handle, with the remaining
//! arguments.
//!
//! \par Disconnecting
//!
//! \code
//! conn->disconnect();
//! \endcode
//!
//! The connection handle is a
//! \ref ref "reference to a reference-counted object", and the connection
//! with the database server gets automatically closed when the last
//! reference to connection object goes out of scope and gets destroyed.
//! disconnect() explicitly close the connection. It is no longer valid.
//!
//! \par Transactions
//!
//! \code
//! conn->autocommit(false);
//! conn->commit();
//! conn->rollback();
//! conn->autocommit(true);
//!
//! conn->begin_work();
//! conn->commit_work();
//! conn->rollback_work();
//! \endcode
//!
//! autocommit() sets the connection's autocommit flag: whether all succesfully
//! executed statements get automatically commited. If \c false, then
//! an explicit commit() commits all changes, or rollback() rolls them back;
//! then the next successfully executed statement opens another transaction.
//!
//! Most databases do not support certain statements, like schema changes, from
//! being a part of a transaction.
//!
//! Both commit() and rollback() take an optional \c bool parameter. If
//! set to true, the autocommit flag gets turned on automatically, as part of
//! the commit or rollback.
//!
//! autocommit(), commit(), and rollback() use the underlying ODBC transaction
//! API. Most current database drivers implement START TRANSACTION and
//! SAVEPOINT SQL that allows for nested logical transactions.
//!
//! begin_work(), commit_work(), and rollback_work() implement logical
//! transactions that can be nested. begin_work() opens a transaction,
//! until it's closed by commit_work() or rollback_work(). While a transaction
//! is open, invoking begin_work() again opens a nested transaction.
//!
//! The next call to commit_work() or rollback_work() closes the nested
//! transaction. The nested transaction's changes get either commited to
//! the original transaction, or undone, leaving the original transaction in
//! its state prior to the nested transaction's start. The original transaction
//! remains open until its own matching commit_work() or rollback_work().
//!
//! \note
//!
//! In addition to throwing an exception, a failed commit_work() or
//! rollback_work() automatically disconnect()s, since the database connection's
//! state can no longer be determined.
//!
//! See also: \ref transaction "INSERT_LIBX_NAMESPACE::sql::transaction"
//!
//! \par Catalog functions
//!
//! \code
//! std::string catalog, schema, table, type;
//!
//! INSERT_LIBX_NAMESPACE::sql::statement stmt=conn->tables(catalog, schema, table, type);
//!
//! std::string table_name;
//! bool ignore;
//!
//! while (stmt->fetch("table_name", table_name, ignore))
//! {
//!     std::cout << table_name << std::endl;
//! }
//! \endcode
//!
//! tables() enumerates accessible tables on the database server. The
//! catalog, schema, table, and type parameters are optional. If specified,
//! they get interpreted by a database server.
//!
//! tables() returns a statement handle with a resultset. Process the SQL
//! statement as if it was an execute()d statement with a resultset. The
//! resultset enumerates the available tables.

typedef ref<connectionObj, connectionBase> connection;

//! A nullable pointer reference to an \ref connection "SQL connection".

typedef ptr<connectionObj, connectionBase> connectionptr;

//! A reference to a constant \ref connection "SQL connection".

typedef const_ref<connectionObj, connectionBase> const_connection;

//! A nullable pointer reference to a constant \ref connection "SQL connection".

typedef const_ptr<connectionObj, connectionBase> const_connectionptr;

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
