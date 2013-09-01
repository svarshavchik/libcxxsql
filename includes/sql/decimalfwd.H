/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_decimalfwd_H
#define x_sql_decimalfwd_H

#include <x/namespace.h>
#include <vector>
#include <string>

namespace LIBCXX_NAMESPACE {
	namespace sql {
#if 0
	};
};
#endif

class decimal_input_parameter;

//! Convert gmp mpf parameters to a string.

//! \internal
//!
//! Converts mpf parameters to execute() and execute_vector() to strings.

void decimal_input_parameter_to_strings(const decimal_input_parameter &,
					size_t,
					std::vector<std::string> &)
	__attribute__((weak));

#if 0
{
	{
#endif
	}
}
#endif
