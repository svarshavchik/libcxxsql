/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/sql/env.H"
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
				       
int main(int argc, char **argv)
{
	LIBCXX_NAMESPACE::option::list
		options(LIBCXX_NAMESPACE::option::list::create());

	options->addDefaultOptions();

	LIBCXX_NAMESPACE::option::parser
		parser(LIBCXX_NAMESPACE::option::parser::create());

	parser->setOptions(options);

	if (parser->parseArgv(argc, argv) ||
	    parser->validate())
		exit(1);

	try {
		testenv();
	} catch (const LIBCXX_NAMESPACE::exception &e) {
		std::cout << e << std::endl;
		exit(1);
	}
	return 0;
}

