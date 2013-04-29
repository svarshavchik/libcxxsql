/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "sql_internal.H"
#include <x/exception.H>

#include <list>
#include <sstream>
#include <string>

namespace LIBCXX_NAMESPACE {
	namespace sql {
#if 0
	}
};
#endif

diagnostics::diagnostics() : e(CUSTOM_EXCEPTION(exceptionObj))
{
}

diagnostics::diagnostics(const char *function,
			 SQLHANDLE handle, SQLSMALLINT type)
	: e(CUSTOM_EXCEPTION(exceptionObj,
			     exceptionObj::base(function, handle, type)))
{
}

diagnostics::~diagnostics() noexcept
{
}

// Common code for all handle type that throws an exception after an error.

void sql_error(const char *function,
	       SQLHANDLE handle,
	       SQLSMALLINT type)
{
	throw diagnostics(function, handle, type).e;
}

envObj::envObj()
{
}

envObj::~envObj() noexcept
{
}

envimplObj::envimplObj()
{
	if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &h)))
	{
		h=nullptr;
		sql_error("SQLAllocHandle", SQL_NULL_HANDLE, SQL_HANDLE_ENV);
	}

	if (!SQL_SUCCEEDED(SQLSetEnvAttr(h, SQL_ATTR_ODBC_VERSION,
					 (void *) SQL_OV_ODBC3, 0)))
	{
		sql_error("SQLSetEnvAtr(SQL_ATTR_ODBC_VERSION)",
			  h, SQL_HANDLE_ENV);
	}

}

envimplObj::~envimplObj() noexcept
{
	if (h)
		SQLFreeHandle(SQL_HANDLE_ENV, h);
}

env envBase::create()
{
	return ref<envimplObj>::create();
}

void envimplObj::get_data_sources(std::map<std::string,
				  std::string> &sources) const
{
	SQLCHAR		servername[SQL_MAX_DSN_LENGTH+1];
	SQLSMALLINT	servername_len;
	SQLCHAR		description[256];
	SQLSMALLINT     description_len;
	SQLUSMALLINT	direction=SQL_FETCH_FIRST;

	std::lock_guard<std::mutex> lock(objmutex);

	while (1)
	{
		auto ret=SQLDataSources(h, direction, servername,
					sizeof(servername),
					&servername_len,
					description,
					sizeof(description),
					&description_len);

		if (ret == SQL_NO_DATA_FOUND)
			break;

		if (!SQL_SUCCEEDED(ret))
			sql_error("SQLDataSources", h, SQL_HANDLE_ENV);
		direction=SQL_FETCH_NEXT;

		sources.insert(std::make_pair(reinterpret_cast<char *>
					      (servername),
					      reinterpret_cast<char *>
					      (description)));
	}
}

void envimplObj::get_drivers(std::map<std::string, std::string>
			     &drivers) const
{
	SQLCHAR	driver[256];
	SQLCHAR	description[256];
	SQLSMALLINT	driver_len;
	SQLSMALLINT	description_len;
	SQLUSMALLINT	direction=SQL_FETCH_FIRST;

	std::lock_guard<std::mutex> lock(objmutex);
	SQLRETURN ret;

	while (1)
	{
		ret = SQLDrivers(h, direction,
				 driver, sizeof(driver),
				 &driver_len,
				 description, sizeof(description),
				 &description_len);

		if (ret == SQL_NO_DATA_FOUND)
			break;

		if (!SQL_SUCCEEDED(ret))
			sql_error("SQLDrivers", h, SQL_HANDLE_ENV);
		direction=SQL_FETCH_NEXT;

		drivers.insert(std::make_pair(reinterpret_cast<char *>
					      (driver),
					      reinterpret_cast<char *>
					      (description)));
	}
}

#if 0
{
	{
#endif
	};
};
