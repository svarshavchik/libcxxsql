/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_connectionobj_H
#define x_sql_connectionobj_H

#include <x/obj.H>
#include <x/sql/connectionfwd.H>
#include <x/sql/statementfwd.H>
#include <x/sql/newstatementfwd.H>
#include <x/sql/dbi/flavorfwd.H>
#include <x/chrcasecmp.H>
#include <x/mpobj.H>
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

//! First parameter to connection->special_columns().

enum class rowid_t {
	unique, //!< Unique identifier for a row, primary key or db-specific rowid
	version, //!< Row version, is modified if row changes
		};

//! Second parameter to connection->special_columns().

enum class scope_t {
	currow, //!< identifier persists only while positioned on that row.
		
	transaction, //!< identifier valid for the duration of current transaction.
	session, //!< identifier persists across transactions
		};

//! SQL connection

class connectionObj : virtual public obj {

public:
	//! Constructor
	connectionObj();

	//! Destructor
	~connectionObj();

	//! Disconnect

	//! Use this to explicitly disconnect.
	virtual void disconnect()=0;

	//! Clone this connection
	virtual connection clone() const=0;
private:

	//! Installed DBI flavor

	mpobj<dbi::flavorptr> dbiflavor;

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

	//! Helper class determines presence of optional initial \c bool arg to table_privileges()

	template<typename argType> class table_privilegesFirstArg {
	public:

		//! Default implementation, \c bool initial parameter not given.

		//! Default it to \c false.

		template<typename ...Arg>
		static inline statement table_privileges(connectionObj &obj,
							 Arg && ...arg)
		{
			return obj.table_privileges_catalog(false, std::forward<Arg>(arg)...);
		}
	};

	//! Helper class determines presence of optional initial \c bool arg to columns()

	template<typename argType> class columnsFirstArg {
	public:

		//! Default implementation, \c bool initial parameter not given.

		//! Default it to \c false.

		template<typename ...Arg>
		static inline statement columns(connectionObj &obj,
						Arg && ...arg)
		{
			return obj.columns_catalog(false,
						   std::forward<Arg>(arg)...);
		}
	};

	//! Helper class determines presence of optional initial \c bool arg to column_privileges()

	template<typename argType> class column_privilegesFirstArg {
	public:

		//! Default implementation, \c bool initial parameter not given.

		//! Default it to \c false.

		template<typename ...Arg>
		static inline statement column_privileges(connectionObj &obj,
							  Arg && ...arg)
		{
			return obj.column_privileges_catalog(false,
							     std::forward<Arg>
							     (arg)...);
		}
	};

	//! Helper class determines presence of optional initial \c bool arg to foreign_keys()

	template<typename argType> class foreign_keysFirstArg {
	public:

		//! Default implementation, \c bool initial parameter not given.

		//! Default it to \c false.

		template<typename ...Arg>
		static inline statement foreign_keys(connectionObj &obj,
						     Arg && ...arg)
		{
			return obj.foreign_keys_catalog(false,
							std::forward<Arg>
							(arg)...);
		}
	};

	//! Helper class determines presence of optional initial \c bool arg to primary_keys()

	template<typename argType> class primary_keysFirstArg {
	public:

		//! Default implementation, \c bool initial parameter not given.

		//! Default it to \c false.

		template<typename ...Arg>
		static inline statement primary_keys(connectionObj &obj,
						     Arg && ...arg)
		{
			return obj.primary_keys_catalog(false,
							std::forward<Arg>
							(arg)...);
		}
	};

	//! Helper class determines presence of optional initial \c bool arg to procedure_columns()

	template<typename argType> class procedure_columnsFirstArg {
	public:

		//! Default implementation, \c bool initial parameter not given.

		//! Default it to \c false.

		template<typename ...Arg>
		static inline statement procedure_columns(connectionObj &obj,
							  Arg && ...arg)
		{
			return obj.procedure_columns_catalog(false,
							     std::forward<Arg>
							     (arg)...);
		}
	};

