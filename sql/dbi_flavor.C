/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/sql/dbi/flavor.H"
#include "x/sql/dbi/constraint.H"
#include "x/sql/dbi/resultset.H"
#include "x/sql/connection.H"
#include "gettext_in.h"

namespace LIBCXX_NAMESPACE {
	namespace sql {
		namespace dbi {
#if 0
		}
	}
};
#endif

flavorObj::flavorObj()
{
}

flavorObj::~flavorObj() noexcept
{
}


/////////////////////////////////////////////////////////////////////////////

class LIBCXX_HIDDEN mysqlFlavorObj : public flavorObj {

 public:
	mysqlFlavorObj();

	//! Destructor
	~mysqlFlavorObj() noexcept;

	void update_with_joins(std::ostream &o,
			       const resultsetObj &rs,
			       std::vector<std::string> &fields,
			       std::vector<std::string> &placeholders)
		const override;

	const char *datatype_serial() override;
	const char *default_value_serial() override;
	constraint get_inserted_serial(const char *table_name,
				       const std::set<std::string> &columns)
		override;
};

mysqlFlavorObj::mysqlFlavorObj()
{
}

mysqlFlavorObj::~mysqlFlavorObj() noexcept
{
}

void mysqlFlavorObj::update_with_joins(std::ostream &o,
				       const resultsetObj &rs,
				       std::vector<std::string> &fields,
				       std::vector<std::string> &placeholders)
		const
{
	o << "UPDATE " << rs.get_table_name() << " AS " << rs.get_table_alias();
	resultsetObj::get_join_sql(o, rs.joinlist);
	resultsetObj::add_update_set(o, fields, placeholders);
	rs.add_where(o);
}

const char *mysqlFlavorObj::datatype_serial()
{
	return "BIGINT AUTO_INCREMENT";
}

const char *mysqlFlavorObj::default_value_serial()
{
	return "NULL";
}

constraint mysqlFlavorObj
::get_inserted_serial(const char *table_name,
		      const std::set<std::string> &columns)
{
	if (columns.size() > 1)
		throw EXCEPTION(_TXT(_txt("Only one serial column can exist in a table")));

	return constraint::create(*columns.begin(), text("="),
				  "LAST_INSERT_ID()");
}

/////////////////////////////////////////////////////////////////////////////

class LIBCXX_HIDDEN postgresFlavorObj : public flavorObj {

 public:
	postgresFlavorObj();

	//! Destructor
	~postgresFlavorObj() noexcept;

	void update_with_joins(std::ostream &o,
			       const resultsetObj &rs,
			       std::vector<std::string> &fields,
			       std::vector<std::string> &placeholders)
		const override;

	const char *datatype_serial() override;
	const char *default_value_serial() override;
	constraint get_inserted_serial(const char *table_name,
				       const std::set<std::string> &columns)
		override;
};

postgresFlavorObj::postgresFlavorObj()
{
}

postgresFlavorObj::~postgresFlavorObj() noexcept
{
}

void postgresFlavorObj::update_with_joins(std::ostream &o,
					  const resultsetObj &rs,
					  std::vector<std::string> &fields,
					  std::vector<std::string> &placeholders)
		const
{
	o << "UPDATE " << rs.get_table_name() << " AS updated_table";

	std::string alias=rs.get_table_alias();

	resultsetObj::remove_prefix(fields, alias);
	resultsetObj::add_update_set(o, fields, placeholders);

	o << " FROM (";

	std::vector<std::string> primary_key_columns;

	alias.push_back('.');

	for (auto p=rs.get_primary_key_columns(); *p; ++p)
		primary_key_columns.push_back(alias + *p);

	rs.get_select_sql(o, primary_key_columns);
	o << ") AS joins WHERE ";

	const char *pfix="";

	for (auto p=rs.get_primary_key_columns(); *p; ++p)
	{
		o << pfix << "updated_table." << *p << "="
		  << "joins." << *p;
		pfix=" AND ";
	}

	if (!*pfix)
		throw EXCEPTION(_TXT(_txt("Primary key column required for an UPDATE with joins")));
}


const char *postgresFlavorObj::datatype_serial()
{
	return "BIGSERIAL";
}

const char *postgresFlavorObj::default_value_serial()
{
	return "DEFAULT";
}

constraint postgresFlavorObj::get_inserted_serial(const char *table_name,
						  const std::set<std::string> &columns)
{
	auto c=ref<constraintObj::andObj>::create();

	for (const auto &column:columns)
	{
		c->add(column, text("="),
		       std::list<std::string>({
				       "currval(pg_get_serial_sequence(?, ?))",
					       table_name,
					       column
					       }));
	}
	return c;
}

#if 0
{
#endif
};

dbi::flavor connectionObj::flavor()
{
	decltype(dbiflavor)::lock lock(dbiflavor);

	if (lock->null())
	{
		bool mysql=false;

		try {
			execute("select @@version");
			mysql=true;
		} catch(...)
		{
		}

		if (mysql)
		{
			*lock=ref<dbi::mysqlFlavorObj>::create();
		}
		else
		{
			*lock=ref<dbi::postgresFlavorObj>::create();
		}
	}

	return *lock;
}


#if 0
{
	{
#endif
	};
};
