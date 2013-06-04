/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/sql/dbi/constraint.H"
#include "gettext_in.h"

namespace LIBCXX_NAMESPACE {
	namespace sql {
		namespace dbi {
#if 0
		}
	}
};
#endif

constraintObj::constraintObj()
{
}

constraintObj::~constraintObj() noexcept
{
}

void constraintObj::get_sql_impl(std::ostream &o,
				 const std::string &name,
				 const std::string &cmptype)
{
	o << name << " " << cmptype << " ?";
}

void constraintObj::containerObj
::get_parameters(statementObj::execute_factory &factory) const
{
	for (const auto &v:*this)
	{
		v->get_parameters(factory);
	}
}

constraintObj::cmpObj<std::nullptr_t>::cmpObj(const std::string &nameArg,
					      const std::string &cmptypeArg,
					      std::nullptr_t valueArg)
	: txt(cmptypeArg == "=" ? ("(" + nameArg + " IS NULL)") :
	      cmptypeArg == "!=" ? ("(" + nameArg + " IS NOT NULL)") : "1=0")
{
}

constraintObj::cmpObj<std::nullptr_t>::~cmpObj() noexcept
{
}

void constraintObj::cmpObj<std::nullptr_t>::get_sql(std::ostream &o) const
{
	o << txt;
}

void constraintObj::cmpObj<std::nullptr_t>
::get_parameters(statementObj::execute_factory &factory) const
{
}

std::string constraintObj::get_vec_cmptype(const std::string &cmptypeArg)
{
	if (cmptypeArg != "=" && cmptypeArg != "!=")
		throw EXCEPTION(_TXT(_txt("Vector value list can only be compared using '=' or '!='")));

	return cmptypeArg;
}

void constraintObj::get_vec_sql_impl(std::ostream &o,
				     const std::string &name,
				     const std::string &cmptype,
				     size_t nvalues)
{
	if (nvalues == 0)
	{
		o << (cmptype == "=" ? "1=0" : name + " IS NOT NULL");
		return;
	}

	o << name << (cmptype == "=" ? " IN ":" NOT IN ");

	const char *sep="(";

	for (size_t i=0; i<nvalues; ++i)
	{
		o << sep << "?";
		sep=", ";
	}
	o << ")";
}

constraintObj::containerObj::~containerObj() noexcept
{
}

constraintObj::andObj::~andObj() noexcept
{
}

void constraintObj::andObj::get_sql(std::ostream &o) const
{
	if (empty())
	{
		o << "(1=1)";
	}

	const char *sep="(";

	for (const auto &v:*this)
	{
		o << sep;
		sep=" AND ";
		v->get_sql(o);
	}
	o << ")";
}

constraintObj::orObj::~orObj() noexcept
{
}

void constraintObj::orObj::get_sql(std::ostream &o) const
{
	if (empty())
	{
		o << "1=0";
	}

	const char *sep="(";

	for (const auto &v:*this)
	{
		o << sep;
		sep=" OR ";
		v->get_sql(o);
	}
	o << ")";
}

constraintObj::notObj::~notObj() noexcept
{
}

void constraintObj::notObj::get_sql(std::ostream &o) const
{
	o << "NOT ";
	andObj::get_sql(o);
}

#if 0
{
	{
		{
#endif
		};
	};
};