	//! Helper class determines presence of optional initial \c bool arg to procedures()

	template<typename argType> class proceduresFirstArg {
	public:

		//! Default implementation, \c bool initial parameter not given.

		//! Default it to \c false.

		template<typename ...Arg>
		static inline statement procedures(connectionObj &obj,
						   Arg && ...arg)
		{
			return obj.procedures_catalog(false,
						      std::forward<Arg>
						      (arg)...);
		}
	};

	//! Helper class determines presence of optional initial \c bool arg to special_columns()

	template<typename argType> class special_columnsFirstArg {
	public:

		//! Default implementation, \c bool initial parameter not given.

		//! Default it to \c false.

		template<typename ...Arg>
		static inline statement special_columns(connectionObj &obj,
						   Arg && ...arg)
		{
			return obj.special_columns_catalog(false,
						      std::forward<Arg>
						      (arg)...);
		}
	};

	//! Helper class determines presence of optional initial \c bool arg to statistics()

	template<typename argType> class statisticsFirstArg {
	public:

		//! Default implementation, \c bool initial parameter not given.

		//! Default it to \c false.

		template<typename ...Arg>
		static inline statement statistics(connectionObj &obj,
						   Arg && ...arg)
		{
			return obj.statistics_catalog(false,
						      std::forward<Arg>
						      (arg)...);
		}
	};
public:
	//! Enumerate available tables

	statement tables();

	//! Enumerate available tables

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

	//! Enumerate table privileges

	//! \overload
	template<typename firstArg, typename ...Args>
	statement table_privileges(firstArg &&arg,
				   Args && ...args)
	{
		return table_privilegesFirstArg<typename std::remove_cv<
			typename std::remove_reference
					      <firstArg>::type>::type>
			::table_privileges(*this,
				 std::forward<firstArg>(arg),
				 std::forward<Args>(args)...);
	}

	//! Enumerate available columns
	statement columns();

	//! Enumerate available columns

	//! \overload
	template<typename firstArg, typename ...Args>
	statement columns(firstArg &&arg,
			  Args && ...args)
	{
		return columnsFirstArg<typename std::remove_cv<
			typename std::remove_reference
					       <firstArg>::type>::type>
			::columns(*this,
				  std::forward<firstArg>(arg),
				  std::forward<Args>(args)...);
	}

	//! Enumerate available column_privileges
	statement column_privileges();

	//! Enumerate available column_privileges

	//! \overload
	template<typename firstArg, typename ...Args>
	statement column_privileges(firstArg &&arg,
				    Args && ...args)
	{
		return column_privilegesFirstArg
			<typename std::remove_cv<
				typename std::remove_reference
				 <firstArg>::type>::type>
			::column_privileges(*this,
					    std::forward<firstArg>(arg),
					    std::forward<Args>(args)...);
	}

	//! Enumerate available foreign_keys
	statement foreign_keys();

	//! Enumerate available foreign_keys

	//! \overload
	template<typename firstArg, typename ...Args>
	statement foreign_keys(firstArg &&arg,
			       Args && ...args)
	{
		return foreign_keysFirstArg
			<typename std::remove_cv<
				typename std::remove_reference
				 <firstArg>::type>::type>
			::foreign_keys(*this,
				       std::forward<firstArg>(arg),
				       std::forward<Args>(args)...);
	}

	//! Enumerate available primary_keys

	//! \overload
	template<typename firstArg, typename ...Args>
	statement primary_keys(firstArg &&arg,
			       Args && ...args)
	{
		return primary_keysFirstArg
			<typename std::remove_cv<
				typename std::remove_reference
				 <firstArg>::type>::type>
			::primary_keys(*this,
				       std::forward<firstArg>(arg),
				       std::forward<Args>(args)...);
	}

	//! Enumerate available procedure_columns
	statement procedure_columns();

	//! Enumerate available procedure_columns

