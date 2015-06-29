/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "sql_internal.H"
#include "gettext_in.h"
#include <x/exception.H>
#include <x/property_value.H>

#include <list>
#include <sstream>
#include <string>
#include <cstring>
#include <odbcinst.h>

namespace LIBCXX_NAMESPACE {
	namespace sql {
#if 0
	}
};
#endif

// Specifies the module that prompts for any additional connection
// parameters. The default value references the
// unixODBC-gui-qt module.

static property::value<std::string> ui_property(LIBCXX_NAMESPACE_STR
						"::sql::uiprompt",
						"odbcinstQ4");

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

envimplObj::envimplObj() : login_timeout_set(false)
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

std::pair<connection, std::string>
envimplObj::connect(const std::string &connection_parameters,
		    connect_flags flags)
{
	ODBCINSTWND wnd;

	{
		auto str=ui_property.getValue();

		wnd.szUI[0]=0;
		strncat(wnd.szUI, str.c_str(), sizeof(wnd.szUI)-1);
	}
	wnd.hWnd=0;

	std::lock_guard<std::mutex> lock(objmutex);

	static const int flagmap[]={
		SQL_DRIVER_PROMPT,
		SQL_DRIVER_COMPLETE,
		SQL_DRIVER_COMPLETE_REQUIRED,
		SQL_DRIVER_NOPROMPT,
	};

	SQLCHAR out_connection_parameters[connection_parameters.size() * 2
					  + 1024];
	SQLSMALLINT out_connection_parameters_len;

	auto conn=ref<connectionimplObj>::create(ref<envimplObj>(this));

	if (login_timeout_set)
		conn->CONN_ATTR(SQL_ATTR_LOGIN_TIMEOUT, uint, login_timeout);

	conn->ret(SQLDriverConnect(conn->h, reinterpret_cast<SQLHWND>(&wnd),
				   to_sqlcharptr(connection_parameters),
				   SQL_NTS,
				   out_connection_parameters,
				   sizeof(out_connection_parameters),
				   &out_connection_parameters_len,
				   (size_t)flags >= 0 &&
				   (size_t)flags < sizeof(flagmap)/
				   sizeof(flagmap[0]) ?
				   flagmap[(size_t)flags]:SQL_DRIVER_NOPROMPT),
		  "SQLDriverConnect");

	conn->connected=true;
	conn->connstring=
		std::string(reinterpret_cast<const char *>
			    (out_connection_parameters));

	return std::make_pair(conn, conn->connstring);
}

std::pair<connection, std::string>
envObj::connect(const std::string &connection_parameters)
{
	return connect(connection_parameters, connect_flags::noprompt);
}

// Look for bad characters in discreet connection string parameter names/values

static bool bad(const std::string &s)
{
	return std::find_if(s.begin(), s.end(),
			    []
			    (char c)
			    {
				    return strchr("[]{}(),;?*=!@\\", c)
					    ? 1:0;
			    }) == s.end();
}

// Build a connection string from explicit name=value tuples.

std::pair<connection, std::string>
envObj::connect(const arglist_t &args, connect_flags flags)
{
	std::ostringstream o;

	for (const auto &arg:args)
	{
		if (bad(arg.first) || bad(arg.second))
			throw EXCEPTION(_TXT(_txt("Invalid character in connection parameter")));
		o << arg.first << '=' << arg.second << ';';
	}

	return connect(o.str(), flags);
}

void envimplObj::set_login_timeout(time_t t)
{
	std::lock_guard<std::mutex> lock(objmutex);

	login_timeout_set=true;
	login_timeout=t;
}

void envimplObj::clear_login_timeout()
{
	std::lock_guard<std::mutex> lock(objmutex);

	login_timeout_set=false;
}


#if 0
{
	{
#endif
	};
};
