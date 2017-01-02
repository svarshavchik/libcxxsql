/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "sql_internal.H"
#include "gettext_in.h"
#include "x/sql/decimal.H"
#include "x/sql/decimalfwd.H"

namespace LIBCXX_NAMESPACE {
	namespace sql {
#if 0
	}
};
#endif

decimal_bound_parameter::decimal_bound_parameter(mpf_class *paramsArg)
	: params(paramsArg)
{
}

class LIBCXX_HIDDEN boundDecimalObj
	: public statementimplObj::bound_indicator::stringsbaseObj {

 public:
	mpf_class *params;

	boundDecimalObj(const decimal_bound_parameter &paramsArg,
			size_t recsize,
			size_t row_array_size,
			SQLPOINTER &targetRet,
			SQLLEN &buffer_lengthRet)
		: stringsbaseObj(recsize, row_array_size),
		params(paramsArg.params)
	{
		targetRet=reinterpret_cast<SQLPOINTER>(&string_buffer[0]);
		buffer_lengthRet=string_size;
	}

	~boundDecimalObj()
	{
	}

	void bind(const statementimplObj::bound_indicator &b,
		  statementimplObj &statement) override
	{
		const char *p=&string_buffer[0];
		auto s=params;
		auto indicators=&b.indicators[0];
		size_t num_rows_fetched=b.indicators.size();

		for (size_t i=0; i<num_rows_fetched;
		     ++i, p += string_size, ++indicators, ++s)
		{
			if (*indicators == SQL_NULL_DATA ||
			    *indicators == 0)
				continue;

			(*s)=p;
		}
	}
};

ref<statementimplObj::bound_indicator::baseObj>
bind_decimal_column(const decimal_bound_parameter &params,
		    size_t recsize,
		    size_t row_array_size,
		    SQLPOINTER &targetRet,
		    SQLLEN &buffer_lengthRet)
{
	return ref<boundDecimalObj>::create(params, recsize,
					    row_array_size,
					    targetRet,
					    buffer_lengthRet);
}


#if 0
{
	{
#endif
	};
};
