#ifndef sql_internal_H
#define sql_internal_H

#include "x/sql/env.H"
#include "x/sql/exception.H"
#include "x/sql/exceptionobj.H"
#include "x/sql/connection.H"
#include "x/sql/statement.H"
#include "x/logger.H"
#include <x/refiterator.H>
#include <sql.h>
#include <sqlext.h>
#include <list>
#include <string>
#include <string_view>

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

inline SQLCHAR *to_sqlcharptr(const std::string_view &s) LIBCXX_HIDDEN;

inline SQLCHAR *to_sqlcharptr(const std::string_view &s)
{
	return reinterpret_cast<SQLCHAR *>(const_cast<char *>(&*s.begin()));
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

	~diagnostics();
};

void sql_error(const char *function,
	       SQLHANDLE handle,
	       SQLSMALLINT type) LIBCXX_HIDDEN;

class LIBCXX_HIDDEN envimplObj : public envObj {

 public:
	SQLHENV h;

	envimplObj();
	~envimplObj();

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
			connect_flags flags) override;
};

class LIBCXX_HIDDEN statementimplObj;
class LIBCXX_HIDDEN newstatementimplObj;

class LIBCXX_HIDDEN execute {

 public:
	LOG_CLASS_SCOPE;
};

class LIBCXX_HIDDEN connectionimplObj : public connectionObj {

 public:
	SQLHDBC h;
	bool connected;
	unsigned transaction_scope_level;
	ref<envimplObj> env;
	std::string connstring;

	friend class newstatementimplObj;

	connectionimplObj(ref<envimplObj> &&envArg);
	~connectionimplObj();

	connection clone() const override;
	void ret(SQLRETURN ret, const char *func);

	diagnostics diags;

	void disconnect() override;
	void do_disconnect();

	statement tables_catalog(bool literal_ids,
				 const std::string &catalog,
				 const std::string &schema,
				 const std::string &table,
				 const std::string &type) override;

	statement table_privileges_catalog(bool literal_ids,
					   const std::string &table,
					   const std::string &catalog,
					   const std::string &schema) override;

	statement columns_catalog(bool literal_ids,
				  const std::string &catalog,
				  const std::string &schema,
				  const std::string &table,
				  const std::string &columnname) override;


	statement column_privileges_catalog(bool literal_ids,
					    const std::string &catalog,
					    const std::string &schema,
					    const std::string &table,
					    const std::string &columnname)
		override;

	statement primary_keys_catalog(bool literal_ids,
				       const std::string &table,
				       const std::string &catalog,
				       const std::string &schema) override;

	statement foreign_keys_catalog(bool literal_ids,
				       const std::string &pk_catalog,
				       const std::string &pk_schema,
				       const std::string &pk_table,
				       const std::string &fk_catalog,
				       const std::string &fk_schema,
				       const std::string &fk_table) override;

	statement procedure_columns_catalog(bool literal_ids,
					    const std::string &catalog,
					    const std::string &schema,
					    const std::string &proc,
					    const std::string &column
					    ) override;

	statement procedures_catalog(bool literal_ids,
				     const std::string &catalog,
				     const std::string &schema,
				     const std::string &proc
				     ) override;

	statement special_columns_catalog(bool literal_ids,
					  rowid_t rowid,
					  scope_t scope,
					  const std::string &table,
					  bool nullable,
					  const std::string &catalog,
					  const std::string &schema
					  ) override;

	statement statistics_catalog(bool literal_ids,
				     const std::string &table,
				     bool unique_only,
				     bool force,
				     const std::string &catalog,
				     const std::string &schema
				     ) override;

	statement type_info() override;
	statement type_info(const std::string &) override;

	statement prepare(const std::string &sql) override;

	newstatement create_newstatement() override;
	ref<newstatementimplObj> do_create_newstatement();
	statement execute_directly(const std::string_view &sql) override;

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

	void autocommit(bool value) override;
	void commit(bool turn_on_autocommit) override;
	void rollback(bool turn_on_autocommit) override;
	void check_not_transaction_scope_level(const char *funcname);
	std::string savepoint_name();
	void commit_rollback_work(const std::string &);
	void begin_work(const std::string &options) override;
	void commit_work() override;
	void rollback_work() override;
	std::string native_sql(const std::string &sql) override;
};

