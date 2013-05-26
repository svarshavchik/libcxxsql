/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/sql/env.H"
#include "x/sql/connection.H"
#include "x/sql/statement.H"
#include "x/sql/exception.H"
#include "x/sql/insertblob.H"
#include "x/sql/fetchblob.H"
#include "x/options.H"
#include "x/join.H"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <random>

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
			   const LIBCXX_NAMESPACE::sql::config_bitmask_t &a2,
			   const char *cursortype)
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

	if (a1.count("SQL_CA1_POSITIONED_UPDATE"))
	{
		std::vector<int> intkey;
		std::vector<std::string> strval;

		auto stmt2=conn2->create_newstatement("CURSOR_TYPE",
						      cursortype)->
			execute("SELECT intkey, strval FROM temptbl"
				" ORDER BY intkey FOR UPDATE");

		if (stmt2->fetch_vectors(2, 0, intkey, 1, strval) != 2 ||
		    stmt2->fetch_vectors(1, 0, intkey, 1, strval) != 1 ||
		    intkey[0] != 2)
		{
			throw EXCEPTION("POSITIONED_UPDATE fetch failed");
		}

		stmt2->modify_fetched_row(0, "UPDATE temptbl SET strval=?",
					  "2modified");
		std::vector<std::string> newval;

		newval.resize(conn2->execute("SELECT strval FROM temptbl ORDER BY intkey")->fetch_vectors(20, "strval", newval));

		auto result=LIBCXX_NAMESPACE::join(newval, ", ");

		if (result != "0, 1, 2modified, 3, 4, 5, 6, 7, 8, 9")
			throw EXCEPTION("Unexpected positioned update result: "
					+ result);

		std::cout << "POSITIONED_UPDATE ok!" << std::endl;
	}

	if (a1.count("SQL_CA1_POSITIONED_DELETE"))
	{
		std::vector<int> intkey;
		std::vector<std::string> strval;

		auto stmt2=conn2->create_newstatement("CURSOR_TYPE",
						      cursortype)->
			execute("SELECT intkey, strval FROM temptbl"
				" ORDER BY intkey FOR UPDATE");

		if (stmt2->fetch_vectors(2, 0, intkey, 1, strval) != 2 ||
		    stmt2->fetch_vectors(1, 0, intkey, 1, strval) != 1)
		{
			throw EXCEPTION("POSITIONED_DELETE fetch failed");
		}

		stmt2->modify_fetched_row(0, "DELETE from temptbl");
		std::vector<std::string> newval;

		newval.resize(conn2->execute("SELECT strval FROM temptbl ORDER BY intkey")->fetch_vectors(20, "strval", newval));

		auto result=LIBCXX_NAMESPACE::join(newval, ", ");

		if (result != "0, 1, 3, 4, 5, 6, 7, 8, 9")
			throw EXCEPTION("Unexpected positioned update result: "
					+ result);

		std::cout << "POSITIONED_DELETE ok!" << std::endl;
		conn2->execute("INSERT INTO temptbl(intkey, strval) values(2, '2')");
	}
}

static void forward_cursor(const LIBCXX_NAMESPACE::sql::connection &conn1,
			   const LIBCXX_NAMESPACE::sql::connection &conn2,
			   const LIBCXX_NAMESPACE::sql::statement &stmt,
			   const LIBCXX_NAMESPACE::sql::config_bitmask_t &a1,
			   const LIBCXX_NAMESPACE::sql::config_bitmask_t &a2)
{
	testcursortype(conn1, conn2, stmt, a1, a2, "FORWARD");
}

static void static_cursor(const LIBCXX_NAMESPACE::sql::connection &conn1,
			  const LIBCXX_NAMESPACE::sql::connection &conn2,
			  const LIBCXX_NAMESPACE::sql::statement &stmt,
			  const LIBCXX_NAMESPACE::sql::config_bitmask_t &a1,
			  const LIBCXX_NAMESPACE::sql::config_bitmask_t &a2)
{
	testcursortype(conn1, conn2, stmt, a1, a2, "STATIC");
}

