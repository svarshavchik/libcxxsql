#ifndef sql_internal_H
#define sql_internal_H

#include "x/sql/env.H"
#include "x/sql/exception.H"
#include "x/sql/exceptionobj.H"
#include "x/sql/connection.H"
#include "x/sql/statement.H"
#include "x/logger.H"
#include <sql.h>
#include <sqlext.h>
#include <list>
#include <string>

namespace LIBCXX_NAMESPACE {
	namespace sql {
#if 0
	}
};
#endif

inline SQLCHAR *to_sqlcharptr(const std::string &s) LIBCXX_HIDDEN;

inline SQLCHAR *to_sqlcharptr(const std::string &s)
{
	return reinterpret_cast<SQLCHAR *>(const_cast<char *>(s.c_str()));
}

inline SQLCHAR *to_sqlcharptr_null(const std::string &s) LIBCXX_HIDDEN;

inline SQLCHAR *to_sqlcharptr_null(const std::string &s)
{
	auto p=to_sqlcharptr(s);

	return *p ? p:nullptr;
}

// Map an SQL TYPE, such as SQL_CHAR to a name, such as "char"
const char *type_to_name(SQLLEN type) LIBCXX_HIDDEN;

// Map an string, such as "char" to SQL_CHAR. Case insensitive
SQLLEN name_to_type(const char *n) LIBCXX_HIDDEN;


struct LIBCXX_HIDDEN exceptionObj::base {

	const char *function;
	SQLHANDLE handle;
	SQLSMALLINT type;

	base(const char *functionArg, SQLHANDLE handleArg, SQLSMALLINT typeArg);
};

class LIBCXX_HIDDEN diagnostics {

 public:

	exception e;

	diagnostics(const char *function, SQLHANDLE handle, SQLSMALLINT type);
	diagnostics();

	~diagnostics() noexcept;
};

void sql_error(const char *function,
	       SQLHANDLE handle,
	       SQLSMALLINT type) LIBCXX_HIDDEN;

class LIBCXX_HIDDEN envimplObj : public envObj {

 public:
	SQLHENV h;

	envimplObj();
	~envimplObj() noexcept;

	void get_data_sources(std::map<std::string,
			      std::string> &sources) const override;

	void get_drivers(std::map<std::string, std::string>
			 &drivers) const override;

	void set_login_timeout(time_t t) override;
	void clear_login_timeout() override;

	bool login_timeout_set;
	time_t login_timeout;

	std::pair<connection, std::string>
		connect(const std::string &connection_parameters,
			connect_flags flags);
};

class LIBCXX_HIDDEN statementimplObj;
class LIBCXX_HIDDEN newstatementimplObj;

class LIBCXX_HIDDEN connectionimplObj : public connectionObj {

	LOG_CLASS_SCOPE;

 public:
	SQLHDBC h;
	bool connected;
	ref<envimplObj> env;

	friend class newstatementimplObj;

	connectionimplObj(ref<envimplObj> &&envArg);
	~connectionimplObj() noexcept;

	void ret(SQLRETURN ret, const char *func);

	diagnostics diags;

	void disconnect();
	void do_disconnect();

	statement tables_catalog(bool literal_ids,
				 const std::string &catalog,
				 const std::string &schema,
				 const std::string &table,
				 const std::string &type) override;
	statement prepare(const std::string &sql) override;

	newstatement create_newstatement() override;
	ref<newstatementimplObj> do_create_newstatement();

	template<typename ret_type>
		void getinfo(SQLUSMALLINT what, const char *what_str,
			     ret_type &ret);

#define LIBCXX_SQL_GETINFO_DECL(t,n) t n() override;
#define LIBCXX_SQL_GETINFO_IMPL(a,b,c,d)
#include "getinfo_internal.H"
#undef LIBCXX_SQL_GETINFO_DECL
#undef LIBCXX_SQL_GETINFO_IMPL

#define CONN_ATTR(n, type, value) \
	set_attribute_ ## type(n, "SQLSetConnectAttr(" #type ", " #value ")", value)