class LIBCXX_HIDDEN newstatementimplObj : public newstatementObj {

 public:

	ref<connectionimplObj> conn;

	std::map<SQLINTEGER, std::pair<const char *, SQLULEN>> ulen_attributes;

	std::string cursor_name;

#define STMT_ATTR(n, type, value) \
	set_attribute_ ## type(n, "SQLSetStmtAttr(" #type ", " #value ")", value)

	void set_attribute_ulen(SQLINTEGER, const char *, SQLULEN);

	newstatementimplObj(ref<connectionimplObj> &&connArg);
	~newstatementimplObj();

	statement prepare(const std::string &sql) override;
	statement prepare(const ref<statementimplObj> &s,
			  const std::string &sql);
	ref<statementimplObj> newstmt();

	void option(const std::string &name, const std::string &value)
		override;
};

class LIBCXX_HIDDEN statementimplObj : public statementObj {

	// Debug output collector buffer.
	std::string logbuffer;

 public:
	SQLHSTMT h;
	ref<connectionimplObj> conn;

	statementimplObj(const ref<connectionimplObj> &connArg);
	~statementimplObj();

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

	void bind_next(size_t column_number, const decimal_bound_parameter &params, bitflag *nullflag) override;

	void bind_next(size_t column_number,
		       const fetchblob<char> *factory,
		       bitflag *nullflag) override;

	void bind_next(size_t column_number,
		       const fetchblob<unsigned char> *factory,
		       bitflag *nullflag) override;

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

		class LIBCXX_PUBLIC baseObj : virtual public obj {

		public:
			baseObj();
			~baseObj();

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

			~bindTransformObj()
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

		class LIBCXX_PUBLIC stringsbaseObj : public baseObj {

		public:
			// Temporary buffer for string columns, before they are
			// converted back to strings

			std::vector<char> string_buffer;
			size_t string_size;

			stringsbaseObj(size_t recsize, size_t row_array_size);
			~stringsbaseObj();
		};

		class stringsObj : public stringsbaseObj {

		public:
			std::string *strings;

			stringsObj(std::string *stringsArg,
				   size_t recsize, size_t row_array_size);
			~stringsObj();
			void bind(const bound_indicator &indicators,
				  statementimplObj &statement) override;
		};

		class bookmarksObj : public stringsbaseObj {

		public:
			bookmark *bookmarks;

			bookmarksObj(bookmark *bookmarksArg,
				     size_t recsize, size_t row_array_size);
			~bookmarksObj();
			void bind(const bound_indicator &indicators,
				  statementimplObj &statement) override;
		};

		// Base class for processing blob columns. Takes care of
		// retrieving the blob's contents, which the subclass dumps
		// into the output iterator
		class blobBaseObj : public baseObj {

		public:
			int datatype;
			size_t column_number;

			// Constructor
			blobBaseObj(// SQL_C_CHAR or SQL_C_BINARY
				    int datatypeArg,

				    // Column number.
				    size_t column_numberArg);

			// Destructor
			~blobBaseObj();

			// Process bind results.
			void bind(const bound_indicator &indicators,
				  statementimplObj &statement) override;

			// Dump a chunk of data into the output iterator
			virtual void chunk(size_t row_number,
					   const char *ptr,
					   size_t cnt)=0;

			// Finished dumping a chunk of data.
			virtual void finish()=0;
		};

		// Save a char or a binary blob

		template<typename char_type, int datatype_val>
		class blobObj : public blobBaseObj {

		public:
			// The output iterator factory from the caller.
			fetchblob<char_type> factory;

			// The output iterator that we're writing to
			// This is a list of one. We create an output iterator
			// for the column, and save it together with its
			// close callback here. This way, these objects will
			// get automatically destroyed, if we get destroyed
			// to.
			//
			// finish() calls fetchclose callback, and empties
			// the list, before we move to the next column,
			std::list<std::pair<outputrefiterator<char_type>,
					    fetchclose>
				  > current_iterator_container;

