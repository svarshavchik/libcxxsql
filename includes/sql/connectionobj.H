/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_connectionobj_H
#define x_sql_connectionobj_H

#include <x/obj.H>
#include <x/sql/statementfwd.H>
#include <x/sql/newstatementfwd.H>
#include <x/chrcasecmp.H>
#include <map>
#include <set>

namespace LIBCXX_NAMESPACE {
	namespace sql {
#if 0
	};
};
#endif

//! Return value for bitmask-based configuration settings

//! This is used by bitmask-based configuration settings,
//! like \ref connectionObj::config_get_alter_table "connection->config_get_alter_table"().

typedef std::set<const char *, chrcasecmp::str_less> config_bitmask_t;

//! SQL connection

class connectionObj : virtual public obj {

public:
	//! Constructor
	connectionObj();

	//! Destructor
	~connectionObj() noexcept;

	//! Disconnect

	//! Use this to explicitly disconnect.
	virtual void disconnect()=0;

private:

	//! Helper class determines presence of optional initial \c bool arg to tables()

	template<typename argType> class tablesFirstArg {
	public:

		//! Default implementation, \c bool initial parameter not given.

		//! Default it to \c false.

		template<typename ...Arg>
		static inline statement tables(connectionObj &obj,
					       Arg && ...arg)
		{
			return obj.tables_catalog(std::forward<Arg>(arg)...);
		}
	};

public:
	//! Enumerate available tables()

	statement tables()
	{
		return tables_catalog(false);
	}

	//! Enumerate available tables()

	//! \overload
	template<typename firstArg, typename ...Args>
	statement tables(firstArg &&arg,
			 Args && ...args)
	{
		return tablesFirstArg<typename std::remove_cv<
			typename std::remove_reference
					      <firstArg>::type>::type>
			::tables(*this,
				 std::forward<firstArg>(arg),
				 std::forward<Args>(args)...);
	}

	//! Prepare a new statement
	virtual statement prepare(const std::string &)=0;

	//! Prepare and execute a statement using the default options.

	//! This is equivalent to calling create_newstatement()->execute().

	template<typename ...Args>
	inline statement execute(//! Forwarded to newstatement
				 Args && ...args);

	//! Prepare and execute a statement with vector parameters, in one shot.

	//! This is equivalent to calling create_newstatement()->execute_vector().
	template<typename ...Args>
	inline statement execute_vector(//! Parameters
					Args && ...args);

	//! Common use case of executing a simple literal string.

	//! Avoid template code bloat.

	template<size_t n>
	inline statement execute(const char (&a)[n])
	{
		return execute(&a[0]);
	}

#include <x/sql/getinfo.H>

	//! Prepare a new statement, with custom options.
	virtual newstatement create_newstatement()=0;

	//! Prepare a new statement, with custom options, given directly

	template<typename firstArg, typename ...Args>
	newstatement create_newstatement(firstArg &&arg, Args ...args);

	//! Set/clear autocommit flag
	virtual void autocommit(bool value)=0;

	//! Manual commit
	virtual void commit(//! Whether to turn on the autocommit flag simultaneously with the commit
			    bool turn_on_autocommit=false)=0;

	//! Rollback
	virtual void rollback(//! Whether to turn on the autocommit flag simultaneously with the rollback
			    bool turn_on_autocommit=false)=0;

	//! Begin transactional scope

	//! The first call executes "START TRANSACTION" SQL. Subsequent
	//! calls, without intervening calls to commit_work() or rollback_work()
	//! execute "SAVEPOINT" SQL.

	void begin_work();

	//! Begin transactional scope

	//! \overload
	virtual void begin_work(//! Database driver-specific options
				const std::string &options)=0;

	//! Commit transactional scope

	//! Executes RELEASE SAVEPOINT or COMMIT WORK SQL
	virtual void commit_work()=0;

	//! Rollback transactional scope

	//! This is the same as calling rollback() with the disable autocommit
	//! flag.

	virtual void rollback_work()=0;

private:

	//! No more options given to create_newstatement()
	void set_newstatement_option(const newstatement &ns)
	{
	}

	//! Forward options given to create_newstatement() to ns->option()
	template<typename ...Args>
	void set_newstatement_option(const newstatement &ns,
				     const std::string &name,
				     const std::string &value,
				     Args ...args);

	//! Forward options given to create_newstatement() to ns->option()
	template<typename ...Args>
	void set_newstatement_option(const newstatement &ns,
				     const std::map<std::string, std::string>
				     &opts,
				     Args ...args);

	//! Retrieve table catalogs

	virtual statement tables_catalog(bool literal_ids,
					 const std::string &catalog="",
					 const std::string &schema="",
					 const std::string &table="",
					 const std::string &type="")=0;

};

//! Specialization: there's an initial initial \c bool arg to tables()

template<> class connectionObj::tablesFirstArg<bool> {
public:

	//! Forward the initial bool parameter value.

	template<typename ...Arg>
	static inline statement tables(connectionObj &obj,
				       bool flag,
				       Arg && ...arg)
	{
		return obj.tables_catalog(flag, std::forward<Arg>(arg)...);
	}
};

#ifndef DOXYGEN
extern template statement connectionObj::execute(const char * &&);
extern template statement connectionObj::execute(std::string &&);
#endif

#if 0
{
	{
#endif
	}
}
#endif