	void set_attribute_uint(SQLINTEGER, const char *, SQLUINTEGER);

};

class LIBCXX_HIDDEN newstatementimplObj : public newstatementObj {

 public:

	ref<connectionimplObj> conn;

	std::map<SQLINTEGER, std::pair<const char *, SQLULEN>> ulen_attributes;

#define STMT_ATTR(n, type, value) \
	set_attribute_ ## type(n, "SQLSetStmtAttr(" #type ", " #value ")", value)

	void set_attribute_ulen(SQLINTEGER, const char *, SQLULEN);

	newstatementimplObj(ref<connectionimplObj> &&connArg);
	~newstatementimplObj() noexcept;

	statement prepare(const std::string &sql) override;
	ref<statementimplObj> newstmt();

	void option(const std::string &name, const std::string &value)
		override;
};

class LIBCXX_HIDDEN statementimplObj : public statementObj {

	LOG_CLASS_SCOPE;

 public:
	SQLHSTMT h;
	ref<connectionimplObj> conn;

	statementimplObj(const ref<connectionimplObj> &connArg);
	~statementimplObj() noexcept;

	void ret(SQLRETURN ret, const char *func);

	size_t size() override;

	void set_attr_str(SQLINTEGER, const char *, const std::string &);
	void set_attr_uint(SQLINTEGER, const char *, SQLUINTEGER);
	void set_attr_ulen(SQLINTEGER, const char *, SQLULEN);
	void set_attr_ptr(SQLINTEGER, const char *, SQLPOINTER);

#define SET_ATTR(what, type, value)				\
	set_attr_ ## type (what, "SQLSetStmtAttr(" #what ")", value)

	void clear_binds(size_t row_array_sizeArg) override;
	void bind_all(const fetch_orientation &orientation) override;

	size_t bind_column_name(const std::string &name) override;

	void bind_bookmarks(bookmark *bookmarks) override;
	void bind_next(size_t column_number, bitflag *n, bitflag *nullflag) override;

	void bind_next(size_t column_number, short *n, bitflag *nullflag) override;

	void bind_next(size_t column_number, unsigned short *n, bitflag *nullflag) override;

	void bind_next(size_t column_number, int *n, bitflag *nullflag) override;

	void bind_next(size_t column_number, unsigned *n, bitflag *nullflag) override;

	void bind_next(size_t column_number, long *n, bitflag *nullflag) override;

	void bind_next(size_t column_number, unsigned long *n, bitflag *nullflag) override;

	void bind_next(size_t column_number, long long *n, bitflag *nullflag) override;

	void bind_next(size_t column_number, unsigned long long *n, bitflag *nullflag) override;

	void bind_next(size_t column_number, float *n, bitflag *nullflag) override;

	void bind_next(size_t column_number, double *n, bitflag *nullflag) override;

	void bind_next(size_t column_number, ymd *s, bitflag *nullflag) override;

	void bind_next(size_t column_number, hms *s, bitflag *nullflag) override;

	void bind_next(size_t column_number, std::string *s, bitflag *nullflag) override;

	size_t fetch_into() override;

	size_t row_array_size;
	int scroll_orientation;
	SQLLEN scroll_offset;
	bookmark scroll_bookmark;

	std::vector<SQLUSMALLINT> row_status_array;
	SQLUINTEGER num_rows_fetched;

	std::vector<column> columns;
	columnmap_t columnmap;
	bool have_columns;
	std::vector<parameter> parameters;
	bool have_parameters;

	const std::vector<column> &get_columns() override;

	const columnmap_t &get_columnmap() override;

	std::string colattribute_str(size_t i,
				     SQLUSMALLINT field_identifier,
				     const char *field_identifier_str,
				     const char *default_value);

	SQLLEN colattribute_n(size_t i,
			      SQLUSMALLINT field_identifier,
			      const char *field_identifier_str);

	// ODBC indicators are SQLINTEGERs, we return them as bools.
	// When binding a column, we add the following instance to
	// bound_indicator_list, and after fetching, we do the mappings.

	class bound_indicator {
	public:
		std::vector<SQLLEN> indicators;
		bitflag *indicator_bools;

