/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_newstatementobj_H
#define x_sql_newstatementobj_H

#include <x/obj.H>
#include <x/sql/statementfwd.H>

namespace LIBCXX_NAMESPACE {
	namespace sql {
#if 0
	};
};
#endif

//! A new SQL statement about to be prepared

//! This object collects options to prepare an SQL statement for execution.

class newstatementObj : virtual public obj {

public:
	//! Constructor
	newstatementObj();

	//! Destructor
	~newstatementObj() noexcept;

	//! Set an option for the next prepared statement
	virtual void option(const std::string &name,
			    const std::string &value)=0;

	//! Prepare a new statement, using current options.
	virtual statement prepare(const std::string &sql)=0;

	//! Prepare and execute a statement, in one shot.

	template<typename ...Args>
	statement execute(//! SQL statement to execute
			  const std::string &statement,

			  //! Parameters
			  Args && ...args);

	//! Prepare and execute a statement, in one shot.

	//! \overload
	template<typename ...Args>
	statement execute(//! Return value of statement->execute()
			  bitflag &status,
			  
			  //! SQL statement to execute
			  const std::string &statement,

			  //! Parameters
			  Args && ...args);

	//! Prepare and execute a statement with vector parameters, in one shot.

	template<typename ...Args>
	statement execute_vector(//! SQL statement to execute
				 const std::string &statement,

				 //! Return value of each executed row
				 std::vector<bitflag> &retvalues,

				 //! Parameters
				 Args && ...args);

};

#ifndef DOXYGEN
extern template statement newstatementObj::execute(const std::string &);
#endif

#if 0
{
	{
#endif
	}
}
#endif