static void keyset_cursor(const LIBCXX_NAMESPACE::sql::connection &conn1,
			  const LIBCXX_NAMESPACE::sql::connection &conn2,
			  const LIBCXX_NAMESPACE::sql::statement &stmt,
			  const LIBCXX_NAMESPACE::sql::config_bitmask_t &a1,
			  const LIBCXX_NAMESPACE::sql::config_bitmask_t &a2)
{
	testcursortype(conn1, conn2, stmt, a1, a2, "KEYSET(100)");
}

static void dynamic_cursor(const LIBCXX_NAMESPACE::sql::connection &conn1,
			   const LIBCXX_NAMESPACE::sql::connection &conn2,
			   const LIBCXX_NAMESPACE::sql::statement &stmt,
			   const LIBCXX_NAMESPACE::sql::config_bitmask_t &a1,
			   const LIBCXX_NAMESPACE::sql::config_bitmask_t &a2)
{
	testcursortype(conn1, conn2, stmt, a1, a2, "DYNAMIC");
}

class randblob {

public:
	std::mt19937 &re;

	std::uniform_int_distribution<size_t> randlen;
	std::uniform_int_distribution<char> characters;

	randblob(std::mt19937 &reArg)
		: re(reArg),
		  randlen(4000, 48000),
		  characters('A', 'Z')
	{
	}

	template<typename vec_type>
	void fill(vec_type &vec)
	{
		randblob *p=this;

		std::generate_n(std::back_insert_iterator<vec_type>(vec),
				randlen(re),
				[p, this]
				{
					return p->characters(re);
				});
	}
};

