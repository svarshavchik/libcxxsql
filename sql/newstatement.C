/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "sql_internal.H"
#include "x/chrcasecmp.H"
#include "x/messages.H"
#include "gettext_in.h"
#include <algorithm>
#include <sstream>

namespace LIBCXX_NAMESPACE {
	namespace sql {
#if 0
	}
};
#endif

newstatementObj::newstatementObj()
{
}

newstatementObj::~newstatementObj() noexcept
{
}

newstatementimplObj::newstatementimplObj(ref<connectionimplObj> &&connArg)
	: conn(std::move(connArg))
{
}

newstatementimplObj::~newstatementimplObj() noexcept
{
}

statement newstatementimplObj::prepare(const std::string &sql)
{
	return prepare(newstmt(), sql);
}

statement newstatementimplObj::prepare(const ref<statementimplObj> &s,
				       const std::string &sql)
{
	if (!cursor_name.empty())
		s->ret(SQLSetCursorName(s->h, to_sqlcharptr(cursor_name),
					SQL_NTS), "SQLSetCursorName");

	s->ret(SQLPrepare(s->h, to_sqlcharptr(sql), SQL_NTS), "SQLPrepare");
	s->save_num_params();
	return s;
}

ref<statementimplObj> newstatementimplObj::newstmt()
{
	std::lock_guard<std::mutex> lock(conn->objmutex);

	auto s=ref<statementimplObj>::create(conn);

	for (const auto &attr:ulen_attributes)
		s->ret(SQLSetStmtAttr(s->h, attr.first,
				      (SQLPOINTER)attr.second.second,
				      0), attr.second.first);
	return s;
}

void newstatementimplObj::option(const std::string &name,
				 const std::string &setting)
{
	chrcasecmp::str_equal_to is;

	if (is(name, "CURSOR_TYPE"))
	{
		std::string::const_iterator b=setting.begin(),
			e=setting.end(), p;
		SQLULEN value=0;

		if (is(setting, "FORWARD"))
		{
			STMT_ATTR(SQL_ATTR_CURSOR_TYPE, ulen,
				  SQL_CURSOR_FORWARD_ONLY);
			return;
		}
		else if (is(setting, "STATIC"))
		{
			STMT_ATTR(SQL_ATTR_CURSOR_TYPE, ulen,
				  SQL_CURSOR_STATIC);
			return;
		} else if (is(std::string(b, (p=std::find(b, e, '('))),
			      "KEYSET")
			   && p != e && ((b=++p), 1)
			   && (p=std::find(b, e, ')')) != e && p != b
			   && !(std::istringstream(std::string(b, p)) >> value)
			   .fail()
			   && value > 0)
		{
			STMT_ATTR(SQL_ATTR_CURSOR_TYPE, ulen,
				  SQL_CURSOR_KEYSET_DRIVEN);
			STMT_ATTR(SQL_ATTR_KEYSET_SIZE, ulen, value);
			return;
		}
		else if (is(setting, "DYNAMIC"))
		{
			STMT_ATTR(SQL_ATTR_CURSOR_TYPE, ulen,
				  SQL_CURSOR_DYNAMIC);
			return;
		}
	}
	else if (is(name, "BOOKMARKS"))
	{
		if (is(setting, "ON"))
		{
			STMT_ATTR(SQL_ATTR_USE_BOOKMARKS, ulen, SQL_UB_VARIABLE);
			return;
		}
		else if (is(setting, "OFF"))
		{
			STMT_ATTR(SQL_ATTR_USE_BOOKMARKS, ulen, SQL_UB_OFF);
			return;
		}
	}
	else if (is(name, "CURSOR_NAME"))
	{
		cursor_name=setting;
		return;
	}
	else
	{
		throw EXCEPTION((std::string)
				gettextmsg(_TXT(_txt("\"%1\" is not a known "
						     "option")),
					   name));
	}

	throw EXCEPTION((std::string)
			gettextmsg(_TXT(_txt("\"%1\" is not a valid setting for"
					     " %2%")),
				   setting, name));
}

void newstatementimplObj::set_attribute_ulen(SQLINTEGER n,
					     const char *n_cstr,
					     SQLULEN v)
{
	auto &vref=ulen_attributes[n];
	vref.first=n_cstr;
	vref.second=v;
}

#if 0
{
	{
#endif
	};
};