		// Anything extra that gets bound here.

		class baseObj : virtual public obj {

		public:
			baseObj();
			~baseObj() noexcept;

			virtual void bind(const bound_indicator &indicators,
					  statementimplObj &statement);
		};

		ref<baseObj> data;

		// template for a baseObj with some arbitrary transformation

		template<typename sqltype,
			 typename nativetype,
			 typename convertClass>
		class bindTransformObj : public baseObj {

		public:
			std::vector<sqltype> sqlbuf;
			nativetype *nativetypeArray;

			bindTransformObj(nativetype *nativetypeArrayArg,
					 size_t row_array_size)
				: nativetypeArray(nativetypeArrayArg)
			{
				sqlbuf.resize(row_array_size);
			}

			~bindTransformObj() noexcept
			{
			}

			void bind(const bound_indicator &indicators,
				  statementimplObj &statement) override
			{
				for (size_t i=0; i<statement.row_array_size;
				     ++i)
				{
					if (indicators.indicators[i] ==
					    SQL_NULL_DATA)
						continue;
					nativetypeArray[i]=convertClass
						::convert(sqlbuf[i]);
				}
			}
		};
		class convert_ymd;
		class convert_hms;

		// baseObj with a string_buffer

		class stringsbaseObj : public baseObj {

		public:
			// Temporary buffer for string columns, before they are
			// converted back to strings

			std::vector<char> string_buffer;
			size_t string_size;

			stringsbaseObj(size_t recsize, size_t row_array_size);
			~stringsbaseObj() noexcept;
		};

		class stringsObj : public stringsbaseObj {

		public:
			std::string *strings;

			stringsObj(std::string *stringsArg,
				   size_t recsize, size_t row_array_size);
			~stringsObj() noexcept;
			void bind(const bound_indicator &indicators,
				  statementimplObj &statement) override;
		};

		class bookmarksObj : public stringsbaseObj {

		public:
			bookmark *bookmarks;

			bookmarksObj(bookmark *bookmarksArg,
				     size_t recsize, size_t row_array_size);
			~bookmarksObj() noexcept;
			void bind(const bound_indicator &indicators,
				  statementimplObj &statement) override;
		};

		bound_indicator(bitflag *indicator_bools_iterArg,
				size_t row_array_size,
				ref<baseObj> &&dataArg=ref<baseObj>::create());

		~bound_indicator() noexcept;

		void resize(size_t recsize, size_t nrecords);
	};

	typedef std::list<bound_indicator> bound_indicator_list_t;
	bound_indicator_list_t bound_indicator_list;

	template<typename ...Args>
	bound_indicator_list_t::iterator add(bitflag *indicator_bools,
					     size_t row_array_size,
					     Args && ...args)
	{
		bound_indicator_list
			.emplace_back(std::move(indicator_bools),
				      row_array_size,
				      std::forward<Args>(args)...);
		return --bound_indicator_list.end();
	}

	// This is constructed on the stack before calling SQLBindCol.
	// If an exception gets thrown, the destructor removes the
	// binding from bound_indicator_list. installed() gets called if
	// SQLBindCol() succeeds, and the destructor leaves things alone.

	class indicator {

	public:
		statementimplObj *stmt;
		bound_indicator_list_t::iterator installed_iter;
		bool installedflag;

		template<typename ...Args>
		indicator(statementimplObj *stmtArg,
			  bitflag *indicator_bools,
			  size_t row_array_size,
			  Args ...args)
			: stmt(stmtArg),
			  installed_iter(stmt->add(std::move(indicator_bools),
						   row_array_size,
						   std::forward<Args>(args)...)
					 ),
			  installedflag(false)
		{
		}

		~indicator() noexcept;

		void installed() { installedflag=true; }

		operator SQLLEN *() const;
	};

	void save_num_params();
	size_t num_params() override;
	size_t num_params_val;

	const std::vector<parameter> &get_parameters() override;

	// Some additional buffer we need for SQLBindParameter()
	template<typename buffer_t>
		class bindBufferObj : virtual public obj {

