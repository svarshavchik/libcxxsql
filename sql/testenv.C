/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/sql/env.H"
#include "x/sql/connection.H"
#include <x/options.H>
#include <x/join.H>
#include <x/ymd.H>
#include <x/hms.H>
#include "x/sql/statement.H"
#include "x/sql/exception.H"
#include <iostream>
#include <iomanip>
#include <algorithm>

void testenv()
{
	std::map<std::string, std::string> sources;

	LIBCXX_NAMESPACE::sql::env::create()->get_data_sources(sources);

	for (const auto &src:sources)
	{
		std::cout << std::setw(30)
			  << std::setiosflags(std::ios::left)
			  << src.first
			  << std::setw(0)
			  << " "
			  << src.second << std::endl;
	}

	sources.clear();

	LIBCXX_NAMESPACE::sql::env::create()->get_drivers(sources);

	for (const auto &src:sources)
	{
		std::cout << std::setw(30)
			  << std::setiosflags(std::ios::left)
			  << src.first
			  << std::setw(0)
			  << " "
			  << src.second << std::endl;
	}

}

static void dump_params(const LIBCXX_NAMESPACE::sql::statement &h)
{
	std::vector<LIBCXX_NAMESPACE::sql::statement::base::parameter>
		params=h->get_parameters();

	for (const auto &param:params)
	{
		std::cout << param.data_type << "(" << param.parameter_size;

		if (param.decimal_digits)
			std::cout << "," << param.decimal_digits;
		std::cout << ")";

		if (param.nullable)
			std::cout << " nullable";
		std::cout << std::endl;
	}
}

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

static void dump(const LIBCXX_NAMESPACE::sql::statement &stmt)
{
	const auto &s=stmt->get_columns();

	std::map<std::string, std::string> columns;
	const char *sep="";

	for (const auto &col:s)
	{
		columns[col.name];

		std::cout << sep << col.name;
		sep="\t";
	}
	std::cout << std::endl;

	while (stmt->fetch(columns))
	{
		sep="";

		for (const auto &col:s)
		{
			std::cout << sep << columns[col.name];
			sep="\t";
		}
		std::cout << std::endl;
	}
}

