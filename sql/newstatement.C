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
	auto s=newstmt();

	s->ret(SQLPrepare(s->h, to_sqlcharptr(sql), SQL_NTS), "SQLPrepare");
	s->save_num_params();
	return s;
}

ref<statementimplObj> newstatementimplObj::newstmt()
{
	std::lock_guard<std::mutex> lock(conn->objmutex);

	auto s=ref<statementimplObj>::create(conn);

	return s;
}

void newstatementimplObj::option(const std::string &name,
				 const std::string &value)
{
}

#if 0
{
	{
#endif
	};
};
