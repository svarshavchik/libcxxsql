#ifndef sql_internal_H
#define sql_internal_H

#include "x/sql/env.H"
#include "x/sql/exception.H"
#include "x/sql/exceptionobj.H"
#include <sql.h>
#include <sqlext.h>
#include <list>
#include <string>

namespace LIBCXX_NAMESPACE {
	namespace sql {
#if 0
	}
};
#endif

struct LIBCXX_HIDDEN exceptionObj::base {

	const char *function;
	SQLHANDLE handle;
	SQLSMALLINT type;

	base(const char *functionArg, SQLHANDLE handleArg, SQLSMALLINT typeArg);
};

class LIBCXX_HIDDEN diagnostics {

 public:

	exception e;

	diagnostics(const char *function, SQLHANDLE handle, SQLSMALLINT type);
	diagnostics();

	~diagnostics() noexcept;
};

void sql_error(const char *function,
	       SQLHANDLE handle,
	       SQLSMALLINT type) LIBCXX_HIDDEN;

class LIBCXX_HIDDEN envimplObj : public envObj {

 public:
	SQLHENV h;

	envimplObj();
	~envimplObj() noexcept;

	void get_data_sources(std::map<std::string,
			      std::string> &sources) const override;

	void get_drivers(std::map<std::string, std::string>
			 &drivers) const override;
};
#if 0
{
	{
#endif
	};
};

#endif