void testconnect(const std::string &connection,
		 int flags)
{
	auto conn=({
			auto ret=LIBCXX_NAMESPACE::sql::env::create()
				->connect(connection,
					  (LIBCXX_NAMESPACE::sql::connect_flags)
					  flags);

			std::cout << "Connected to " << ret.second << std::endl;

			ret.first;
		});

	std::cout << "Batch support: "
		  << LIBCXX_NAMESPACE::join(conn->config_get_batch_support(),
					    ", ") << std::endl;
	std::cout << "Accessible tables: "
		  << conn->config_get_accessible_tables() << std::endl;

	std::cout << "Database name: "
		  << conn->config_get_database_name()
		  << std::endl;

	std::cout << "DBMS: " << conn->config_get_dbms_name() << std::endl;
	std::cout << "VER: " << conn->config_get_dbms_ver() << std::endl;
	std::cout << "Scroll: "
		  << LIBCXX_NAMESPACE::join(conn->config_get_scroll_options(),
					    ", ") << std::endl;

	std::cout << "Columns: " << std::endl;
	dump(conn->columns());

	try {
		std::cout << "Column Privileges: " << std::endl;
		dump(conn->column_privileges("", "", "temptbl1"));
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		std::cout << e << std::endl;
	}
	std::cout << "Foreign keys: " << std::endl;
	dump(conn->foreign_keys("", "", "tmptbl1"));
	dump(conn->type_info());
	dump(conn->type_info("varchar"));
	std::cout << "Procedures:" << std::endl;
	dump(conn->procedures("", "", "temp%"));

	std::cout << "Procedure Columns:" << std::endl;
	dump(conn->procedure_columns("", "", "temp%", "%"));

	{
		auto tables=conn->tables("", "", "tmptbl%");

		const auto &columns=tables->get_columnmap();

		std::map<std::string, std::pair<std::string,
						LIBCXX_NAMESPACE::sql::bitflag>
			 > table;

		for (const auto &col:columns)
		{
			table.insert(std::make_pair(col.first,
						    std::make_pair("", false)));
		}

		while (tables->fetch(table))
		{
			std::vector<std::string> values;

			std::transform(std::begin(table),
				       std::end(table),
				       std::back_insert_iterator<
				       std::vector<std::string> >(values),
				       []
				       (decltype(*std::begin(table)) &value)
				       {
					       return value.first + "="
						       + (value.second.second
							  ? "(null)"
							  : value.second.first);
				       });

			std::cout << LIBCXX_NAMESPACE::join(values, ", ")
				  << std::endl;
		}
	}

	{
		auto tables=get_tables(conn, false, "tmptbl%");

		for (const auto &table_name:tables)
		{
			std::cout << "Table: " << table_name << std::endl;
			conn->prepare("drop table " + table_name)->execute();
		}
	}

	{
		auto tables=conn->tables("", "", "tmptbl%");

		std::map<std::string, std::pair<std::string,
						LIBCXX_NAMESPACE::sql::bitflag
						>> columns;

		columns["table_name"];

		while (tables->fetch(columns))
		{
		}
	}

	conn->prepare("create table tmptbl1(int_col int not null, varchar_col varchar(255) null);")->execute();
	conn->prepare("create table tmptbl2(bigint_col bigint not null, char_col char(4) null, text_col text, child_col bigint null, primary key(bigint_col), foreign key(child_col) references tmptbl2(bigint_col));")->execute();
	std::cout << "tmptbl2 primary keys:" << std::endl;
	dump(conn->primary_keys("tmptbl2"));
	std::cout << "tmptbl2 foreign keys:" << std::endl;
	dump(conn->foreign_keys("", "", "tmptbl2"));
	dump(conn->foreign_keys("", "", "", "", "", "tmptbl2"));

	if (get_tables(conn, false, "tmptbl%").count("tmptbl1") != 1)
		throw EXCEPTION("get_tables test 1 failed");

	if (get_tables(conn, false, "\"tmptbl1\"").count("tmptbl1") != 0)
		throw EXCEPTION("get_tables test 2 failed");

	dump(conn->special_columns(LIBCXX_NAMESPACE::sql::rowid_t::unique,
				   LIBCXX_NAMESPACE::sql::scope_t::session,
				   "tmptbl2"));

	dump(conn->special_columns(LIBCXX_NAMESPACE::sql::rowid_t::version,
				   LIBCXX_NAMESPACE::sql::scope_t::session,
				   "tmptbl2"));
	dump(conn->statistics("tmptbl2"));
	std::cout << "Table privileges:" << std::endl;
	dump(conn->table_privileges("tmptbl2"));

#if 0
	if (get_tables(conn, true, "tmptbl%").count("tmptbl1") != 0)
		throw EXCEPTION("get_tables test 3 failed");

	if (get_tables(conn, true, "\"tmptbl1\"").count("tmptbl1") != 1)
		throw EXCEPTION("get_tables test 4 failed");
#endif
	{
		auto h=conn->prepare("INSERT INTO tmptbl1 values(?, ?)");

		if (h->num_params() != 2)
			throw EXCEPTION("Did not get the expected two parameters");
		try {
			dump_params(h);
		} catch (const LIBCXX_NAMESPACE::exception &e)
		{
			std::cout << e << std::endl;
		}

		h->execute( ((unsigned short)-1), "a");
		h->execute( "1", nullptr);
		h->execute( 3, std::string(100, 'x'));

		{
			char charbuf[10];

			strcpy(charbuf, "zzz");

			h->execute(6, charbuf);
		}
	}

	LIBCXX_NAMESPACE::sql::bitflag ignore_bitflag;

	conn->execute(ignore_bitflag, "INSERT INTO tmptbl1 VALUES (?, ?)", "5", "yyy");

	{
		std::list<std::string> params;

		params.push_back("2");
		params.push_back("xxxx");
		conn->execute("INSERT INTO tmptbl1 VALUES(?, ?)", params);
	}

	{
		std::list<std::pair<std::string,
				    LIBCXX_NAMESPACE::sql::bitflag>> params;

		params.push_back({"4", false});
		params.push_back({"", true});
		conn->execute("INSERT INTO tmptbl1 VALUES(?, ?)", params);
	}

	std::string expected="1=null,2=xxxx,3=xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx,4=null,5=yyy,6=zzz,65535=a";

	{
		auto stmt=conn->execute("SELECT * FROM tmptbl1 ORDER BY int_col");

		std::pair<std::vector<int>,
			  std::vector<LIBCXX_NAMESPACE::sql::bitflag>> n;

		std::pair<std::vector<std::string>,
			  std::vector<LIBCXX_NAMESPACE::sql::bitflag>> str;

		std::ostringstream o;
		const char *sep="";

		size_t c;

		while ((c=stmt->fetch_vectors(2, "int_col", n,
					      "varchar_col", str))
		       > 0)
		{
			for (size_t i=0; i<c; ++i)
			{
				o << sep;
				sep=",";

				if (n.second[i])
					o << "null";
				else
					o << n.first[i];

				o << "=" << (str.second[i] ? "null":str.first[i]);
			}
		}

		std::string got=o.str();

		if (got != expected)
			throw EXCEPTION("GOT THIS INSTEAD: " + got);
	}

	{
		auto stmt=conn->execute("SELECT * FROM tmptbl1 ORDER BY int_col");

		std::vector<int> n;
		std::vector<std::string> str;

		std::ostringstream o;
		const char *sep="";

		size_t c;

		while ((c=stmt->fetch_vectors(2, "int_col", n,
					      "varchar_col", str))
		       > 0)
		{
			for (size_t i=0; i<c; ++i)
			{
				o << sep;
				sep=",";

				o << n[i] << "=" << str[i];
			}
		}

		std::string expected="1=,2=xxxx,3=xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx,4=,5=yyy,6=zzz,65535=a";

		std::string got=o.str();

		if (got != expected)
			throw EXCEPTION("GOT THIS INSTEAD: " + got + " (2)");
	}


	{
		auto stmt=conn->execute("SELECT * FROM tmptbl1 ORDER BY int_col");
		std::map<std::string, std::pair<std::vector<std::string>,
						std::vector<LIBCXX_NAMESPACE::sql::bitflag>>> columns;

		auto &int_col=columns["int_col"];
		auto &varchar_col=columns["varchar_col"];

		std::ostringstream o;
		const char *sep="";

		stmt->fetch_vectors_all
			(2, [&int_col, &varchar_col, &o, &sep]
			 (size_t c)
			 {
				 auto int_col_v_iter=int_col.first.begin();
				 auto int_col_n_iter=int_col.second.begin();

				 auto varchar_col_v_iter=
					 varchar_col.first.begin();
				 auto varchar_col_n_iter=
					 varchar_col.second.begin();

				 for (size_t i=0; i<c; ++i,
					      ++int_col_v_iter,
					      ++int_col_n_iter,
					      ++varchar_col_v_iter,
					      ++varchar_col_n_iter)
				 {
					 o << sep;
					 sep=",";

					 if (*int_col_n_iter)
						 o << "null";
					 else
						 o << *int_col_v_iter;

					 o << "=" << (*varchar_col_n_iter
						      ? "null"
						      : *varchar_col_v_iter);
				 }
			 }, columns);

		std::string got=o.str();

		if (got != expected)
			throw EXCEPTION("Got " + got + " instead of " + expected);
	}

	{
		std::vector<int> one={1, 2};
		std::vector<std::string> two={"1", "2"};
		std::vector<LIBCXX_NAMESPACE::sql::bitflag> status;
		status.resize(2);

		conn->execute_vector("insert into tmptbl2(bigint_col, char_col) values(?, ?)",
				     status, one, two);
		if (!(status[0] & 1) ||
		    !(status[1] & 1))
			throw EXCEPTION("status fail check 1");

		one={3, 4};
		std::pair<std::vector<std::string>,
			  std::vector<LIBCXX_NAMESPACE::sql::bitflag>>
			two_null={ {"", ""}, {1, 1}};

		conn->execute_vector("insert into tmptbl2(bigint_col, char_col) values(?, ?)",
				     status, one, two_null);

		if (!(status[0] & 1) ||
		    !(status[1] & 1))
			throw EXCEPTION("status fail check 2");
	}

	{
		std::list<std::vector<std::string>> params;

		params.push_back({"5", "6"});
		params.push_back({"5", "6"});

		std::vector<LIBCXX_NAMESPACE::sql::bitflag> status;
		status.resize(2);

		conn->execute_vector("insert into tmptbl2(bigint_col, char_col) values(?, ?)",
				     status, params);
		if (!(status[0] & 1) ||
		    !(status[1] & 1))
			throw EXCEPTION("status fail check 3");
	}

	{
		std::list<std::pair<std::vector<std::string>,
				    std::vector<LIBCXX_NAMESPACE::sql::bitflag>>> params;

		params.emplace_back(std::vector<std::string>({"7", "8"}), std::vector<LIBCXX_NAMESPACE::sql::bitflag>({0, 0}));

		params.emplace_back(std::vector<std::string>({"", ""}), std::vector<LIBCXX_NAMESPACE::sql::bitflag>({1, 1}));

		std::vector<LIBCXX_NAMESPACE::sql::bitflag> status;
		status.resize(2);

		conn->execute_vector("insert into tmptbl2(bigint_col, char_col) values(?, ?)",
				     status, params);
		if (!(status[0] & 1) ||
		    !(status[1] & 1))
			throw EXCEPTION("status fail check 4");
	}

	{
		std::vector<int> one={9, 7};
		std::vector<std::string> two={"9", "7"};
		std::vector<LIBCXX_NAMESPACE::sql::bitflag> status;
		status.resize(2);

		bool caught=false;
		try {
			conn->execute_vector("insert into tmptbl2(bigint_col, char_col) values(?, ?)",
					     status, one, two);
		} catch (const LIBCXX_NAMESPACE::sql::exception &e)
		{
			caught=true;
			std::cout << "Exception caught: "
				  << (LIBCXX_NAMESPACE::exception)e
				  << std::endl;
		}

		if (!caught)
			throw EXCEPTION("Duplicate insert did not throw an exception");

		if (!(status[0] & 1) ||
		    (status[1] & 1))
			throw EXCEPTION("status fail check 5");

	}

	{
		auto stmt=conn->execute("SELECT * FROM tmptbl2 ORDER BY bigint_col");

		std::vector<long long> bigint;

		std::pair<std::vector<std::string>,
			  std::vector<LIBCXX_NAMESPACE::sql::bitflag>>
			charcol, textcol;

		std::ostringstream o;

		stmt->fetch_vectors_all(100,
					[&]
					(size_t c)
					{
						for (size_t i=0; i<c; ++i)
							o << bigint[i]
							  << ": "
							  << (charcol.second[i]
							      ? "null":
							      (charcol.first[i] + "    ").substr(0, 4))
							  << ", "
							  << (textcol.second[i]
							      ? "null":
							      textcol.first[i])
							  << "|";
					},
					"bigint_col", bigint,
					"char_col", charcol,
					"text_col", textcol);


		std::string got=o.str();

		if (got !=
		    "1: 1   , null|"
		    "2: 2   , null|"
		    "3: null, null|"
		    "4: null, null|"
		    "5: 5   , null|"
		    "6: 6   , null|"
		    "7: null, null|"
		    "8: null, null|"
		    "9: 9   , null|")
			throw EXCEPTION("Got \"" + got + "\" instead of what was expected");
	}

	{
		std::vector<int> one={1, 2};
		std::vector<std::string> two={"1", "2"};
		std::vector<LIBCXX_NAMESPACE::sql::bitflag> status;
		status.resize(2);

		bool caught=false;

		std::fill(status.begin(), status.end(), 1);

		try {
			conn->execute_vector("insert into tmptbl2(nonexistent_col, char_col) values(?, ?)",
					     status, one, two);
		} catch (const LIBCXX_NAMESPACE::exception &e)
		{
			std::cerr << "Expected exception: " << e << std::endl;
			caught=true;
		}
		if (!caught)
			throw EXCEPTION("Did not catch expected exception from execute_vector");

		if ((status[0] & 1) ||
		    (status[1] & 1))
			throw EXCEPTION("status fail check 6");
	}

	conn->execute("CREATE TABLE tmptbl4(d date, t time)");

	std::vector<LIBCXX_NAMESPACE::ymd> dates={ {2013,1,1}, {2013,4,1} };
	std::vector<LIBCXX_NAMESPACE::hms> times={ {9,0,0}, {15,0,0}};

	std::vector<LIBCXX_NAMESPACE::sql::bitflag> insert_status;
	insert_status.resize(2);
	conn->execute_vector("INSERT INTO tmptbl4(d, t) VALUES(?, ?)",
			     insert_status, dates, times);

	LIBCXX_NAMESPACE::ymd ymd_select;
	LIBCXX_NAMESPACE::hms hms_select;

	{
		auto stmt=conn->execute("SELECT d, t from tmptbl4 ORDER BY d");

		if (!stmt->fetch(0, ymd_select, 1, hms_select) ||
		    ymd_select != LIBCXX_NAMESPACE::ymd(2013, 1, 1) ||
		    hms_select != LIBCXX_NAMESPACE::hms(9, 0, 0) ||
		    !stmt->fetch(0, ymd_select, 1, hms_select) ||
		    ymd_select != LIBCXX_NAMESPACE::ymd(2013, 4, 1) ||
		    hms_select != LIBCXX_NAMESPACE::hms(15, 0, 0))
			throw EXCEPTION("date/time values do not compare");
	}
}

