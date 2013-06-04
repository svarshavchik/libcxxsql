/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_dbi_constraint_H
#define x_sql_dbi_constraint_H

#include <x/sql/dbi/constraintobj.H>
#include <x/sql/dbi/constraintfwd.H>
#include <x/sql/statement.H>
#include <x/ref.H>
#include <x/vector.H>
#include <type_traits>
#include <utility>

namespace LIBCXX_NAMESPACE {
	namespace sql {
#if 0
	};
};
#endif

//! Process an execute parameter factory

template<typename ...OtherArgs>
void statementObj::process_execute_non_null_param<dbi::constraint>
::process(statementObj &s,
	  size_t param_number,
	  const dbi::constraint *parameters,
	  OtherArgs && ...otherArgs)
{
	execute_factory factory(s, param_number);

	(*parameters)->get_parameters(factory);

	// Onto the next parameter.

	s.process_execute_params(factory.param_number,
				 std::forward<OtherArgs>(otherArgs)...);
}

namespace dbi {
#if 0
}
#endif

//! Base class for SQL \ref dbi_constraint "constraints".

//! The base class supplies a custom create() method.

class constraintBase {

public:

	//! The create() method's object factory.

	template<typename ref_type> class objfactory {

	public:

		//! The custom create() method for \ref dbi_constraint "constraints"

		//! The custom create() method instantiates an AND constraint.

		template<typename ...Args>
		static ref_type create(//! Arguments forwarded to the AND constraint
				       Args && ...args)
		{
			return ref<constraintObj::andObj>
				::create(std::forward<Args>(args)...);
		}
	};
};

template<typename ...Args>
constraintObj::containerObj::containerObj(Args && ...args)
{
	this->add(std::forward<Args>(args)...);
}

//! Helper class for parsing value of a constraint

//! The default implementation instantiates a \ref cmpObj "cmpObj" object
//! to implement the individual comparison constraint.
//!
//! The template class gets specialized for implementing a value given as
//! a std::vector, which creates an IN or a NOT IN clause.

template<typename cmpTypeObj> class constraintObj::containerObj::add_value {

public:

	//! Default template implementation, instantiate cmpObj.

	template<typename ValueType>
	static void add(constraintObj::containerObj &self,
			const std::string &nameArg,
			const std::string &cmptypeArg,
			ValueType &&value)
	{
		self.push_back(ref<cmpObj<cmpTypeObj>>
			       ::create(nameArg, cmptypeArg,
					std::forward<ValueType>
					(value)));
	}
};

//! Specialization

//! Specialized implementation for when a std::vector is passed for a constraint
//! value. This results in constructing IN or NOT IN SQL.

//! Specialization

//! Specialized implementation for when a std::vector is passed for a constraint
//! value. This results in constructing IN or NOT IN SQL.

template<typename vectorValueObj>
class constraintObj::containerObj::add_value<std::vector<vectorValueObj>> {

public:

	//! Specialized implementation

	//! Use cmpvecObj to implement this constraint.
	template<typename ValueType>
	static void add(constraintObj::containerObj &self,
			const std::string &nameArg,
			const std::string &cmptypeArg,
			ValueType &&value)
	{
		self.push_back(ref<cmpvecObj<vectorValueObj>>
			       ::create(nameArg, cmptypeArg,
					std::forward<ValueType>(value)));
	}
};

//! Specialization

//! Specialized implementation for when a reference-counted vector gets passed
//! as a constraint value. This results in constructing IN or NOT IN SQL.

template<typename vectorValueObj>
class constraintObj::containerObj::add_value<vector<vectorValueObj>> {

public:

	//! Specialized implementation

	//! Use cmpvecObj to implement this constraint.
	template<typename ValueType>
	static void add(constraintObj::containerObj &self,
			const std::string &nameArg,
			const std::string &cmptypeArg,
			ValueType &&value)
	{
		self.push_back(ref<cmpvecObj<vectorValueObj>>
			       ::create(nameArg, cmptypeArg,
					const_vector<vectorValueObj>
					(std::forward<ValueType>(value))));
	}
};

//! Specialization

//! Specialized implementation for when a reference-counted vector gets passed
//! as a constraint value. This results in constructing IN or NOT IN SQL.

template<typename vectorValueObj>
class constraintObj::containerObj::add_value<const_vector<vectorValueObj>> {

public:

	//! Specialized implementation

	//! Use cmpvecObj to implement this constraint.
	template<typename ValueType>
	static void add(constraintObj::containerObj &self,
			const std::string &nameArg,
			const std::string &cmptypeArg,
			ValueType &&value)
	{
		self.push_back(ref<cmpvecObj<vectorValueObj>>
			       ::create(nameArg, cmptypeArg,
					std::forward<ValueType>(value)));
	}
};

//! Specialization

//! Specialized implementation for when a std::pair<value, bitflag> gets passed
//! as a constraint value.

template<typename cmpTypeObj>
class constraintObj::containerObj::add_value<std::pair<cmpTypeObj, bitflag>> {

public:

