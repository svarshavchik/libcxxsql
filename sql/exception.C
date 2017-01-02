/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "sql_internal.H"
#include "gettext_in.h"
#include "x/sql/exception.H"

#include <cstring>
#include <iostream>

namespace LIBCXX_NAMESPACE {
	namespace sql {
#if 0
	}
};
#endif

exception::base::base(const char *functionArg,
		      SQLHANDLE handleArg, SQLSMALLINT typeArg)
	: function(functionArg), handle(handleArg), type(typeArg)
{
}

exceptionObj::exceptionObj(base &&diagnostic_info)
	: function(diagnostic_info.function)
{
	SQLINTEGER	i = 0;

	while (1)
	{
		SQLINTEGER	native;
		SQLCHAR		state[6];
		SQLSMALLINT	len;

		len=0;
		if (!SQL_SUCCEEDED(SQLGetDiagRec(diagnostic_info.type,
						 diagnostic_info.handle, ++i,
						 state,
						 &native, nullptr, 0, &len)))
			break;

		SQLCHAR text[len+1];

		if (!SQL_SUCCEEDED(SQLGetDiagRec(diagnostic_info.type,
						 diagnostic_info.handle, i,
						 state,
						 &native, text, len+1, &len)))
			break;

		diagnostics.emplace_back(reinterpret_cast<const char *>
					 (state),
					 native,
					 reinterpret_cast<const char *>
					 (text));
	}
}

void exceptionObj::describe(LIBCXX_NAMESPACE::exception &e) const
{
	bool multiline=diagnostics.size() > 1;

	if (!multiline) // One line of errors
	{
		e << function << ": ";

		if (diagnostics.empty())
			e << " Unknown error";
	}
	else
	{
		e << function << ":";
	}

	for (const auto &diagnostic:diagnostics)
	{
		if (multiline)
		{
			e << "\n    ";
		}

		e << diagnostic.errcode << ":"
		  << diagnostic.errnumber
		  << ":"
		  << diagnostic.text;
	}
}

exceptionObj::exceptionObj() : function(nullptr)
{
}

exceptionObj::~exceptionObj()
{
}

exceptionObj::diagnostic::diagnostic(const char *errcodeArg,
				     int errnumberArg,
				     const char *textArg)
	: errnumber(errnumberArg), text(textArg)
{
	errcode[0]=0;

	strncat(errcode, errcodeArg, sizeof(errcode)-1);
}

exceptionObj::diagnostic::~diagnostic()
{
}

bool exceptionObj::diagnostic::operator==(const char *errcodeArg) const
{
	return strcmp(errcode, errcodeArg) == 0;
}

bool exceptionObj::is(const char *errcode) const
{
	for (const auto &diagnostic:diagnostics)
	{
		if (diagnostic != errcode)
			return false;
	}

	return true;
}

#if 0
{
	{
#endif
	};
};
