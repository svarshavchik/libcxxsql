/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_statement_H
#define x_sql_statement_H

#include <x/sql/connection.H>
#include <x/sql/newstatement.H>
#include <x/sql/statementobj.H>
#include <x/sql/statementfwd.H>
#include <x/sql/env.H>
#include <x/ref.H>

namespace LIBCXX_NAMESPACE {
	namespace sql {
#if 0
	};
};
#endif

//! Base class for \ref env "environment handles".

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::sql::env::base.

class statementBase : public ptrrefBase {

public:

	//! Parameter metadata

	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::sql::statement::base::parameter

	typedef statementObj::parameter parameter;

	//! Resultset column metadata

	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::sql::statement::base::column.
	//! This is returned by \ref statementObj::get_columns "statement->get_columns()".

	typedef statementObj::column column;

	//! Resultset column metadata, keyed by column name

	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::sql::statement::base::columnmap.
	//! This is returned by \ref statementObj::get_columns "statement->get_columnmap()".

	typedef statementObj::columnmap_t columnmap_t;

};

template<typename ...Args>
statement connectionObj::execute(Args && ...args)
{
	return create_newstatement()->execute(std::forward<Args>(args)...);
}

template<typename ...Args>
statement connectionObj::execute_vector(Args && ...args)
{
	return create_newstatement()->
		execute_vector(std::forward<Args>(args)...);
}

template<typename ...Args>
statement newstatementObj::execute(const std::string &statement, Args && ...args)
{
	bitflag dummy;

	return execute(dummy, statement, std::forward<Args>(args)...);
}

template<typename ...Args>
statement newstatementObj::execute(bitflag &retval,
				   const std::string &statement,
				   Args && ...args)
{
	retval=0;
	auto s=prepare(statement);
	retval=s->execute(std::forward<Args>(args)...);
	return s;
}

template<typename ...Args>
statement newstatementObj::execute_vector(//! SQL statement to execute
					  const std::string &statement,

					  //! Return value of each executed row
					  std::vector<bitflag> &retvalues,

					  //! Parameters
					  Args && ...args)
{
	statementObj::prep_retvalues(retvalues);

	auto s=prepare(statement);
	s->begin_execute_params(&retvalues[0], retvalues.size());
	s->process_execute_vector_params(0, std::forward<Args>(args)...);
	s->verify_vector_execute(retvalues);
	return s;
}

//! Prepare a new statement, with custom options, given directly

template<typename firstArg, typename ...Args>
newstatement connectionObj::create_newstatement(firstArg &&arg, Args ...args)
{
	auto s=create_newstatement();
	set_newstatement_option(s, std::forward<firstArg>(arg),
				std::forward<Args>(args)...);
	return s;
}

template<typename ...Args>
void connectionObj::set_newstatement_option(const newstatement &ns,
					    const std::string &name,
					    const std::string &value,
					    Args ...args)
{
	ns->option(name, value);
	set_newstatement_option(ns, std::forward<Args>(args)...);
}

template<typename ...Args>
void connectionObj::set_newstatement_option(const newstatement &ns,
					    const std::map<std::string,
					    std::string>
					    &opts,
					    Args ...args)
{
	for (const auto &opt:opts)
		ns->option(opt.first, opt.second);
	set_newstatement_option(ns, std::forward<Args>(args)...);
}


#if 0
{
	{
#endif
	}
}
#endif
