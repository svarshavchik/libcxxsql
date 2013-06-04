/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_dbi_constraintobj_H
#define x_sql_dbi_constraintobj_H

#include <x/obj.H>
#include <x/vectorfwd.H>
#include <x/vectorobj.H>
#include <x/sql/dbi/constraintfwd.H>
#include <x/sql/statementobj.H>

#include <iostream>

namespace LIBCXX_NAMESPACE {
	namespace sql {

		//! SQL database interface factory.

		namespace dbi {
#if 0
		};
	};
};
#endif

//! Base class for DBI constraints

class constraintObj : virtual public obj {

public:
	//! Constructor
	constraintObj();

	//! Destructor
	~constraintObj() noexcept;

	//! Retrieve SQL for the constraint
	virtual void get_sql(std::ostream &) const=0;

	//! Enumerate parameters to \ref statementObj::execute "statement->execute()"

	virtual void get_parameters(statementObj::execute_factory &factory)
		const =0;

	template<typename T> class cmpObj;
	template<typename T> class cmpvecObj;

	class containerObj;
	class andObj;
	class orObj;
	class notObj;

protected:

	//! Generate SQL for a comparison operation.

	//! Used by cmpObj.

	static void get_sql_impl(std::ostream &o,
				 const std::string &name,
				 const std::string &cmptype);

	//! Verify that the constraint is "=" or "!=".

	//! This is used when the constraint value is a vector, which
	//! translates "=" and "!=" into an IN or a NOT IN SQL (with some
	//! finesse).

	static std::string get_vec_cmptype(const std::string &cmptypeArg);

	//! Construct an SQL that implements "=" or "!=" of a vector using IN or NOT IN.

	static void get_vec_sql_impl(std::ostream &o,
				     const std::string &name,
				     const std::string &cmptype,
				     size_t nvalues);

};

//! A constraint that compares a field against a value.

template<typename T>
class constraintObj::cmpObj : public constraintObj {

	//! Field name
	std::string name;

	//! Comparison
	std::string cmptype;

	//! Value
	T value;
public:
	//! Constructor
	cmpObj(const std::string &nameArg,
	       const std::string &cmptypeArg,
	       const T &valueArg) : name(nameArg),
				 cmptype(cmptypeArg),
				 value(valueArg)
	{
	}

	//! Move constructor
	cmpObj(const std::string &nameArg,
	       const std::string &cmptypeArg,
	       T &&valueArg) : name(nameArg),
			    cmptype(cmptypeArg),
			    value(std::move(valueArg))
	{
	}

	//! Destructor
	~cmpObj() noexcept
	{
	}

	//! Retrieve SQL for the constraint
	void get_sql(std::ostream &o) const override
	{
		get_sql_impl(o, name, cmptype);
	}

	//! Enumerate parameters to \ref statementObj::execute "statement->execute()"

	void get_parameters(statementObj::execute_factory &factory)
		const override
	{
		get_parameters_impl(factory);
	}

private:

	//! Enumerate non-NULL parameters to \ref statementObj::execute "statement->execute()"

	void get_parameters_impl(statementObj::execute_factory &factory)
		const
	{
		factory.parameter(value);
	}
};


//! A constraint that compares a field against a value.

template<typename T>
class constraintObj::cmpvecObj : public constraintObj {

	//! Field name
	std::string name;

	//! Comparison
	std::string cmptype;

	//! Value
	const_vector<T> values;
public:
	//! Constructor
	template<typename vecType>
	cmpvecObj(const std::string &nameArg,
		  const std::string &cmptypeArg,
		  vecType && valueArg);

	//! Destructor
	~cmpvecObj() noexcept;

	//! Retrieve SQL for the constraint

	//! If the constraint operator is "=", this constructs a
	//! "<name> IN (?, ?...)" SQL.
	//! If the constraint operator is "!=", this constructs a SQL
	//! "<name> NOT IN (?, ?...)" SQL.
	//!
	//! When the list of values is empty:
	//!
	//! - The "=" constraint operator forms the "1=0" SQL,
	//! which is always false.
	//!
	//! - The "!=" constraint operator forms the "<name> IS NOT NULL"
	//! SQL, which is true for constraint values that are not NULL.

	void get_sql(std::ostream &o) const override
	{
		this->get_vec_sql_impl(o, name, cmptype, values->size());
	}

	//! Enumerate parameters to \ref statementObj::execute "statement->execute()"

	void get_parameters(statementObj::execute_factory &factory)
		const override;
};

//! Specialization for a NULL value comparison

template<>
class constraintObj::cmpObj<std::nullptr_t> : public constraintObj {

	//! Constraint text

	std::string txt;
public:
	//! Constructor
	cmpObj(const std::string &nameArg,
	       const std::string &cmptypeArg,
	       std::nullptr_t valueArg);

