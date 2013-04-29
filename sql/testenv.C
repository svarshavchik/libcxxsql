/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/sql/env.H"
#include "x/sql/connection.H"
#include <x/options.H>
#include <iostream>
#include <iomanip>

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

void testconnect(const std::string &connection,
		 int flags)
{
	LIBCXX_NAMESPACE::sql::env::create()
		->connect(connection,
			  (LIBCXX_NAMESPACE::sql::connect_flags)flags);
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

