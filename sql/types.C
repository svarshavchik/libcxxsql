/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "sql_internal.H"
#include "gettext_in.h"
#include "x/exception.H"
#include <cstring>
#include <algorithm>

namespace LIBCXX_NAMESPACE {
	namespace sql {
#if 0
	}
};
#endif

// Evil hack to take all SQL type names, and generate:
//
// 1) A const char array, of all SQL type names, separated by \0s.
// 2) A function that maps an SQL type to a type name.
// 3) A function that maps a type name to an SQL type


static const char type_names[]={

#define T(index, code, name) name "\0"
#include "sql_internal_type_list.H"
#undef T

};

// constexpr function that returns the length of each name+null byte

static constexpr size_t typename_i(size_t i)
{
	return
#define T(index, code, name) i == index ? sizeof(name):
#include "sql_internal_type_list.H"
#undef T
		0;
}

// constexpr computes offset of each one in type_names

static constexpr size_t typename_offset(size_t i)
{
	return i == 0 ? 0:typename_i(i-1) + typename_offset(i-1);
}

const char *type_to_name(SQLLEN n)
{
	size_t i=0;

	switch (n) {
#define T(index, code, name) case code: i=typename_offset(index); break;
#include "sql_internal_type_list.H"
#undef T
	}

	return type_names + i;
}

SQLLEN name_to_type(const char *n)
{
	size_t l=strlen(n);
	char buf[l+1];

	strcpy(buf, n);

	std::transform(buf, buf+l, buf, []
		       (char c)
		       {
			       return (c >= 'A' && c <= 'Z' ?
				       (c) + ('a'-'A'):(c));
		       });

	const char *p=type_names;

#define T(index, code, name) if (strcmp(buf, p + typename_offset(index)) == 0) return code;
#include "sql_internal_type_list.H"
#undef T

	return SQL_UNKNOWN_TYPE;
}

#if 0
{
	{
#endif
	};
};
