/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/sql/env.H"
#include "x/sql/connection.H"
#include "x/sql/statement.H"
#include <x/options.H>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <map>
#include <set>
#include <stdint.h>
#include "testschematbl.H"
#include "testschematbl.C"

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

void testschema(const std::string &connection,
		int flags)
{
	auto env=LIBCXX_NAMESPACE::sql::env::create();
	env->set_login_timeout(10);
	auto conn=env->connect(connection,
			       (LIBCXX_NAMESPACE::sql::connect_flags)flags)
		.first;

	droptables(conn);

	// Create schema

	conn->execute("create table temptbl_accounts(account_id integer not null, account_type_id integer not null, account_category_id integer not null, name varchar(255) null, primary key(account_id))");

	conn->execute("create table temptbl_transactions(transaction_id integer not null, transaction_type_id integer not null, transaction_date date not null, primary key(transaction_id))");

	conn->execute("create table temptbl_ledger_entries(ledger_entry_id integer not null, transaction_id integer not null, account_id integer not null, ledger_date date not null, amount numeric(11,2) not null, primary key(ledger_entry_id))");

	conn->execute("alter table temptbl_ledger_entries add foreign key(transaction_id) references temptbl_transactions(transaction_id)");

	conn->execute("alter table temptbl_ledger_entries add foreign key(account_id) references temptbl_accounts(account_id)");

	conn->execute("create table temptbl_account_types(account_type_id integer not null, account_category_id integer not null, name varchar(255) not null, primary key(account_type_id, account_category_id))");

	conn->execute("create table temptbl_transaction_types(transaction_type_id integer not null, name varchar(255) not null, primary key(transaction_type_id))");

	conn->execute("alter table temptbl_accounts add foreign key(account_type_id, account_category_id) references temptbl_account_types(account_type_id, account_category_id)");

	conn->execute("alter table temptbl_transactions add foreign key(transaction_type_id) references temptbl_transaction_types(transaction_type_id)");

	auto account_types=temptbl_account_types::create(conn);
}

#include "exampleschema1.H"
#include "exampleschema1.C"

void example1_compiles(const LIBCXX_NAMESPACE::sql::connection &conn)
{
	examples::example1::accounts accounts=
		examples::example1::accounts::create(conn);

	for (examples::example1::accounts::base::iterator
		     b=accounts->begin(),
		     e=accounts->end(); b != e; ++b)
	{
		examples::example1::accounts::base::row row=*b;

		std::cout << row->account_id.value() << std::endl;

		if (!row->name.isnull())
			std::cout << row->name.value() << std::endl;

		std::cout << row->balance.value() << std::endl;
	}

	for (auto const &row: *accounts)
	{
		std::cout << row->account_id.value() << std::endl;

		if (!row->name.isnull())
			std::cout << row->name.value() << std::endl;
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
			testschema(connect_value->value, flags_value->value);
		}
	} catch (const LIBCXX_NAMESPACE::exception &e) {
		std::cout << e << std::endl;
		exit(1);
	}
	return 0;
}