template<typename char_type>
void testblobs(const LIBCXX_NAMESPACE::sql::connection &conn,
	       std::mt19937 &re,
	       bool skip_fetch_vector_blobs)
{
	typedef std::vector<char_type> vec_t;

	vec_t largeblobs[4];

	{
		randblob r(re);

		r.fill(largeblobs[0]);
		r.fill(largeblobs[1]);
		r.fill(largeblobs[2]);
		r.fill(largeblobs[3]);
	}

	conn->execute("DELETE FROM temptbl2");

	for (size_t i=0; i<4; ++i)
	{
		vec_t &largeblob=largeblobs[i];

		conn->execute("INSERT INTO temptbl2(intkey, strval) VALUES(?, ?)",
			      i,
			      LIBCXX_NAMESPACE::sql::insertblob::create(largeblob.begin(),
									largeblob.end()));
	}

	int close_called=0;

	{
		auto stmt=conn->execute("SELECT intkey, strval FROM temptbl2 ORDER BY intkey");

		int intkey;
		vec_t strval;

		if (!stmt->fetch("intkey", intkey,
				 "strval",
				 LIBCXX_NAMESPACE::sql::fetchblob<char_type>
				 ::create([&strval]
					  (size_t rownum)
					  {
						  strval.clear();
						  return std::back_insert_iterator<vec_t>
							  (strval);
					  })))
			throw EXCEPTION("Unexpected EOF when fetching 1st blob");
		if (intkey != 0 || strval != largeblobs[0])
			throw EXCEPTION("Unexpected result of fetching 1st blob");

		std::pair<LIBCXX_NAMESPACE::sql::fetchblob<char_type>,
			  LIBCXX_NAMESPACE::sql::bitflag> strval_null
			(LIBCXX_NAMESPACE::sql::fetchblob<char_type>::create
			 ([&strval, &close_called]
			  (size_t rownum)
			  {
				  strval.clear();
				  return std::make_pair(std::back_insert_iterator<vec_t>
							(strval),
							[&close_called]
							(std::back_insert_iterator<vec_t> dummy)
							{
								++close_called;
							});
			  }), 0);

		if (!stmt->fetch(0, intkey,
				 1, strval_null))
			throw EXCEPTION("Unexpected EOF when fetching 2nd blob");
		if (intkey != 1 || strval != largeblobs[1])
			throw EXCEPTION("Unexpected result of fetching 2nd blob");
		if (close_called != 1)
			throw EXCEPTION("Close callback was not invoked for 2nd blob");

		std::map<std::string, LIBCXX_NAMESPACE::sql::fetchblob<char_type>>
			strval_map;

		strval_map.insert(std::make_pair("strval",
						 strval_null.first));

		if (!stmt->fetch(0, intkey,
				 strval_map))
			throw EXCEPTION("Unexpected EOF when fetching 3rd blob");

		if (intkey != 2 || strval != largeblobs[2])
			throw EXCEPTION("Unexpected result of fetching 3rd blob");
		if (close_called != 2)
			throw EXCEPTION("Close callback was not invoked for 3rd blob");

		std::map<std::string, std::pair<LIBCXX_NAMESPACE::sql::fetchblob<char_type>,
						LIBCXX_NAMESPACE::sql::bitflag>
			 > strval_map_null;

		strval_map_null.insert(std::make_pair("strval",
						      std::make_pair(strval_null
								     .first,
								     0)));
		if (!stmt->fetch(0, intkey,
				 strval_map_null))
			throw EXCEPTION("Unexpected EOF when fetching 4th blob");
		if (intkey != 3 || strval != largeblobs[3])
			throw EXCEPTION("Unexpected result of fetching 4th blob");
		if (close_called != 3)
			throw EXCEPTION("Close callback was not invoked for 4th blob");
	}

	if (skip_fetch_vector_blobs)
		return;

	std::vector<vec_t> fetchblobs;
	std::set<size_t> fetch_close_called;

	auto stmt=conn->execute("SELECT intkey, strval FROM temptbl2 ORDER BY intkey");

	fetchblobs.resize(4);
	stmt->fetch_vectors
		(4, 1, 
		 LIBCXX_NAMESPACE::sql::fetchblob<char_type>
		 ::create([&fetchblobs,
			   &fetch_close_called]
			  (size_t rownum)
			  {
				  fetchblobs[rownum].clear();
				  return std::make_pair
					  (std::back_insert_iterator<vec_t>
					   (fetchblobs[rownum]),
					   [&fetch_close_called, rownum]
					   (std::back_insert_iterator<vec_t> dummy)
					   {
						   fetch_close_called
							   .insert(rownum);
					   });
			  }));

	if (fetchblobs[0] != largeblobs[0] ||
	    fetchblobs[1] != largeblobs[1] ||
	    fetchblobs[2] != largeblobs[2] ||
	    fetchblobs[3] != largeblobs[3])
		throw EXCEPTION("Vector blob fetch failed");

	if (fetch_close_called != std::set<size_t>({0, 1, 2, 3}))
		throw EXCEPTION("Vector blob close called flag check failed");

	std::cout << "Tested vector blob fetches" << std::endl;

	std::vector<LIBCXX_NAMESPACE::sql::insertblob> vector_insert;

	for (size_t i=0; i<4; ++i)
		vector_insert.push_back(LIBCXX_NAMESPACE::sql
					::insertblob
					::create(largeblobs[i].begin(),
						 largeblobs[i].end()));
	std::vector<LIBCXX_NAMESPACE::sql::bitflag> flags;
	std::vector<int> intkeys={10, 11, 12, 13};
	flags.resize(4);

	conn->execute_vector("INSERT INTO temptbl2(intkey, strval) VALUES(?, ?)",
			     flags, intkeys, vector_insert);

	stmt=conn->execute("SELECT intkey, strval FROM temptbl2 WHERE intkey >= 10 and intkey <= 13 ORDER BY intkey");

	LIBCXX_NAMESPACE::sql::fetchblob<char_type> fetchblob=
		LIBCXX_NAMESPACE::sql::fetchblob<char_type>
		::create([&fetchblobs]
			 (size_t rownum)
			 {
				 fetchblobs[rownum].clear();
				 return std::back_insert_iterator<vec_t>
				 (fetchblobs[rownum]);
			 });
	fetchblobs.clear();
	fetchblobs.resize(4);
	stmt->fetch_vectors(4, 1, fetchblob);

	if (fetchblobs[0] != largeblobs[0] ||
	    fetchblobs[1] != largeblobs[1] ||
	    fetchblobs[2] != largeblobs[2] ||
	    fetchblobs[3] != largeblobs[3])
		throw EXCEPTION("Vector blob fetch #2 failed");
	std::cout << "Tested vector blob fetches #2" << std::endl;

	std::pair<std::vector<LIBCXX_NAMESPACE::sql::insertblob>,
		  std::vector<LIBCXX_NAMESPACE::sql::bitflag>>
		vector_null_insert;

	for (size_t i=0; i<4; ++i)
		vector_null_insert.first
			.push_back(LIBCXX_NAMESPACE::sql::insertblob
				   ::create(largeblobs[i].begin(),
					    largeblobs[i].end()));

	vector_null_insert.second={0, 1, 0, 0};
	intkeys={20, 21, 22, 23};
	conn->execute_vector("INSERT INTO temptbl2(intkey, strval) VALUES(?, ?)",
			     flags, intkeys, vector_null_insert);

	fetchblobs.clear();
	fetchblobs.resize(4);
	std::pair<LIBCXX_NAMESPACE::sql::fetchblob<char_type>,
		  std::vector<LIBCXX_NAMESPACE::sql::bitflag> >
		fetchblobs_null_ind=
		std::make_pair(LIBCXX_NAMESPACE::sql::fetchblob<char_type>
			       ::create([&fetchblobs]
					(size_t rownum)
		{
			return std::back_insert_iterator<vec_t>(fetchblobs[rownum]);
		}), std::vector<LIBCXX_NAMESPACE::sql::bitflag>());

	stmt=conn->execute("SELECT strval FROM temptbl2 WHERE intkey >= 20 and intkey <= 23 ORDER BY intkey");

	stmt->fetch_vectors(4, "strval", fetchblobs_null_ind);

	if (fetchblobs_null_ind.second !=
	    std::vector<LIBCXX_NAMESPACE::sql::bitflag>({0, 1, 0, 0}))
		throw EXCEPTION("Vector blob fetch #3 null flag fail");

	if (fetchblobs[0] != largeblobs[0] ||
	    fetchblobs[2] != largeblobs[2] ||
	    fetchblobs[3] != largeblobs[3])
		throw EXCEPTION("Vector blob fetch #3 failed");

	std::cout << "Tested vector blob fetches #3" << std::endl;
	std::list<decltype(vector_null_insert)> vector_null_insert_list;

	{
		vector_null_insert_list
			.push_front(decltype(vector_null_insert)());

		auto &vec=vector_null_insert_list.front();

		for (size_t i=0; i<4; ++i)
			vec.first.push_back(LIBCXX_NAMESPACE::sql::insertblob
					    ::create(largeblobs[i].begin(),
						     largeblobs[i].end()));

		vec.second={0, 1, 0, 0};
		intkeys={30, 31, 32, 33};
		conn->execute_vector("INSERT INTO temptbl2(intkey, strval) VALUES(?, ?)",
				     flags, intkeys, vector_null_insert_list);
	}

	fetchblobs.clear();
	fetchblobs.resize(4);

	std::map<std::string, decltype(fetchblobs_null_ind)>
		fetchblobs_null_ind_map;

	fetchblobs_null_ind_map.insert(std::make_pair("strval",
						      fetchblobs_null_ind));

	stmt=conn->execute("SELECT strval FROM temptbl2 WHERE intkey >= 30 and intkey <= 33 ORDER BY intkey");

	stmt->fetch_vectors(4, fetchblobs_null_ind_map);

	if (fetchblobs_null_ind_map.begin()->second.second !=
	    std::vector<LIBCXX_NAMESPACE::sql::bitflag>({0, 1, 0, 0}))
		throw EXCEPTION("Vector blob fetch #4 null flag fail");

	if (fetchblobs[0] != largeblobs[0] ||
	    fetchblobs[2] != largeblobs[2] ||
	    fetchblobs[3] != largeblobs[3])
		throw EXCEPTION("Vector blob fetch #4 failed");
	std::cout << "Tested vector blob fetches #4" << std::endl;
}

