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

decimal_input_parameter::decimal_input_parameter(const mpf_class *paramsArg)
	: params(paramsArg)
{
}

void decimal_input_parameter_to_strings(const decimal_input_parameter &params,
					size_t n,
					std::vector<std::string> &output)
{
	output.reserve(n);

	for (size_t i=0; i<n; i++)
	{
		mp_exp_t exp;
		std::string n=params.params[i].get_str(exp);
		size_t digits;

		std::string minus;

		if (n[0] == '-')
		{
			minus="-";
			n=n.substr(1);
		}
		digits=n.size();

		if (exp < 0)
		{
			if (exp >= -4)
			{
				n="0." + std::string(-exp, '0') + n;
			}
			else
			{
				std::ostringstream o;

				o << "0." << n << "e" << exp;

				n=o.str();
			}
		}
		else
		{
			if (exp < digits)
			{
				n=n.substr(0, exp) + "." + n.substr(exp);
			}
			else
			{
				size_t zeroes=exp-digits;

				if (exp <= 8)
				{
					n.append(zeroes, '0');
				}
				else
				{
					std::ostringstream o;

					o << n << "e+" << zeroes;
					n=o.str();
				}
			}
		}

		if (n.size() == 0)
			n="0";

		output.push_back(minus + n);
	}
}

#if 0
{
	{
#endif
	};
};
