/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_exceptionobj_H
#define x_sql_exceptionobj_H

#include <x/obj.H>
#include <x/exceptionfwd.H>
#include <x/namespace.h>
#include <vector>

namespace LIBCXX_NAMESPACE {
	namespace sql {
#if 0
	};
};
#endif

//! A thrown SQL exception object.

//! \see exception

class exceptionObj : virtual public obj {

public:
	//! \internal
	class base;

	//! Constructor
	exceptionObj(base &&diagnostic_info);

	//! Constructor
	exceptionObj();

	//! Destructor
	~exceptionObj();

	//! Individual diagnostic

	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::sql::exception::base::diagnostic

	struct diagnostic {

		//! Five-character diagnostic code

		char errcode[6];

		//! Native diagnostic code
		int errnumber;

		//! Diagnostic text
		std::string text;

		//! Constructor
		diagnostic(const char *errcodeArg,
			   int errnumberArg,
			   const char *textArg);

		//! Destructor
		~diagnostic();

		//! Compare against an official error code
		bool operator==(const char *errcodeArg) const;

		//! Compare against an official error code
		bool operator!=(const char *errcodeArg) const
		{
			return !operator==(errcodeArg);
		}
	};

	//! The collected diagnostics
	std::vector<diagnostic> diagnostics;

	//! Returns false if any diagnostic's error code in not this one.
	bool is(const char *errcode) const;

	//! Saved function name

	const char *function;

	//! Describe this diagnostic, as an exception

	//! \internal
	void describe(LIBCXX_NAMESPACE::exception &e) const;
};

#if 0
{
	{
#endif
	}
}
#endif