void testcursor(const std::string &connection,
		int flags,
		std::mt19937 &re,
		bool skip_fetch_vector_blobs)
{
	auto env=LIBCXX_NAMESPACE::sql::env::create();
	env->set_login_timeout(10);
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

	if (supported_cursor_types.count("SQL_SO_FORWARD_ONLY"))
	{
		std::cout << "Forward cursor" << std::endl;

		forward_cursor(conn, conn2,
			       conn->create_newstatement("CURSOR_TYPE",
							 "FORWARD",
							 opts)
			       ->execute("SELECT intkey, strval FROM temptbl"),
			       conn->config_get_forward_only_cursor_attributes1(),
			       conn->config_get_forward_only_cursor_attributes2());
	}

	if (supported_cursor_types.count("SQL_SO_STATIC"))
	{
		std::cout << "Static cursor" << std::endl;

		static_cursor(conn, conn2,
			      conn->create_newstatement("CURSOR_TYPE",
							"STATIC",
							opts)
			      ->execute("SELECT intkey, strval FROM temptbl"),
			      conn->config_get_static_cursor_attributes1(),
			      conn->config_get_static_cursor_attributes2());
	}

	if (supported_cursor_types.count("SQL_SO_KEYSET_DRIVEN"))
	{
		std::cout << "Keyset cursor" << std::endl;

		keyset_cursor(conn, conn2,
			      conn->create_newstatement("CURSOR_TYPE",
							"KEYSET(100)",
							opts)
			      ->execute("SELECT intkey, strval FROM temptbl"),
			      conn->config_get_keyset_cursor_attributes1(),
			      conn->config_get_keyset_cursor_attributes2());
	}

	if (supported_cursor_types.count("SQL_SO_DYNAMIC"))
	{
		std::cout << "Dynamic cursor" << std::endl;

		dynamic_cursor(conn, conn2,
			       conn->create_newstatement("CURSOR_TYPE",
							 "DYNAMIC",
							 opts)
			       ->execute("SELECT intkey, strval FROM temptbl"),
			       conn->config_get_keyset_cursor_attributes1(),
			       conn->config_get_keyset_cursor_attributes2());
	}


	std::cout << "Batch support: "
		  << LIBCXX_NAMESPACE::join(conn->config_get_batch_support(),
					    ", ")
		  << std::endl;

	{
		auto stmt2=conn2->execute("DELETE FROM temptbl WHERE intkey=?; INSERT INTO temptbl(intkey, strval) VALUES(?, ?); UPDATE temptbl SET strval=''; UPDATE temptbl SET strval=intkey", 2, 2, "2");

		if (!stmt2->more())
			throw EXCEPTION("Where's more?");

		if (stmt2->row_count() != 1)
			throw EXCEPTION("Rowcount is not 1");

		if (!stmt2->more())
			throw EXCEPTION("Where's more?");

		if (!stmt2->more())
			throw EXCEPTION("Where's more?");

		if (stmt2->row_count() != 10)
			throw EXCEPTION("Rowcount is not 10");

		if (stmt2->more())
			throw EXCEPTION("What more there is to it?");
	}

	alarm(60);

	int cnt;

	{
		LIBCXX_NAMESPACE::sql::transaction tran(conn2);

		conn2->execute("INSERT INTO temptbl VALUES(10, 10)");

		conn->execute("SELECT COUNT(*) FROM temptbl")->fetch(0, cnt);

		if (cnt != 10)
			throw EXCEPTION("Another connection should not see a new record");

		{
			LIBCXX_NAMESPACE::sql::transaction tran(conn2);

			conn2->execute("INSERT INTO temptbl VALUES(11, 11)");

			tran.commit_work();
		}

		int dummy;

		conn2->execute("SELECT COUNT(*) FROM temptbl where intkey=11")
			->fetch(0, dummy);

		if (!dummy)
			throw EXCEPTION("Savepoint unexpectedly rolled back");


		{
			LIBCXX_NAMESPACE::sql::transaction tran(conn2);

			conn2->execute("DELETE FROM temptbl where intkey=11");

			LIBCXX_NAMESPACE::sql::transaction tran2(conn2);

			conn2->execute("DELETE FROM temptbl where intkey=10");
		}

		conn2->execute("SELECT COUNT(*) FROM temptbl where intkey=11")
			->fetch(0, dummy);

		if (!dummy)
			throw EXCEPTION("Savepoint unexpectedly committed");
	}

	conn->execute("SELECT COUNT(*) FROM temptbl")->fetch(0, cnt);

	if (cnt != 10)
		throw EXCEPTION("A rolled back transaction wasn't");

	{
		LIBCXX_NAMESPACE::sql::transaction tran(conn2);

		conn2->execute("INSERT INTO temptbl VALUES(10, 10)");

		tran.commit_work();
	}

	conn->execute("SELECT COUNT(*) FROM temptbl")->fetch(0, cnt);

	if (cnt != 11)
		throw EXCEPTION("A committed transaction wasn't");
	alarm(0);

	conn->execute("create table temptbl2(intkey int not null, strval text null, primary key(intkey))");

	testblobs<char>(conn, re, skip_fetch_vector_blobs);

	bool created=false;

	try {
		conn->execute("create table temptbl3(intkey int not null, strval1 blob, strval2 blob, primary key(intkey))");
		created=true;
	} catch (...) {
	}

	if (created)
	{
		testblobs<unsigned char>(conn, re, skip_fetch_vector_blobs);

		std::vector<unsigned char> blob1, blob2;

		blob1.insert(blob1.end(), 32000, 'A');
		blob2.insert(blob2.end(), 32000, 'B');

		conn->execute("INSERT INTO temptbl3(intkey, strval1, strval2) VALUES(?, ?, ?)",
			      0,
			      LIBCXX_NAMESPACE::sql::insertblob::create(blob1.begin(),
									blob1.end()),
			      LIBCXX_NAMESPACE::sql::insertblob::create(blob2.begin(),
									blob2.end()));
		std::cout << "Tested binary blob data type" << std::endl;
	} 
}