	public:
		buffer_t buffer;

		bindBufferObj() {}
		~bindBufferObj() noexcept {}
	};

	// Buffers for strlen_or-ind parameters to SQLBindParameter()

	class strlen_or_ind_buffer_t {

	public:
		std::vector<SQLLEN> strlen_or_ind;
		ptr<obj> extrabuffer;

		strlen_or_ind_buffer_t();
		~strlen_or_ind_buffer_t() noexcept;
	};

	// We reserve() this ahead, so it won't get reallocated
	std::vector<strlen_or_ind_buffer_t> strlen_buffer;

	// Output buffer for SQL_ATTR_PARAM_STATUS_PTR
	std::vector<SQLUSMALLINT> param_status_buf;

	// After execute(), param_status_buf is returned here.
	bitflag *param_status_ptr;

	// How many parameter rows were processed.
	SQLULEN param_status_processed;

	bitflag execute() override;
	void begin_execute_params(bitflag *, size_t) override;
	void process_execute_params(size_t param_number);

	strlen_or_ind_buffer_t &add_lengths(size_t nvalues, size_t nnullvalues);
	strlen_or_ind_buffer_t &create_lengths_buffer(const bitflag *nullflags, size_t nvalues, size_t nnullvalues);

	SQLLEN *create_lengths(const bitflag *nullflags, size_t nvalues, size_t nnullvalues);

	void bind_input_parameter(size_t, SQLSMALLINT, SQLSMALLINT,
				  SQLSMALLINT, SQLSMALLINT, SQLPOINTER,
				  SQLLEN, SQLLEN *);

	void process_input_parameter(size_t param_number, const bitflag *n, const bitflag *nullflags, size_t nvalues, size_t nnullvalues) override;
	void process_input_parameter(size_t param_number, const short *n, const bitflag *nullflags, size_t nvalues, size_t nnullvalues) override;
	void process_input_parameter(size_t param_number, const unsigned short *n, const bitflag *nullflags, size_t nvalues, size_t nnullvalues) override;
	void process_input_parameter(size_t param_number, const int *n, const bitflag *nullflags, size_t nvalues, size_t nnullvalues) override;
	void process_input_parameter(size_t param_number, const unsigned *n, const bitflag *nullflags, size_t nvalues, size_t nnullvalues) override;
	void process_input_parameter(size_t param_number, const long *n, const bitflag *nullflags, size_t nvalues, size_t nnullvalues) override;
	void process_input_parameter(size_t param_number, const unsigned long *n, const bitflag *nullflags, size_t nvalues, size_t nnullvalues) override;
	void process_input_parameter(size_t param_number, const long long *n, const bitflag *nullflags, size_t nvalues, size_t nnullvalues) override;
	void process_input_parameter(size_t param_number, const unsigned long long *n, const bitflag *nullflags, size_t nvalues, size_t nnullvalues) override;
	void process_input_parameter(size_t param_number, const float *n, const bitflag *nullflags, size_t nvalues, size_t nnullvalues) override;
	void process_input_parameter(size_t param_number, const double *n, const bitflag *nullflags, size_t nvalues, size_t nnullvalues) override;

	void process_input_parameter(size_t param_number, const ymd *s,
				     const bitflag *nullflags, size_t nvalues,
				     size_t nnullvalues) override;

	void process_input_parameter(size_t param_number, const hms *s,
				     const bitflag *nullflags, size_t nvalues,
				     size_t nnullvalues) override;

	void process_string_input_parameter(size_t param_number,
					    const std::string *strp,
					    const char **charp,
					    const bitflag *nullflags,
					    size_t nvalues,
					    size_t nnullvalues);

	void process_input_parameter(size_t param_number, const std::string *s, const bitflag *nullflags, size_t nvalues, size_t nnullvalues) override;
	void process_input_parameter(size_t param_number, const char **s, const bitflag *nullflags, size_t nvalues, size_t nnullvalues) override;

	void verify_vector_execute(const std::vector<bitflag> &retvalues)
		override;
};

#if 0
{
	{
#endif
	};
};

#endif
