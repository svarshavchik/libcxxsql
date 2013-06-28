/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/sql/env.H"
#include "x/sql/connection.H"
#include "x/sql/statement.H"
#include "x/sql/dbi/constraint.H"
#include <x/options.H>
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

void testdbi(const std::string &connection,
	     int flags)
{
	auto env=LIBCXX_NAMESPACE::sql::env::create();
	env->set_login_timeout(10);
	auto conn=env->connect(connection,
			       (LIBCXX_NAMESPACE::sql::connect_flags)flags)
		.first;
	try {
		conn->execute("set foreign_key_checks=0");
	} catch (...) {
	}
	{
		auto tables=get_tables(conn, false, "temptbl%");

		for (const auto &table_name:tables)
		{
			conn->execute("drop table " + table_name);
		}
	}
	try {
		conn->execute("set foreign_key_checks=1");
	} catch (...) {
	}

	conn->execute("CREATE TABLE temptbl1(a int not null, b int null, c int, d int)");

	{
		auto insert=conn->prepare("INSERT INTO temptbl1 VALUES(?, ?, ?, ?)");
		insert->execute(4, 1, 2, 1);
		insert->execute(4, nullptr, 2, 2);
		insert->execute(4, 2, 3, 3);
	}

	{
		auto first_constraint=LIBCXX_NAMESPACE::sql::dbi::constraint
			::create("c", "=", "2");

		auto constraint=LIBCXX_NAMESPACE::sql::dbi
			::AND("a", "=", std::make_pair(4, LIBCXX_NAMESPACE
						       ::sql::bitflag(0)),
			      first_constraint,
			      "b", "=", nullptr);

		int value=0;

		std::ostringstream o;

		o << "SELECT d FROM temptbl1 WHERE " << constraint
		  << " ORDER BY d";

		std::cout << o.str() << std::endl;
		auto stmt=conn->execute(o.str(), constraint);

		if (!stmt->fetch("d", value) || value != 2
		    || stmt->fetch("d", value))
			throw EXCEPTION("AND test 1 failed");
	}

	{
		std::vector<LIBCXX_NAMESPACE::sql::dbi::constraint> constraints;

		constraints.push_back(LIBCXX_NAMESPACE::sql::dbi::constraint::create("a", "!=", 3));

		constraints.push_back(LIBCXX_NAMESPACE::sql::dbi::constraint::create("b", "!=", nullptr));

		auto constraint=LIBCXX_NAMESPACE::sql::dbi::AND(constraints,
								"c", "=", 2);

		int value=0;

		std::ostringstream o;

		o << "SELECT d FROM temptbl1 WHERE ";

		constraint->get_sql(o);

		o << " ORDER BY d";

		std::cout << o.str() << std::endl;
		auto stmt=conn->execute(o.str(), constraint);

		if (!stmt->fetch("d", value) || value != 1
		    || stmt->fetch("d", value))
			throw EXCEPTION("AND test 2 failed");
	}

	{
		auto constraint=LIBCXX_NAMESPACE::sql::dbi
			::constraint::create("a", "=", 4,
					     "b", ">", nullptr,
					     "c", "=", 2);

		int value=0;

		std::ostringstream o;

		o << "SELECT d FROM temptbl1 WHERE " << constraint
		  << " ORDER BY d";

		std::cout << o.str() << std::endl;
		auto stmt=conn->execute(o.str(), constraint);

		if (stmt->fetch("d", value))
			throw EXCEPTION("AND test 1 failed");
	}


	{
		auto constraint=LIBCXX_NAMESPACE::sql::dbi
			::AND("a", "!=", 3,
			      "b", "!=",
			      std::make_pair(0, LIBCXX_NAMESPACE::sql::bitflag
					     (1)),
			      "c", "=", 2);

		int value=0;

		std::ostringstream o;

		o << "SELECT d FROM temptbl1 WHERE "
		  << constraint << " ORDER BY d";

		std::cout << o.str() << std::endl;
		auto stmt=conn->execute(o.str(), constraint);

		if (!stmt->fetch("d", value) || value != 1
		    || stmt->fetch("d", value))
			throw EXCEPTION("AND test 2 failed");
	}


	{
		auto constraint=LIBCXX_NAMESPACE::sql::dbi::OR("b", "=", 1,
							       "c", "=", 3);

		int value=0;

		std::ostringstream o;

		o << "SELECT d FROM temptbl1 WHERE "
		  << constraint << " ORDER BY d";

		std::cout << o.str() << std::endl;
		auto stmt=conn->execute(o.str(), constraint);

		if (!stmt->fetch("d", value) || value != 1
		    || !stmt->fetch("d", value) || value != 3)
			throw EXCEPTION("OR test 3 failed");
	}

	{
		std::vector<int> keys={1,2};
		auto constraint=LIBCXX_NAMESPACE::sql::dbi::AND("b", "=", keys);

		int value=0;

		std::ostringstream o;

		o << "SELECT d FROM temptbl1 WHERE "
		  << constraint << " ORDER BY d";

		std::cout << o.str() << std::endl;
		auto stmt=conn->execute(o.str(), constraint);

		if (!stmt->fetch("d", value) || value != 1
		    || !stmt->fetch("d", value) || value != 3)
			throw EXCEPTION("IN test 1 failed");
	}

	{
		std::vector<int> keys={1,2};
		auto constraint=LIBCXX_NAMESPACE::sql::dbi::AND("b", "!=", keys);

		int value=0;

		std::ostringstream o;

		o << "SELECT d FROM temptbl1 WHERE "
		  << constraint
		  << " ORDER BY d";

		std::cout << o.str() << std::endl;
		auto stmt=conn->execute(o.str(), constraint);

		if (stmt->fetch("d", value))
			throw EXCEPTION("NOT IN test 1 failed");
	}

	{
		auto keys=LIBCXX_NAMESPACE::vector<int>::create();
		auto constraint=LIBCXX_NAMESPACE::sql::dbi::AND("b", "=", keys);

		int value=0;

		std::ostringstream o;

		o << "SELECT d FROM temptbl1 WHERE "
		  << constraint << " ORDER BY d";

		std::cout << o.str() << std::endl;
		auto stmt=conn->execute(o.str(), constraint);

		if (stmt->fetch("d", value))
			throw EXCEPTION("IN test 2 failed");
	}

	{
		auto keys=LIBCXX_NAMESPACE::const_vector<int>::create();
		auto constraint=LIBCXX_NAMESPACE::sql::dbi::AND("b", "!=", keys);

		LIBCXX_NAMESPACE::vector<int> cpy=keys;

		int value=0;

		std::ostringstream o;

		o << "SELECT d FROM temptbl1 WHERE "
		  << constraint << " ORDER BY d";

		std::cout << o.str() << std::endl;
		auto stmt=conn->execute(o.str(), constraint);

		if (!stmt->fetch("d", value) || value != 1
		    || !stmt->fetch("d", value) || value != 3)
			throw EXCEPTION("NOT IN test 2 failed");
	}

	{
		auto constraint=LIBCXX_NAMESPACE::sql::dbi::NOT("c", "!=", 3,
								"b", "=", 1);

		int value=0;

		std::ostringstream o;

		o << "SELECT d FROM temptbl1 WHERE "
		  << constraint << " ORDER BY d";

		std::cout << o.str() << std::endl;
		auto stmt=conn->execute(o.str(), constraint);

		if (!stmt->fetch("d", value) || value != 3)
			throw EXCEPTION("NOT test failed");
	}

	std::cout << x::sql::dbi::AND("key", "=", 4,
				      x::sql::dbi::OR("category", "=", "memo",
                                                      "category", "=", "note"),
                                      "discarded", "=", 0)
		  << std::endl;
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
			testdbi(connect_value->value, flags_value->value);
		}
	} catch (const LIBCXX_NAMESPACE::exception &e) {
		std::cout << e << std::endl;
		std::cout << e->backtrace;
		exit(1);
	}
	return 0;
}