int main(int argc, char **argv)
{
	LIBCXX_NAMESPACE::option::list
		options(LIBCXX_NAMESPACE::option::list::create());

	LIBCXX_NAMESPACE::option::int_value flags_value(LIBCXX_NAMESPACE::option::int_value::create((int)LIBCXX_NAMESPACE::sql::connect_flags::noprompt));

	LIBCXX_NAMESPACE::option::string_value connect_value(LIBCXX_NAMESPACE::option::string_value::create());

	/*
	  Bug in mysql's ODBC connector.

	  SQLGetData (results.c):

	  length= irrec->row.datalen;

	  In a multi-row fetch, this is set to the length of this column in the
	  last row, and when we iterate over all rows, for this column, this
	  gets used in every row. Bad.
	*/

	auto skip_fetch_vector_blobs=
		LIBCXX_NAMESPACE::option::bool_value::create();

	auto seed_value=LIBCXX_NAMESPACE::ref<LIBCXX_NAMESPACE::option
					      ::valueObj<time_t>>
		::create(time(NULL));

	options->add(connect_value, 'c', L"connect",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     L"Make a test connection",
		     L"data_source")
		.add(flags_value, 'f', L"flags",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     L"Connection flag",
		     L"flag")
		.add(seed_value, 's', L"seed",
		     LIBCXX_NAMESPACE::option::list::base::hasvalue,
		     L"Random seed",
		     L"n")
		.add(skip_fetch_vector_blobs, 0, L"skip-fetch-vector-blobs",
		     0,
		     L"Skip fetch vector blobs test, until someone fixes http://bugs.mysql.com/bug.php?id=61991");

	options->addDefaultOptions();

	LIBCXX_NAMESPACE::option::parser
		parser(LIBCXX_NAMESPACE::option::parser::create());

	parser->setOptions(options);

	if (parser->parseArgv(argc, argv) ||
	    parser->validate())
		exit(1);

	auto seed=seed_value->value;
	try {
		if (connect_value->isSet())
		{
			std::cout << "Seed " << seed << std::endl;

			std::mt19937 re;
			re.seed(seed);

			testcursor(connect_value->value,
				   flags_value->value, re,
				   skip_fetch_vector_blobs->value);
		}
	} catch (const LIBCXX_NAMESPACE::exception &e) {
		std::cout << e << std::endl;
		std::cout << e->backtrace;
		std::cout << "Seed " << seed << std::endl;
		exit(1);
	}
	return 0;
}