	//! Destructor
	~cmpObj() noexcept;

	//! Construct SQL for this constraint.

	//! When the comparison operator is "=", this forms "<name> IS NULL"
	//! SQL.
	//! When the comparison operator is "!=", this forms "<name> IS NOT NULL"
	//! SQL.
	//! For all other comparison operators, his forms "1=0" SQL, which
	//! is always false.
	void get_sql(std::ostream &o) const override;

	//! No parameters here, does nothing.

	void get_parameters(statementObj::execute_factory &factory)
		const override;
};

//! A container for AND and OR constraints

//! Superclass of \ref andObj "AND" and \ref orObj "OR" constraint objects.

class constraintObj::containerObj
	: public vectorObj<const_constraint>, public constraintObj {

public:

	//! Constructor
	template<typename ...Args>
	containerObj(//! List of constraints combined into an AND or OR clause
		     Args && ...args);

	//! Destructor
	~containerObj() noexcept;

	//! No more parameter values to add().
	void add()
	{
	}

	//! Variadic function, invoked by the constructor, that parses constraints

	template<typename cmpType, typename ...Args>
	void add(//! Name of variable
		 const std::string &nameArg,

		 //! Comparison operator, like "=" or "!="
		 const std::string &cmpArg,

		 //! Comparison value
		 cmpType && value,

		 //! Other variadic parameters.
		 Args && ...args);

	//! Variadic function, invoked by the constructor, that parses constraints

	template<typename ...Args>
	void add(//! Another constraint
		 const const_constraint &import_constraint,

		 //! Other variadic parameters.
		 Args && ...args);

	//! Variadic function, invoked by the constructor, that parses constraints

	template<typename ...Args>
	void add(//! Another constraint
		 const std::vector<constraint> &constraints,

		 //! Other variadic parameters.
		 Args && ...args);

	//! Variadic function, invoked by the constructor, that parses constraints

	template<typename ...Args>
	void add(//! Another constraint
		 const std::vector<const_constraint> &constraints,

		 //! Other variadic parameters.
		 Args && ...args);

	//! Enumerate parameters in the SQL formed by this clause

	//! Calls get_parameters() on each constraint.
	void get_parameters(statementObj::execute_factory &factory)
		const override;

private:

	template<typename value_type> class add_value;
};

//! An AND constraint

//! The arguments to the AND constraint is a list of subconstraints, which
//! get combined using "AND" clauses.

class constraintObj::andObj : public containerObj {

public:
	//! Construct an AND constraint

	template<typename ...Args>
	andObj(//! Subconstraints
	       Args && ...args)
		: containerObj(std::forward<Args>(args)...)
	{
	}

	//! Destructor
	~andObj() noexcept;

	//! Create "AND" SQL constraint.

	//! Invokes get_sql() on each constraint that was passed to the
	//! constructor. Catenates them together using AND clauses, and puts
	//! everything inside parenthesis. If the list of containers that was
	//! passed to the constructor is empty, this generates "1=1" which
	//! evaluates to true.
	//!
	//! The WHERE clause is a top-level AND constraint, so the empty
	//! AND constraint evalues to a no-op WHERE clause.

	void get_sql(std::ostream &o) const override;
};

//! An OR constraint

//! The arguments to the OR constraint is a list of subconstraints, which
//! get combined using "OR" clauses.

class constraintObj::orObj : public containerObj {

public:
	//! Construct an OR constraint
	template<typename ...Args>
	orObj(//! Subconstraints
	      Args && ...args)
		: containerObj(std::forward<Args>(args)...)
	{
	}

	//! Destructor
	~orObj() noexcept;

	//! Create "OR" SQL constraint.

	//! Invokes get_sql() on each constraint that was passed to the
	//! constructor. Catenates them together using AND clauses, and puts
	//! everything inside parenthesis. If the list of containers that was
	//! passed to the constructor is empty, this generates "1=0" which
	//! evaluates to false.

	void get_sql(std::ostream &o) const override;
};

//! A NOT constraint

//! The arguments to the NOT constraint is a list of subconstraints, which
//! get combined using "AND" clauses, with the whole thing wrapped inside a
//! NOT

class constraintObj::notObj : public constraintObj::andObj {

public:
	//! Construct an AND constraint

	template<typename ...Args>
	notObj(//! Subconstraints
	       Args && ...args)
		: andObj(std::forward<Args>(args)...)
	{
	}

	//! Destructor
	~notObj() noexcept;

	//! Create a "NOT" SQL constraint.

	//! Invokes parent superclass's get_sql(), wrapping it inside a
	//! NOT clause.

	void get_sql(std::ostream &o) const override;
};

#if 0
{
	{
		{
#endif
		};
	};
};
#endif