			// The pointer to the raw iterator, for speed

			outputrefiteratorObj<char_type> *current_iterator;

			// Constructor
			blobObj(const fetchblob<char_type> &factoryArg,
				size_t column_numberArg)
				: blobBaseObj(datatype_val, column_numberArg),
				  factory(factoryArg),
				  current_iterator(nullptr)
			{
			}

			~blobObj()
			{
			}

			void chunk(size_t row_number,
				   const char *ptr,
				   size_t cnt) override
			{
				if (!current_iterator)
				{
					// First time, time to create an output
					// iterator.
					current_iterator_container
						.push_back(factory->create
							   (row_number));

					current_iterator=
						&*current_iterator_container
						.back().first;
				}

				while (cnt)
				{
					*current_iterator=*ptr++;
					--cnt;
				}
			}

			void finish() override
			{
				if (current_iterator)
				{
					// Save the close callback.
					auto cpy=current_iterator_container
						.back().second;

					// Clean up
					current_iterator=nullptr;
					current_iterator_container.clear();

					// Invoke the close callback
					cpy->close();
				}
			}
		};

		bound_indicator(bitflag *indicator_bools_iterArg,
				size_t row_array_size,
				ref<baseObj> &&dataArg=ref<baseObj>::create());

		~bound_indicator();

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

		~indicator();

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
		~bindBufferObj() {}
	};

	// Buffers for strlen_or-ind parameters to SQLBindParameter()

	class strlen_or_ind_buffer_t {

	public:
		std::vector<SQLLEN> strlen_or_ind;
		ptr<obj> extrabuffer;

		const insertblob *blobptr;
		size_t blobindex;

		strlen_or_ind_buffer_t();
		~strlen_or_ind_buffer_t();
	};

	// We reserve() this ahead, so it won't get reallocated
	std::vector<strlen_or_ind_buffer_t> strlen_buffer;

	// Output buffer for SQL_ATTR_PARAM_STATUS_PTR
	std::vector<SQLUSMALLINT> param_status_buf;

	// After execute(), param_status_buf is returned here.
	bitflag *param_status_ptr;

	// How many parameter rows were processed.
	SQLULEN param_status_processed;

	// Maximum number of rows requested
	SQLULEN maxrows;

	// How many rows were retrieved
	size_t rows_retrieved;

	void limit(size_t maxrowsArg) override;
	bitflag execute() override;
	void begin_execute_params(bitflag *, size_t) override;
	void process_execute_params(size_t param_number) override;

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
					    const char *const *charp,
					    const bitflag *nullflags,
					    size_t nvalues,
					    size_t nnullvalues,
					    bool shortcut_for_one);

	void process_input_parameter(size_t param_number, const std::string *s, const bitflag *nullflags, size_t nvalues, size_t nnullvalues) override;
	void process_input_parameter(size_t param_number, const char * const *s, const bitflag *nullflags, size_t nvalues, size_t nnullvalues) override;
	void process_input_parameter(size_t param_number, const insertblob *blobs, const bitflag *nullflags, size_t nvalues, size_t nnullvalues) override;
	void process_input_parameter(size_t param_number, const decimal_input_parameter &input, const bitflag *nullflags, size_t nvalues, size_t nnullvalues)
		override;

	void verify_vector_execute(const std::vector<bitflag> &retvalues)
		override;

	statement prepare_modify_fetched_row(size_t rownum,
					     const std::string &sql) override;

	std::string get_cursor_name();

	bool more() override;
	size_t row_count() override;
	void next_resultset();
};

// Defined in libcxxsqldecimal, where decimal_bound_parameter's constructor
// is implemented, so we'll be able to link with it.

ref<statementimplObj::bound_indicator::baseObj>
bind_decimal_column(const decimal_bound_parameter &params,
		    size_t recsize,
		    size_t row_array_size,
		    SQLPOINTER &targetRet,
		    SQLLEN &buffer_lengthRet) __attribute__((weak));

#if 0
{
	{
#endif
	};
};

#endif
