/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "sql_internal.H"
#include <algorithm>
#include <cstring>
#include <sstream>
#include "gettext_in.h"
#include "x/exception.H"

LOG_CLASS_INIT(LIBCXX_NAMESPACE::sql::connectionimplObj);

namespace LIBCXX_NAMESPACE {
	namespace sql {
#if 0
	}
};
#endif

connectionObj::connectionObj()
{
}

connectionObj::~connectionObj() noexcept
{
}

connectionimplObj::connectionimplObj(ref<envimplObj> &&envArg)
	: h(nullptr), connected(false), env(std::move(envArg))
{
	if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_DBC, env->h, &h)))
	{
		h=nullptr;
		sql_error("SQLAllocHandle", env->h, SQL_HANDLE_ENV);
	}
}

connectionimplObj::~connectionimplObj() noexcept
{
	try {
		disconnect();
	} catch (const LIBCXX_NAMESPACE::exception &e) {
		LOG_ERROR(e);
		LOG_TRACE(e->backtrace);
	}
	if (h)
		SQLFreeHandle(SQL_HANDLE_DBC, h);
}

// Check the error from an SQL connection handle call

// Throw an exception if there was an error. Log a diagnostic at the warning
// level.

void connectionimplObj::ret(SQLRETURN ret, const char *func)
{
	if (!SQL_SUCCEEDED(ret))
		sql_error(func, h, SQL_HANDLE_DBC);

	if (ret == SQL_SUCCESS_WITH_INFO)
	{
		diagnostics diags(func, h, SQL_HANDLE_DBC);

		LOG_WARNING(diags.e);
	}
}

void connectionimplObj::disconnect()
{
	std::lock_guard<std::mutex> lock(objmutex);

	do_disconnect();
}

void connectionimplObj::do_disconnect()
{
	if (!connected)
		return;

	connected=false;
	ret(SQLDisconnect(h), "SQLDisconnect");
}

#if 0
{
	{
#endif
	};
};
