/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/sql/env.H"
#include "x/sql/connection.H"
#include "x/sql/statement.H"
#include "x/sql/exception.H"
#include "x/options.H"
#include "x/join.H"
#include <iostream>
#include <iomanip>
#include <algorithm>

static std::set<std::string>
get_tables(const LIBCXX_NAMESPACE::sql::connection &conn,
	   bool flag,
	   const std::string &pattern)
{
	auto tables=conn->tables(flag, "", "", pattern);

	std::set<std::string> names;

	std::string table_name;

	while (tables->fetch("table_name", table_name))
	{
		names.insert(table_name);
	}

	return names;
}

static void testcursortype(const LIBCXX_NAMESPACE::sql::connection &conn1,
			   const LIBCXX_NAMESPACE::sql::connection &conn2,
			   const LIBCXX_NAMESPACE::sql::statement &stmt,
			   const LIBCXX_NAMESPACE::sql::config_bitmask_t &a1,
			   const LIBCXX_NAMESPACE::sql::config_bitmask_t &a2)
{
	std::cout << LIBCXX_NAMESPACE::join(a1, ", ") << std::endl;
	std::cout << LIBCXX_NAMESPACE::join(a2, ", ") << std::endl;

	if (a1.count("SQL_CA1_ABSOLUTE"))
	{
		int n;

		if (!stmt->fetch("intkey", n, LIBCXX_NAMESPACE::sql::fetch::last)
		    || n != 9)
			throw EXCEPTION("fetch(last) failed");

		if (!stmt->fetch("intkey", n, LIBCXX_NAMESPACE::sql::fetch::absolute(2))
		    || n != 2)
			throw EXCEPTION("fetch(absolute) failed");

		if (!stmt->fetch("intkey", n, LIBCXX_NAMESPACE::sql::fetch::first)
		    || n != 0)
			throw EXCEPTION("fetch(last) failed");
		std::cout << "Absolute ok!" << std::endl;
	}
	else
	{
		int n;

		if (!stmt->fetch("intkey", n, LIBCXX_NAMESPACE::sql::fetch::next)
		    || n != 0)
			throw EXCEPTION("fetch(next) failed");
	}

	if (a1.count("SQL_CA1_RELATIVE"))
	{
		int n;

		if (!stmt->fetch("intkey", n, LIBCXX_NAMESPACE::sql::fetch::relative(1))
		    || n != 1)
			throw EXCEPTION("fetch(relative 1) failed");

		if (!stmt->fetch("intkey", n, LIBCXX_NAMESPACE::sql::fetch::relative(1))
		    || n != 2)
			throw EXCEPTION("fetch(relative 1) failed");

		if (!stmt->fetch("intkey", n, LIBCXX_NAMESPACE::sql::fetch::relative(-1))
		    || n != 1)
			throw EXCEPTION("fetch(relative -1) failed");

		if (!stmt->fetch("intkey", n, LIBCXX_NAMESPACE::sql::fetch::prior)
		    || n != 0)
			throw EXCEPTION("fetch(prior) failed");

		std::cout << "Relative ok!" << std::endl;
	}

	if (a1.count("SQL_CA1_BOOKMARK"))
	{
		int n;

		LIBCXX_NAMESPACE::sql::bookmark bookmark0;

		if (!stmt->fetch(bookmark0, "intkey", n)
		    || n != 1)
			throw EXCEPTION("fetch(relative) failed");

		std::vector<LIBCXX_NAMESPACE::sql::bookmark> bookmarks5;
		std::vector<std::string> rows;

		if (stmt->fetch_vectors(4, bookmarks5, "intkey", rows) != 4 ||
		    LIBCXX_NAMESPACE::join(rows, "|") != "2|3|4|5")
			throw EXCEPTION("Did not retrieve rows 2-5");

		if (!stmt->fetch("intkey", n,
				 LIBCXX_NAMESPACE::sql::fetch::atbookmark(bookmark0, -1))
		    || n != 0)
			throw EXCEPTION("Bookmark fetch failed!\n");

		std::cout << "Bookmark ok!" << std::endl;
	}
}

static void forward_cursor(const LIBCXX_NAMESPACE::sql::connection &conn1,
			   const LIBCXX_NAMESPACE::sql::connection &conn2,
			   const LIBCXX_NAMESPACE::sql::statement &stmt,
			   const LIBCXX_NAMESPACE::sql::config_bitmask_t &a1,
			   const LIBCXX_NAMESPACE::sql::config_bitmask_t &a2)
{
	testcursortype(conn1, conn2, stmt, a1, a2);
}