void dummy_connection_by_params()
{
	LIBCXX_NAMESPACE::sql::env::base::arglist_t args;

	args.push_back({"DSN", "dev"});

	std::map<std::string, std::pair<std::string, bool> >  params;

	params.insert({"book_name", {"", false}});
}

int main(int argc, char **argv)
{
	LIBCXX_NAMESPACE::option::list
		options(LIBCXX_NAMESPACE::option::list::create());

	LIBCXX_NAMESPACE::option::int_value flags_value(LIBCXX_NAMESPACE::option::int_value::create((int)LIBCXX_NAMESPACE::sql::connect_flags::noprompt));

	LIBCXX_NAMESPACE::option::string_value connect_value(LIBCXX_NAMESPACE::option::string_value::create());

	LIBCXX_NAMESPACE::option::string_value execute_value(LIBCXX_NAMESPACE::option::string_value::create());

	options->add(connect_value, 'c', L"connect",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     L"Make a test connection",
		     L"data_source")
		.add(execute_value, 'e', L"execute",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     L"Execute SQL statement",
		     L"SQL")
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
			testconnect(connect_value->value,
				    flags_value->value);
		}
		else
		{
			testenv();
		}
	} catch (const LIBCXX_NAMESPACE::exception &e) {
		std::cout << e << std::endl;
		exit(1);
	}
	return 0;
}