	//! \overload
	template<typename firstArg, typename ...Args>
	statement procedure_columns(firstArg &&arg,
				    Args && ...args)
	{
		return procedure_columnsFirstArg
			<typename std::remove_cv<
				typename std::remove_reference
				 <firstArg>::type>::type>
			::procedure_columns(*this,
					    std::forward<firstArg>(arg),
					    std::forward<Args>(args)...);
	}

	//! Enumerate available procedures
	statement procedures();

	//! Enumerate available procedures

	//! \overload
	template<typename firstArg, typename ...Args>
	statement procedures(firstArg &&arg,
			     Args && ...args)
	{
		return proceduresFirstArg
			<typename std::remove_cv<
				typename std::remove_reference
				 <firstArg>::type>::type>
			::procedures(*this,
				     std::forward<firstArg>(arg),
				     std::forward<Args>(args)...);
	}

	//! Enumerate available special_columns

	template<typename firstArg, typename ...Args>
	statement special_columns(firstArg &&arg,
				  Args && ...args)
	{
		return special_columnsFirstArg
			<typename std::remove_cv<
				typename std::remove_reference
				 <firstArg>::type>::type>
			::special_columns(*this,
					  std::forward<firstArg>(arg),
					  std::forward<Args>(args)...);
	}

	//! Enumerate available statistics

	template<typename firstArg, typename ...Args>
	statement statistics(firstArg &&arg,
				  Args && ...args)
	{
		return statisticsFirstArg
			<typename std::remove_cv<
				typename std::remove_reference
				 <firstArg>::type>::type>
			::statistics(*this,
				     std::forward<firstArg>(arg),
				     std::forward<Args>(args)...);
	}

	//! Enumerate available types
	virtual statement type_info()=0;

	//! Enumerate available types
	virtual statement type_info(//! One of the SQL types, like "varchar"
				    const std::string &sqltype)=0;

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

	//! Execute SQL statement directly
	virtual statement execute_directly(const std::string_view &sql)=0;

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

	//! Return native SQL string

	virtual std::string native_sql(const std::string &sql)=0;

	//! Return currently-installed DBI flavor implementation
	dbi::flavor flavor();

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

	//! Retrieve table privileges

	virtual statement table_privileges_catalog(bool literal_ids,
						   const std::string &table,
						   const std::string &catalog="",
						   const std::string &schema="")=0;

	//! Retrieve column catalogs

	virtual statement columns_catalog(bool literal_ids,
					  const std::string &catalog="",
					  const std::string &schema="",
					  const std::string &table="",
					  const std::string &columnname="")=0;

	//! Retrieve column privileges catalogs

	virtual statement column_privileges_catalog(bool literal_ids,
						    const std::string &catalog="",
						    const std::string &schema="",
						    const std::string &table="",
						    const std::string &columnname="")=0;

	//! Retrieve primary keys catalog
	virtual statement primary_keys_catalog(bool literal_ids,
					       const std::string &table,
					       const std::string &catalog="",
					       const std::string &schema="")=0;

	//! Retrieve foreign keys catalog
	virtual statement foreign_keys_catalog(bool literal_ids,
					       const std::string &pk_catalog="",
					       const std::string &pk_schema="",
					       const std::string &pk_table="",
					       const std::string &fk_catalog="",
					       const std::string &fk_schema="",
					       const std::string &fk_table="")
	=0;

	//! Retrieve procedure columns catalog
	virtual statement procedure_columns_catalog(bool literal_ids,
						    const std::string &catalog="",
						    const std::string &schema="",
						    const std::string &proc="",
						    const std::string &column=""
						    )=0;

	//! Retrieve procedure columns catalog
	virtual statement procedures_catalog(bool literal_ids,
					     const std::string &catalog="",
					     const std::string &schema="",
					     const std::string &proc=""
					     )=0;

	//! Retrieve special columns catalog
	virtual statement special_columns_catalog(bool literal_ids,
						  rowid_t rowid,
						  scope_t scope,
						  const std::string &table,
						  bool nullable=true,
						  const std::string &catalog="",
						  const std::string &schema=""
						  )=0;

