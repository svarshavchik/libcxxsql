/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_decimal_H
#define x_sql_decimal_H

#include <x/obj.H>
#include <gmpxx.h>

namespace LIBCXX_NAMESPACE {
	namespace sql {
#if 0
	};
};
#endif

//! Stub class to bind mpf_class parameters to execute() and execute_vector()

//! This class is visible only upon explicit include of decimal.H. Its
//! constructor takes a constant pointer to mpf_class. Passing
//! an mpf_class parameter to \ref statement "statement"'s execute() or
//! execute_vector() eventually calls a private virtual function with a
//! pointer to the argument, which in this case is a pointer to gmp's
//! mpf_class.
//!
//! In this manner, we don't pollute the default namespace with libgmp's
//! symbols unless this header gets explicitly included.

class decimal_input_parameter {

public:
	//! The parameters

	const mpf_class *params;

	//! Constructor
	decimal_input_parameter(const mpf_class *paramsArg);

	//! Destructor
	~decimal_input_parameter() noexcept
	{
	}
};

//! Stub class to bind mpf_class parameters to fetched columns,

//! This class is visible only upon explicit include of decimal.H. Its
//! constructor takes a pointer to mpf_class. Passing
//! an mpf_class parameter to \ref statement "statement"'s fetch() or
//! fetch_vector() eventually calls a private virtual function with an
//! instance of this class as an argument,
//!
//! In this manner, we don't pollute the default namespace with libgmp's
//! symbols unless this header gets explicitly included.

class decimal_bound_parameter {

public:
	//! Bound parameters
	mpf_class *params;

	//! Constructor
	decimal_bound_parameter(mpf_class *paramsArg);

	//! Destructor
	~decimal_bound_parameter() noexcept
	{
	}
};

#if 0
{
	{
#endif
	}
}
#endif
