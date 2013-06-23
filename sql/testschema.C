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

#include "exampleschema2.H"
#include "exampleschema2.C"

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

	// Insert into temptbl_account_types
	//
	// 1, 1, Type 1
	// 2, 2, Type 2

	auto account_types_rs=account_types::create(conn);

        conn->execute("insert into temptbl_account_types values(1, 1, 'Type 1')");
        conn->execute("insert into temptbl_account_types values(2, 2, 'Type 2')");

	{
		std::map<int, std::string> account_types_fetched;

		for (const auto &account_type:*account_types_rs)
		{
			account_types_fetched[account_type->account_type_id.value()]=account_type->name.value();
		}

		if (account_types_fetched != std::map<int, std::string>({
					{1, "Type 1"},
					{2, "Type 2"}}))
			throw EXCEPTION("SELECT * failed");
	}

	account_types_rs->search(LIBCXX_NAMESPACE::sql::dbi
			      ::OR("account_type_id", "=", 1,
				   "name", "!=", "Type 2"));

	{
		std::map<int, std::string> account_types_fetched;

		for (account_types::base::iterator
			     b=account_types_rs->begin(), e=account_types_rs->end();
		     b != e; ++b)
		{
			account_types::base::row row=*b;

			account_types_fetched[row->account_type_id.value()]=row->name.value();
		}

		if (account_types_fetched != std::map<int, std::string>({
					{1, "Type 1"}}))
			throw EXCEPTION("SELECT * failed");
	}

	// Insert into accounts, account 1 and 2.

        conn->execute("insert into temptbl_accounts values(1, 1, 1, 'Account 1')");
        conn->execute("insert into temptbl_accounts values(2, 2, 2, 'Account 2')");

	{
		auto accounts=accounts::create(conn);

		account_types::base::joins j=accounts->join_account_types();

		accounts->search(j->get_table_alias() + ".name", "=", "Type 1");

		std::multiset<std::string> names;

		for (const auto &row: *accounts)
		{
			names.insert(row->name.value());
		}

		if (names.size() != 1 ||
		    *names.begin() != "Account 1")
			throw EXCEPTION("Simple left join failed");
	}

	droptables(conn);

	conn->execute("create table temptbl_accounts(account_id integer not null, account_type_id integer not null, code varchar(255) null, primary key(account_id))");

	conn->execute("create table temptbl_account_types(account_type_id integer not null, primary key(account_type_id))");

	conn->execute("create table temptbl_ledger_entries(ledger_entry_id integer not null, account_id integer not null, ledger_date date not null, amount numeric(11,2) not null, primary key(ledger_entry_id))");

	conn->execute("alter table temptbl_accounts add foreign key(account_type_id) references temptbl_account_types(account_type_id)");

	conn->execute("alter table temptbl_ledger_entries add foreign key(account_id) references temptbl_accounts(account_id)");

	conn->execute("create table temptbl_payments(payment_id integer not null, source_ledger_id integer not null, dest_ledger_id integer not null)");

	conn->execute("alter table temptbl_payments add foreign key(source_ledger_id) references temptbl_ledger_entries(ledger_entry_id)");
	conn->execute("alter table temptbl_payments add foreign key(dest_ledger_id) references temptbl_ledger_entries(ledger_entry_id)");

	conn->execute("insert into temptbl_account_types values(1)");
	conn->execute("insert into temptbl_account_types values(2)");
	conn->execute("insert into temptbl_account_types values(3)");

	conn->execute("insert into temptbl_accounts(account_id, account_type_id, code) values(?, ?, ?)", 1, 1, "Acct1");
	conn->execute("insert into temptbl_accounts(account_id, account_type_id, code) values(?, ?, ?)", 2, 2, "Acct2");
	conn->execute("insert into temptbl_ledger_entries(ledger_entry_id, account_id, ledger_date, amount) values(?, ?, ?, ?)", 1, 1, "2013-08-03", 10);
	conn->execute("insert into temptbl_ledger_entries(ledger_entry_id, account_id, ledger_date, amount) values(?, ?, ?, ?)", 2, 2, "2013-08-03", -10);

	{
		auto ledger_entries=example2::ledger_entries::create(conn);

		example2::accounts::base::joins
			accounts_join=ledger_entries->join_accounts();

		ledger_entries->search("code", "=", std::vector<const char *>({"Acct1", "Acct2"}));

		std::set<double> values;

		for (const auto &row: *ledger_entries)
		{
			auto amount=row->amount.value();

			values.insert(amount);
			std::cout << LIBCXX_NAMESPACE::tostring(row->ledger_date.value())
				  << ": "
				  << amount
				  << std::endl;
		}

		if (values != std::set<double>({10, -10}))
			throw EXCEPTION("Join did not get expected results (1)");
	}

	conn->execute("INSERT INTO temptbl_payments(payment_id,source_ledger_id,dest_ledger_id) values(1,1,2)");

	{
		auto payments=example2::payments::create(conn);

		auto source_account=
			payments->join_source_ledger_id()->join_accounts();

		auto dest_account=
			payments->join_dest_ledger_id()->join_accounts();

		payments->search(source_account->get_table_alias() + ".code", "=", "Acct1",
				 dest_account->get_table_alias() + ".code", "=", "Acct2");

		std::set<int> keys;
		std::set<double> values;

		for (const example2::payments::base::row &row: *payments)
		{
			std::cout << row->payment_id.value()
				  << " "
				  << row->source_ledger_id.value()
				  << " "
				  << row->dest_ledger_id.value()
				  << std::endl;

			keys.insert(row->payment_id.value());

			values.insert(row->join_source_ledger_id()
				      ->amount.value());

		}

		if (keys != std::set<int>({1}))
			throw EXCEPTION("Join did not get expected results (2)");
		if (values != std::set<double>({10}))
			throw EXCEPTION("Join did not get expected results (3)");
	}

	{
		auto payments=example2::payments::create(conn);

		payments->search("payment_id", "<", 0);

		if (!payments->maybe().null())
			throw EXCEPTION("Mysterious maybe() results");
	}

	bool caught=false;

	try {
		auto ledgers=example2::ledger_entries::create(conn);

		ledgers->only();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		caught=true;
		std::cout << "Expected exception: " << e << std::endl;
	}

	if (!caught)
		throw EXCEPTION("only() failed to detect multiple rows");

	{
		example2::accounts accounts=example2::accounts::create(conn);

		for (const example2::accounts::base::row &row: *accounts)
		{
			example2::ledger_entries
				ledger_entries=row->join_ledger_entries();

			for (const example2::ledger_entries::base::row &row: *ledger_entries)
			{
				row;
			}
		}
	}

	caught=false;

	try
	{
		auto ledger_entry_rs=
			example2::ledger_entries::create(conn);
		ledger_entry_rs->search("ledger_entry_id", "=", -11);

		auto ledger_entry=ledger_entry_rs->only();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		caught=true;
		std::cout << "Expected exception: " << e << std::endl;
	}

	if (!caught)
		throw EXCEPTION("only() failed to detect no rows?");

	{
		auto ledger_entry_rs=
			example2::ledger_entries::create(conn);
		ledger_entry_rs->search("ledger_entry_id", "=", 1);

		auto ledger_entry=ledger_entry_rs->only();

		auto account=ledger_entry->join_accounts();

		if (account->account_id.value() != 1)
			throw EXCEPTION("Did not get account 1?");

		account->account_id.value(2);

		if (ledger_entry->join_accounts()->account_id.value() != 2)
			throw EXCEPTION("row join() does not cache");

		if (ledger_entry->join_source_ledger_id().null())
			throw EXCEPTION("maybe() row join was null");
		if (!ledger_entry->join_dest_ledger_id().null())
			throw EXCEPTION("maybe() row join was not null");
	}

	{
		auto payments=example2::payments::create(conn);

		auto source_ledger_id_join=payments->join_source_ledger_id();
		auto dest_ledger_id_join=payments->join_dest_ledger_id();

		example2::ledger_entries::base::prefetchedrow
			prefetched_src_ledger=source_ledger_id_join->prefetch(),
			prefetched_dst_ledger=dest_ledger_id_join->prefetch();

		payments->search("payment_id", "=", 1);

		bool found=false;

		for (const example2::payments::base::row &payment: *payments)
		{
			example2::ledger_entries::base::row
				source_ledger=prefetched_src_ledger->row,
				dest_ledger=prefetched_dst_ledger->row;

			std::cout << "Payment ID: "
				  << payment->payment_id.value()
				  << " "
				  << source_ledger->amount.value()
				  << " "
				  << dest_ledger->amount.value()
				  << std::endl;

			if (payment->payment_id.value() != 1 ||
			    source_ledger->amount.value() != 10 ||
			    dest_ledger->amount.value() != -10)
				throw EXCEPTION("Did not receive expected prefetched data");
			found=true;
		}

		if (!found)
			throw EXCEPTION("Did not receive expected prefetched data");
	}

	{
		auto payments=example2::payments::create(conn);

		auto source_ledger_id_join=payments->join_source_ledger_id();
		auto dest_ledger_id_join=payments->join_dest_ledger_id();

		example2::ledger_entries::base::prefetchedrow
			prefetched_src_ledger=source_ledger_id_join->prefetch(),
			prefetched_dst_ledger=dest_ledger_id_join->prefetch();

		payments->search("payment_id", "=", 1);

		auto payment=payments->only();

		example2::ledger_entries::base::row
			source_ledger=prefetched_src_ledger->row,
			dest_ledger=prefetched_dst_ledger->row;

		std::cout << "Payment ID: "
			  << payment->payment_id.value()
			  << " "
			  << source_ledger->amount.value()
			  << " "
			  << dest_ledger->amount.value()
			  << std::endl;

		if (payment->payment_id.value() != 1 ||
		    source_ledger->amount.value() != 10 ||
		    dest_ledger->amount.value() != -10)
			throw EXCEPTION("Did not receive expected prefetched data from only()");
	}

	{
		auto ledger_entries=example2::ledger_entries::create(conn);

		ledger_entries->order_by("account_id desc", "ledger_entry_id");

		std::vector<int64_t> ids;

		for (const auto &row: *ledger_entries)
		{
			ids.push_back(row->ledger_entry_id.value());
		}

		if (ids != std::vector<int64_t>({2, 1}))
			throw EXCEPTION("order by failed");
	}

	{
		auto all_accounts=example2::accounts::create(conn);

		all_accounts->join_ledger_entries();
		all_accounts->group_by(all_accounts->get_table_alias() + ".account_id");
		all_accounts->having("COUNT(*)", ">", 0);
		all_accounts->order_by(all_accounts->get_table_alias() + ".account_id");
		std::vector<int64_t> ids;

		for (const auto &row: *all_accounts)
		{
			ids.push_back(row->account_id.value());
		}

		if (ids != std::vector<int64_t>({1, 2}))
			throw EXCEPTION("group by/having/order by failed");

		all_accounts->get_select_sql(std::cout);
		std::cout << std::endl << std::flush;
	}

	{
		auto all_accounts=example2::accounts::create(conn);

		all_accounts->search("account_id", "=", 1);

		if (all_accounts->update("account_type_id", 2,
					 all_accounts->get_table_alias() + ".code",
					 nullptr)
		    != 1)
			throw EXCEPTION("Unexpected number of rows returned from update (1)");

		auto row=all_accounts->only();

		if (row->account_type_id.value() != 2 ||
		    !row->code.isnull())
			throw EXCEPTION("update failed (1)");
	}

	{
		auto all_accounts=example2::accounts::create(conn);

		auto join=all_accounts->join_ledger_entries();

		all_accounts->search(join->get_table_alias() + ".account_id", "=", 1);

		if (all_accounts->update("account_type_id", "1",
					 all_accounts->get_table_alias() + ".code",
					 "Acct1")
		    != 1)
			throw EXCEPTION("Unexpected number of rows returned from update (2)");

		auto row=all_accounts->only();

		if (row->account_type_id.value() != 1 ||
		    row->code.value() != "Acct1")
			throw EXCEPTION("update failed (2)");
	}

	{
		auto all_accounts=example2::accounts::create(conn);
		all_accounts->search("account_id", "=", 1);

		auto a=all_accounts->only();
		auto b=all_accounts->only();

		if (a->update())
			throw EXCEPTION("No updates returned true");

		a->account_type_id.value(3);
		if (!a->update())
			throw EXCEPTION("An update returned false");

		b->code.value("Acct6");
		b->update();

		if (b->code.value() != "Acct6" ||
		    b->account_type_id.value() != 3)
			throw EXCEPTION("Update did not refresh the row");
	}
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