static void static_cursor(const LIBCXX_NAMESPACE::sql::connection &conn1,
			  const LIBCXX_NAMESPACE::sql::connection &conn2,
			  const LIBCXX_NAMESPACE::sql::statement &stmt,
			  const LIBCXX_NAMESPACE::sql::config_bitmask_t &a1,
			  const LIBCXX_NAMESPACE::sql::config_bitmask_t &a2)
{
	testcursortype(conn1, conn2, stmt, a1, a2);
}

static void keyset_cursor(const LIBCXX_NAMESPACE::sql::connection &conn1,
			  const LIBCXX_NAMESPACE::sql::connection &conn2,
			  const LIBCXX_NAMESPACE::sql::statement &stmt,
			  const LIBCXX_NAMESPACE::sql::config_bitmask_t &a1,
			  const LIBCXX_NAMESPACE::sql::config_bitmask_t &a2)
{
	testcursortype(conn1, conn2, stmt, a1, a2);
}

void testcursor(const std::string &connection,
		int flags)
{
	auto env=LIBCXX_NAMESPACE::sql::env::create();
	auto conn=env->connect(connection,
			       (LIBCXX_NAMESPACE::sql::connect_flags)flags)
		.first;
	{
		auto tables=get_tables(conn, false, "temptbl%");

		for (const auto &table_name:tables)
		{
			conn->execute("drop table " + table_name);
		}
	}

	conn->execute("create table temptbl(intkey int not null, strval varchar(255) not null, primary key(intkey))");

	{
		std::vector<LIBCXX_NAMESPACE::sql::bitflag> flags;
		std::vector<int> n={0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

		flags.resize(n.size());

		conn->execute_vector("INSERT INTO temptbl VALUES(?, ?)",
				     flags, n, n);
	}

	auto conn2=env
		->connect(connection,
			  (LIBCXX_NAMESPACE::sql::connect_flags)flags).first;

	auto supported_cursor_types=conn->config_get_scroll_options();

	bool has_bookmarks=!conn->config_get_bookmark_persistence().empty();

	std::map<std::string, std::string> opts;

	if (has_bookmarks)
		opts["BOOKMARKS"]="ON";

	std::cout << "Forward cursor" << std::endl;
	if (supported_cursor_types.count("SQL_SO_FORWARD_ONLY"))
	{
		forward_cursor(conn, conn2,
			       conn->create_newstatement("CURSOR_TYPE",
							 "FORWARD",
							 opts)
			       ->execute("SELECT intkey, strval FROM temptbl"),
			       conn->config_get_forward_only_cursor_attributes1(),
			       conn->config_get_forward_only_cursor_attributes2());
	}

	std::cout << "Static cursor" << std::endl;
	if (supported_cursor_types.count("SQL_SO_STATIC"))
	{
		static_cursor(conn, conn2,
			      conn->create_newstatement("CURSOR_TYPE",
							"STATIC",
							opts)
			      ->execute("SELECT intkey, strval FROM temptbl"),
			      conn->config_get_static_cursor_attributes1(),
			      conn->config_get_static_cursor_attributes2());
	}

	std::cout << "Dynamic cursor" << std::endl;
	if (supported_cursor_types.count("SQL_SO_KEYSET_DRIVEN"))
	{
		keyset_cursor(conn, conn2,
			      conn->create_newstatement("CURSOR_TYPE",
							"KEYSET(100)",
							opts)
			      ->execute("SELECT intkey, strval FROM temptbl"),
			      conn->config_get_keyset_cursor_attributes1(),
			      conn->config_get_keyset_cursor_attributes2());
	}
}

int main(int argc, char **argv)
{
	LIBCXX_NAMESPACE::option::list
		options(LIBCXX_NAMESPACE::option::list::create());

	LIBCXX_NAMESPACE::option::int_value flags_value(LIBCXX_NAMESPACE::option::int_value::create((int)LIBCXX_NAMESPACE::sql::connect_flags::noprompt));

	LIBCXX_NAMESPACE::option::string_value connect_value(LIBCXX_NAMESPACE::option::string_value::create());

	options->add(connect_value, 'c', L"connect",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     L"Make a test connection",
		     L"data_source")
		.add(flags_value, 'f', L"flags",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     L"Connection flag",
		     L"flag");

	options->addDefaultOptions();

	LIBCXX_NAMESPACE::option::parser
		parser(LIBCXX_NAMESPACE::option::parser::create());

	parser->setOptions(options);

	if (parser->parseArgv(argc, argv) ||
	    parser->validate())
		exit(1);

	try {
		if (connect_value->isSet())
		{
			testcursor(connect_value->value,
				   flags_value->value);
		}
	} catch (const LIBCXX_NAMESPACE::exception &e) {
		std::cout << e << std::endl;
		exit(1);
	}
	return 0;
}

