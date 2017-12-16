/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_dbi_resultset_H
#define x_sql_dbi_resultset_H

#include <x/sql/dbi/resultsetobj.H>
#include <x/sql/dbi/resultsetfwd.H>
#include <x/sql/dbi/constraint.H>
#include <x/sql/statement.H>

namespace LIBCXX_NAMESPACE {
	namespace sql {
		namespace dbi {
#if 0
		};
	};
};
#endif

//! Base class for \ref resultset "SQL resultsets".

class resultsetBase : public ptrref_base {
};

template<typename ...Args>
inline void resultsetObj::search(Args && ...args)
{
	where->add(std::forward<Args>(args)...);
}

template<typename ...Args>
inline void resultsetObj::having(Args && ...args)
{
	having_constraint->add(std::forward<Args>(args)...);
}

//! Process insert() and update() variadic parameters

template<typename valueType, typename ...Args>
inline void get_updins_list(const ref<constraintObj::andObj> &set,
			    const std::string &fieldname,
			    valueType && fieldvalue,
			    Args && ...args)
{
	set->add(fieldname, "=", std::forward<valueType>(fieldvalue));
	return get_updins_list(set, std::forward<Args>(args)...);
}

//! Process insert() and update() variadic parameters

template<typename valueType, typename ...Args>
inline void get_updins_list(const ref<constraintObj::andObj> &set,
			    const std::map<std::string, valueType> &values,
			    Args && ...args)
{
	for (const auto &value:values)
		set->add(value.first, "=", value.second);
	return get_updins_list(set, std::forward<Args>(args)...);
}

//! Process insert() and update() variadic parameters
template<typename ...Args>
inline void get_updins_list(const ref<constraintObj::andObj> &set,
			    const ref<constraintObj::andObj> &values,
			    Args && ...args)
{
	// Used to implement row UPDATEs.
	set->add(constraint(values));
	return get_updins_list(set, std::forward<Args>(args)...);
}

//! End of parameters for insert() and update()
inline void get_updins_list(const ref<constraintObj::andObj> &set)
{
}

template<typename ...Args>
inline size_t resultsetObj::update(Args && ...args) const
{
	auto set=ref<constraintObj::andObj>::create();

	get_updins_list(set, std::forward<Args>(args)...);
	return do_update(set);
}

#if 0
{
#endif
};

template<typename ArgType>
void bindrow::consecutive::bind(ArgType &&arg)
{
	factory.bind(column, std::forward<ArgType>(arg));
	++column;
}

#if 0
{
	{
#endif
	};
};
#endif