	//! Default template implementation, instantiate cmpObj.

	template<typename ValueType>
	static void add(constraintObj::containerObj &self,
			const std::string &nameArg,
			const std::string &cmptypeArg,
			ValueType &&value)
	{
		if (value.second)
			self.push_back(ref<cmpObj<std::nullptr_t>>
				       ::create(nameArg, cmptypeArg, nullptr));
		else
			self.push_back(ref<cmpObj<cmpTypeObj>>
				       ::create(nameArg, cmptypeArg,
						value.first));
	}
};

template<typename cmpType, typename ...Args>
void constraintObj::containerObj::add(const std::string &nameArg,
				      const std::string &cmptypeArg,
				      cmpType && value,
				      Args && ...args)
{
	add_value<typename std::decay<cmpType>::type>
		::add(*this, nameArg, cmptypeArg,
		      std::forward<cmpType>(value));

	add(std::forward<Args>(args)...);
}

template<typename ...Args>
void constraintObj::containerObj::add(const const_constraint &import_constraint,
				      Args && ...args)
{
	this->push_back(import_constraint);
	add(std::forward<Args>(args)...);
}

template<typename ...Args>
void constraintObj::containerObj::add(const std::vector<constraint> &constraints,
				      Args && ...args)
{
	this->insert(this->end(), constraints.begin(),
		     constraints.end());
	add(std::forward<Args>(args)...);
}

template<typename ...Args>
void constraintObj::containerObj::add(const std::vector<const_constraint> &constraints,
				      Args && ...args)
{
	this->insert(this->end(), constraints.begin(),
		     constraints.end());
	add(std::forward<Args>(args)...);
}

template<typename T>
constraintObj::cmpvecObj<T>::~cmpvecObj() noexcept
{
}

template<typename T>
void constraintObj::cmpvecObj<T>
::get_parameters(statementObj::execute_factory &factory) const
{
	for (const auto &v:*values)
	{
		factory.parameter(v);
	}
}

//! Helper class for initializing the container in a list comparison constraint.

//! \internal
//! The value argument to a list constraint is either an
//! \ref vector "INSERT_LIBX_NAMESPACE::vector", or a \c std::vector.
//!
//! This helper class defines an init() method.
//!
//! The \c std::vector initializer gets taken care of by a specialization,
//! which uses create() to construct the
//! \ref vector "INSERT_LIBX_NAMESPACE::vector", and this defaule template
//! class expects a
//! \ref vector "INSERT_LIBX_NAMESPACE::vector" or a
//! \ref const_vector "INSERT_LIBX_NAMESPACE::const_vector", and defines an
//! init() method that returns a \c vector.

template<typename initType> class init_vectorObj {

public:

	//! Default init() method that expects the list value to already be a \ref vector "INSERT_LIBX_NAMESPACE::vector".

	template<typename T, typename ArgType>
	static inline const_vector<T> init(ArgType && arg)
	{
		return const_vector<T>(std::forward<ArgType>(arg));
	}
};

//! Specialization for list constraint value given in a \c std::vector

//! The specialization's init() converts it to a
//! \ref vector "INSERT_LIBX_NAMESPACE::vector".

template<typename valueType, typename allocatorType>
class init_vectorObj<std::vector<valueType, allocatorType>> {
public:

	//! Call \ref vector "INSERT_LIBX_NAMESPACE::vector<>::create" to convert the \c std::list to a vector.

	//! The wrapper that calls this always passes a single parameter,
	//! but we'll use variadic parameters in order to allow future
	//! flexibility.

	template<typename T, typename ...ArgsType>
	static inline vector<T> init(ArgsType && ...arg)
	{
		return vector<T>::create(std::forward<ArgsType>(arg)...);
	}
};


template<typename T>
template<typename vecType>
constraintObj::cmpvecObj<T>::cmpvecObj(const std::string &nameArg,
				       const std::string &cmptypeArg,
				       vecType && valueArg)
	: name(nameArg),
	  cmptype(this->get_vec_cmptype(cmptypeArg)),
	  values(init_vectorObj<typename std::decay<vecType>::type>
		 ::template init<T>(std::forward<vecType>(valueArg)))
{
}

//! Helper method for creating an \c AND \ref constraint "constraint".

template<typename ...Args>
static constraint AND(Args && ...args)
{
	return constraint::create(std::forward<Args>(args)...);
}

//! Helper method for creating an \c OR \ref constraint "constraint".

template<typename ...Args>
static constraint OR(Args && ...args)
{
	return ref<constraintObj::orObj>::create(std::forward<Args>(args)...);
}

//! Helper method for creating a \c NOT \ref constraint "constraint".

template<typename ...Args>
static constraint NOT(Args && ...args)
{
	return ref<constraintObj::notObj>::create(std::forward<Args>(args)...);
}

//! Overload << to invoke get_sql().

inline std::ostream &operator<<(std::ostream &o, const constraint &cons)
{
	cons->get_sql(o);
	return o;
}

#if 0
{
	{
		{
#endif
		};
	};
};
#endif