	//! Retrieve special columns catalog
	virtual statement statistics_catalog(bool literal_ids,
					     const std::string &table,
					     bool unique_only=false,
					     bool force=false,
					     const std::string &catalog="",
					     const std::string &schema=""
					     )=0;
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

//! Specialization: there's an initial initial \c bool arg to table_privileges()

template<> class connectionObj::table_privilegesFirstArg<bool> {
public:

	//! Forward the initial bool parameter value.

	template<typename ...Arg>
	static inline statement table_privileges(connectionObj &obj,
						 bool flag,
						 Arg && ...arg)
	{
		return obj.table_privileges_catalog(flag, std::forward<Arg>(arg)...);
	}
};

//! Specialization: there's an initial initial \c bool arg to columns()

template<> class connectionObj::columnsFirstArg<bool> {
public:

	//! Forward the initial bool parameter value.

	template<typename ...Arg>
	static inline statement columns(connectionObj &obj,
					bool flag,
					Arg && ...arg)
	{
		return obj.columns_catalog(flag,
					   std::forward<Arg>(arg)...);
	}
};

//! Specialization: there's an initial \c bool arg to column_privileges()

template<> class connectionObj::column_privilegesFirstArg<bool> {
public:

	//! Forward the initial bool parameter value.

	template<typename ...Arg>
	static inline statement column_privileges(connectionObj &obj,
						  bool flag,
						  Arg && ...arg)
	{
		return obj.column_privileges_catalog(flag,
						     std::forward<Arg>
						     (arg)...);
	}
};

//! Specialization: there's an initial \c bool arg to foreign_keys()

template<> class connectionObj::foreign_keysFirstArg<bool> {
public:

	//! Forward the initial bool parameter value.

	template<typename ...Arg>
	static inline statement foreign_keys(connectionObj &obj,
					     bool flag,
					     Arg && ...arg)
	{
		return obj.foreign_keys_catalog(flag,
						std::forward<Arg>
						(arg)...);
	}
};

//! Specialization: there's an initial \c bool arg to primary_keys()

template<> class connectionObj::primary_keysFirstArg<bool> {
public:

	//! Forward the initial bool parameter value.

	template<typename ...Arg>
	static inline statement primary_keys(connectionObj &obj,
					     bool flag,
					     Arg && ...arg)
	{
		return obj.primary_keys_catalog(flag,
						std::forward<Arg>
						(arg)...);
	}
};

//! Specialization: there's an initial \c bool arg to procedure_columns()

template<> class connectionObj::procedure_columnsFirstArg<bool> {
public:

	//! Forward the initial bool parameter value.

	template<typename ...Arg>
	static inline statement procedure_columns(connectionObj &obj,
						  bool flag,
						  Arg && ...arg)
	{
		return obj.procedure_columns_catalog(flag,
						     std::forward<Arg>
						     (arg)...);
	}
};

//! Specialization: there's an initial \c bool arg to procedures()

template<> class connectionObj::proceduresFirstArg<bool> {
public:

	//! Forward the initial bool parameter value.

	template<typename ...Arg>
	static inline statement procedures(connectionObj &obj,
						  bool flag,
						  Arg && ...arg)
	{
		return obj.procedures_catalog(flag,
					      std::forward<Arg>(arg)...);
	}
};

//! Specialization: there's an initial \c bool arg to special_columns()

template<> class connectionObj::special_columnsFirstArg<bool> {
public:

	//! Forward the initial bool parameter value.

	template<typename ...Arg>
	static inline statement special_columns(connectionObj &obj,
						  bool flag,
						  Arg && ...arg)
	{
		return obj.special_columns_catalog(flag,
						   std::forward<Arg>(arg)...);
	}
};

//! Specialization: there's an initial \c bool arg to statistics()

template<> class connectionObj::statisticsFirstArg<bool> {
public:

	//! Forward the initial bool parameter value.

	template<typename ...Arg>
	static inline statement statistics(connectionObj &obj,
					   bool flag,
					   Arg && ...arg)
	{
		return obj.statistics_catalog(flag,
					      std::forward<Arg>(arg)...);
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
