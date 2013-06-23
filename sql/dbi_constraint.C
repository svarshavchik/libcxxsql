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

void constraintObj::only_equ()
{
	throw EXCEPTION(_TXT(_txt("Only \"=\" constraints can be specified for INSERT or UPDATE statements")));
}

void constraintObj::get_sql(std::vector<std::string> &fields,
			    std::vector<std::string> &placeholders,
			    std::vector<const_constraint> &constraints) const
{
	only_equ();
}

void constraintObj::get_sql_impl(std::ostream &o,
				 const std::string &name,
				 const std::string &cmptype)
{
	o << name << " " << cmptype << " ?";
}

void constraintObj::get_sql_impl(std::vector<std::string> &fields,
				 std::vector<std::string> &placeholders,
				 std::vector<const_constraint> &constraints,
				 const_constraint &&this_constraint,
				 const std::string &name,
				 const std::string &cmptype,
				 const char *placeholder)
{
	if (cmptype != "=")
		only_equ();

	fields.push_back(name);
	placeholders.push_back(placeholder);
	constraints.push_back(std::move(this_constraint));
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
	      cmptypeArg == "!=" ? ("(" + nameArg + " IS NOT NULL)") : "1=0"),
	  name(cmptypeArg == "=" ? nameArg:"")
{
}

void constraintObj::cmpObj<std::nullptr_t>
::get_sql(std::vector<std::string> &fields,
	  std::vector<std::string> &placeholders,
	  std::vector<const_constraint> &constraints) const
{
	if (name.empty())
		only_equ();
	get_sql_impl(fields, placeholders, constraints,
		     const_constraint(this), name, "=", "NULL");
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

constraintObj::rawcmpObj::rawcmpObj(const std::string &nameArg,
				    const std::string &cmptypeArg,
				    const std::string &rawsqlArg)
	: name(nameArg),
	  cmptype(cmptypeArg),
	  rawsql(rawsqlArg)
{
}

constraintObj::rawcmpObj::rawcmpObj(const std::string &nameArg,
				    const std::string &cmptypeArg,
				    const std::list<std::string> &rawsqlArg)
	: name(nameArg),
	  cmptype(cmptypeArg)
{
	auto b=rawsqlArg.begin(), e=rawsqlArg.end();

	if (b != e)
		rawsql=*b++;

	placeholders.insert(placeholders.end(), b, e);
}

constraintObj::rawcmpObj::rawcmpObj(const std::string &nameArg,
				    const std::string &cmptypeArg,
				    std::string &&rawsqlArg)
	: name(nameArg),
	  cmptype(cmptypeArg),
	  rawsql(std::move(rawsqlArg))
{
}

constraintObj::rawcmpObj::rawcmpObj(const std::string &nameArg,
				    const std::string &cmptypeArg,
				    std::list<std::string> &&rawsqlArg)
	: name(nameArg),
	  cmptype(cmptypeArg),
	  placeholders(std::move(rawsqlArg))
{
	if (!placeholders.empty())
	{
		rawsql=placeholders.front();
		placeholders.pop_front();
	}
}

constraintObj::rawcmpObj::~rawcmpObj() noexcept
{
}

void constraintObj::rawcmpObj::get_sql(std::ostream &o) const
{
	o << name << " " << cmptype << " " << rawsql;
}

void constraintObj::rawcmpObj
::get_sql(std::vector<std::string> &fields,
	  std::vector<std::string> &placeholders,
	  std::vector<const_constraint> &constraints) const
{
	get_sql_impl(fields, placeholders,
		     constraints,
		     const_constraint(this), name, cmptype, rawsql.c_str());
}

void constraintObj::rawcmpObj::get_parameters(statementObj::execute_factory
						 &factory) const
{
	for (const auto &v:placeholders)
	{
		factory.parameter(v);
	}
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

void constraintObj::andObj::get_sql(std::vector<std::string> &fields,
				    std::vector<std::string> &placeholders,
				    std::vector<const_constraint> &constraints)
	const
{
	for (const auto &v:*this)
		v->get_sql(fields, placeholders, constraints);
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
