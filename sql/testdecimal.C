/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include <x/options.H>
#include "x/sql/env.H"
#include "x/sql/connection.H"
#include "x/sql/statement.H"
#include "x/sql/decimal.H"

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

void droptables(const LIBCXX_NAMESPACE::sql::connection &conn)
{
	auto tables=get_tables(conn, false, "temptbl%");

	try {
		conn->execute("set foreign_key_checks=0");
	} catch (...) {
	}
	for (const auto &table_name:tables)
	{
		conn->execute("drop table " + table_name + " cascade");
	}
	try {
		conn->execute("set foreign_key_checks=1");
	} catch (...)
	{
	}
}

void testdecimal(const std::string &connection,
		 int flags)
{
	auto env=LIBCXX_NAMESPACE::sql::env::create();
	env->set_login_timeout(10);
	auto conn=env->connect(connection,
			       (LIBCXX_NAMESPACE::sql::connect_flags)flags)
		.first;

	droptables(conn);

	// Create schema

	conn->execute("create table temptbl_decimal(n int, balance decimal(18,6) null)");

	conn->execute("INSERT INTO temptbl_decimal values (?,?)",
		      0, 1.20_mpf);


	std::vector<LIBCXX_NAMESPACE::sql::bitflag> status;
	status.resize(13);

	std::vector<int> n={1,2,3,4,5,6,7,8,9,10,11,12,13};

	std::pair<std::vector<mpf_class>,
		  std::vector<LIBCXX_NAMESPACE::sql::bitflag> > vector={
		{0_mpf,
		 0_mpf, 100_mpf, 1000000000_mpf, .1_mpf, .01_mpf, .000002_mpf,
		 -100_mpf, -1000000000_mpf, -.1_mpf, -.01_mpf, -.000002_mpf,
		 -123456789012.345678_mpf},
		{1,
		 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0,
		}};

	conn->execute_vector("INSERT INTO temptbl_decimal values (?,?)",
			     status, n, vector);

	std::pair<std::vector<mpf_class>,
		  std::vector<LIBCXX_NAMESPACE::sql::bitflag>> rows;

	if (conn->execute("SELECT balance FROM temptbl_decimal ORDER BY n")
	    ->fetch_vectors(20, 0, rows) != 14)
		throw EXCEPTION("Did not receive 14 rows");

	vector.first.insert(vector.first.begin(), 1.2_mpf);
	vector.second.insert(vector.second.begin(), 0);
	rows.first[1]=0_mpf;

	rows.first.resize(14);
	rows.second.resize(14);

	if (vector.second != rows.second)
		throw EXCEPTION("Null indicators did not match");
	if (vector.first != rows.first)
		throw EXCEPTION("Values did not match");
}

int main(int argc, char **argv)
{
	LIBCXX_NAMESPACE::option::list
		options(LIBCXX_NAMESPACE::option::list::create());

	LIBCXX_NAMESPACE::option::int_value flags_value(LIBCXX_NAMESPACE::option::int_value::create((int)LIBCXX_NAMESPACE::sql::connect_flags::noprompt));

	LIBCXX_NAMESPACE::option::string_value connect_value(LIBCXX_NAMESPACE::option::string_value::create());

	options->add(connect_value, 'c', "connect",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     "Make a test connection",
		     "data_source")
		.add(flags_value, 'f', "flags",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     "Connection flag",
		     "flag");

	options->addDefaultOptions();

	LIBCXX_NAMESPACE::option::parser
		parser(LIBCXX_NAMESPACE::option::parser::create());

	parser->setOptions(options);

	if (parser->parseArgv(argc, argv) ||
	    parser->validate())
		exit(1);

	try {
		if (connect_value->is_set())
		{
			testdecimal(connect_value->value, flags_value->value);
		}
	} catch (const LIBCXX_NAMESPACE::exception &e) {
		std::cout << e << std::endl;
		exit(1);
	}
	return 0;
}
