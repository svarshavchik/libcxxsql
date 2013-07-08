/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "sql_internal.H"
#include <algorithm>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include "gettext_in.h"
#include "x/exception.H"
#include "x/ymd.H"
#include "x/hms.H"
#include "x/tostring.H"
#include "x/join.H"
#include "x/messages.H"
#include "x/sql/insertblob.H"
#include "x/sql/fetchblob.H"

namespace LIBCXX_NAMESPACE {
	namespace sql {
#if 0
	}
};
#endif

bookmark::bookmark()
{
}

bookmark::~bookmark() noexcept
{
}

bookmark &bookmark::operator=(vectorptr<unsigned char> &&value)
{
	vectorptr<unsigned char>::operator=(std::move(value));
	return *this;
}

// Translate our exported fetch orientation objects into ODBC constants

const fetch_orientation fetch::next(SQL_FETCH_NEXT, 0, nullptr);

const fetch_orientation fetch::prior(SQL_FETCH_PRIOR, 0, nullptr);

const fetch_orientation fetch::last(SQL_FETCH_LAST, 0, nullptr);

const fetch_orientation fetch::first(SQL_FETCH_FIRST, 0, nullptr);

fetch::absolute::absolute(long long pos) :
	// We use 0-based row numbers, ODBC uses 1-based row numbers
	fetch_orientation(SQL_FETCH_ABSOLUTE, pos+1, nullptr)
{
	if (offset < 0)
		throw EXCEPTION(_TXT(_txt("Absolute row number too large")));
}

fetch::relative::relative(long long pos) :
	fetch_orientation(SQL_FETCH_RELATIVE, pos, nullptr)
{
}


fetch::atbookmark::atbookmark(const bookmark &bookmarksaveArg,
			      long long offset) :
	fetch_orientation(SQL_FETCH_BOOKMARK, offset, &bookmarkSave),
	bookmarkSave(bookmarksaveArg)
{
}

statementObj::execute_factory::execute_factory(statementObj &stmtArg,
					       size_t param_numberArg)
	: stmt(stmtArg), param_number(param_numberArg)
{
}

void statementObj::execute_factory::parameter(std::nullptr_t null_param)
{
	// Handle this as a single NULL character string parameter.

	const char *nullp=nullptr;
	bitflag nullflag=1;

	stmt.process_input_parameter(param_number, &nullp,
				     &nullflag, 1, 1);
	++param_number;
}

statementObj::statementObj()
{
}

statementObj::~statementObj() noexcept
{
}

void statementObj::prep_retvalues(std::vector<bitflag> &retvaluesArg)
{
	std::fill(retvaluesArg.begin(), retvaluesArg.end(), 0);
}

statementimplObj::statementimplObj(const ref<connectionimplObj> &connArg)
	: h(nullptr), conn(connArg), num_rows_fetched(0), have_columns(false),
	  have_parameters(false), num_params_val(0), param_status_processed(0),
	  maxrows(0)
{
	if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_STMT, conn->h, &h)))
	{
		h=nullptr;
		sql_error("SQLAllocHandle", conn->h, SQL_HANDLE_DBC);
	}
}

statementimplObj::~statementimplObj() noexcept
{
	if (h)
		SQLFreeHandle(SQL_HANDLE_STMT, h);
}

void statementimplObj::ret(SQLRETURN ret, const char *func)
{
	if (!SQL_SUCCEEDED(ret))
		sql_error(func, h, SQL_HANDLE_STMT);
}

void statementimplObj::limit(size_t maxrowsArg)
{
	maxrows=maxrowsArg;
}

bitflag statementimplObj::execute()
{
	bitflag retval;

	// This is equivalent to an execute with an empty parameter list.

	begin_execute_params(&retval, 1);
	process_execute_params(0);

	return retval;
}

void statementimplObj::begin_execute_params(bitflag *status,
					    size_t execute_rows)
{
	logbuffer.clear();

	if (execute_rows <= 0)
		throw EXCEPTION(_TXT(_txt("Row array size not positive")));

	next_resultset();

	// Unbind any existing bound parameters. Clear all buffers.

	strlen_buffer.clear();
	strlen_buffer.reserve(num_params_val);

	ret(SQLFreeStmt(h, SQL_RESET_PARAMS), "SQLFreeStmt");

	param_status_buf.resize(execute_rows);
	param_status_ptr=status;
	std::fill(param_status_buf.begin(),
		  param_status_buf.end(), SQL_PARAM_ERROR);

	SET_ATTR(SQL_ATTR_PARAM_STATUS_PTR, ptr, &param_status_buf[0]);
	SET_ATTR(SQL_ATTR_PARAMS_PROCESSED_PTR, ptr, &param_status_processed);
	param_status_processed=0;
	SET_ATTR(SQL_ATTR_PARAMSET_SIZE, ulen, execute_rows);
	SET_ATTR(SQL_ATTR_MAX_LENGTH, ulen, 0);
	SET_ATTR(SQL_ATTR_MAX_ROWS, ulen, maxrows);
}

// Called right after prepare() to retrieve the number of parameters.

void statementimplObj::save_num_params()
{
	SQLSMALLINT count=0;
	ret(SQLNumParams(h, &count), "SQLNumParams");
	num_params_val=count;
}

size_t statementimplObj::num_params()
{
	return num_params_val;
}

statementimplObj::parameter::parameter(const char *data_typeArg,
				       size_t parameter_sizeArg,
				       size_t decimal_digitsArg,
				       bool nullableArg)
	: data_type(data_typeArg),
	  parameter_size(parameter_sizeArg),
	  decimal_digits(decimal_digitsArg),
	  nullable(nullableArg)
{
}

statementimplObj::parameter::~parameter() noexcept
{
}

const std::vector<statementimplObj::parameter> &statementimplObj::get_parameters()
{
	if (!have_parameters)
	{
		parameters.clear();

		parameters.reserve(num_params_val);

		for (size_t i=0; i<num_params_val; ++i)
		{
			SQLSMALLINT DataType;
			SQLULEN ParameterSize;
			SQLSMALLINT DecimalDigits;
			SQLSMALLINT Nullable;

			ret(SQLDescribeParam(h, i+1, &DataType, &ParameterSize,
					     &DecimalDigits,
					     &Nullable), "SQLDescribeParam");

			parameters.emplace_back(type_to_name(DataType),
						ParameterSize,
						DecimalDigits,
						!!Nullable);
		}
		have_parameters=true;
	}
	return parameters;
}

void statementimplObj::process_execute_params(size_t param_number)
{
	LOG_FUNC_SCOPE(execute::logger);

	if (param_number != num_params_val)
		throw EXCEPTION((std::string)
				gettextmsg(_TXTN(_txtn("%1% parameter in the SQL statement; ",
						       "%1% parameters in the SQL statement."), num_params_val), num_params_val) +
				(std::string)
				gettextmsg(_TXTN(_txtn("%1% parameter provided; ",
						       "%1% parameters provided."),
						 param_number),
					   param_number));

	LOG_DEBUG("SQLExecute("
		  // Each execute parameter is prefixed by ", "
		  << (logbuffer.empty() ? "":logbuffer.substr(2))
		  << ")");

	decltype(SQLExecute(h)) retval;

	if ((retval=SQLExecute(h)) == SQL_NEED_DATA)
	{
		// There are blobs to insert, here.

		SQLPOINTER blob;

		while ((retval=SQLParamData(h, &blob)) == SQL_NEED_DATA)
		{
			strlen_or_ind_buffer_t *paramdata
				=(strlen_or_ind_buffer_t *)blob;

			// The next non-NULL parameter.

			while (paramdata->blobindex <
			       paramdata->strlen_or_ind.size())
			{
				if (paramdata->
				    strlen_or_ind[paramdata->blobindex]
				    != SQL_NULL_DATA)
					break;
				++paramdata->blobindex;
			}

			if (paramdata->blobindex >=
			    paramdata->strlen_or_ind.size())
				throw EXCEPTION("Internal error: insert blob index exceeds vector size");

			const insertblob *current_blob=
				paramdata->blobptr + paramdata->blobindex;
			++paramdata->blobindex; // For next time.

			size_t bufsize=fdbaseObj::get_buffer_size();
			char buffer[bufsize];

			try {
				size_t s;

				// We need to call SQLPutData() at least once.
				bool first=true;

				while ((s=(*current_blob)
					->fill(buffer, bufsize)) > 0 || first)
				{
					first=false;
					ret(SQLPutData(h, buffer, s),
					    "SQLPutData");
				}
			} catch (...) {

				SQLCancel(h);
				std::fill(param_status_ptr,
					  param_status_ptr+
					  param_status_buf.size(), 0);
				param_status_buf.clear();
				strlen_buffer.clear();
				throw;
			}
		}
	}

	// Process per-row results, for vector inserts.
	// Replace "unavailable" status from ODBC with the results of the
	// Execute() call.

	bitflag diag_unavailable= SQL_SUCCEEDED(retval) ? 1:0;

	std::transform(param_status_buf.begin(),
		       param_status_buf.end(),
		       param_status_ptr,
		       [diag_unavailable]
		       (SQLUSMALLINT v)
		       {
			       return v == SQL_PARAM_ERROR ? 0:
				       v == SQL_PARAM_SUCCESS ? 1:
				       v == SQL_PARAM_UNUSED ? 2:
				       v == SQL_PARAM_DIAG_UNAVAILABLE ?
				       diag_unavailable:3;
		       });
	param_status_buf.clear();
	strlen_buffer.clear();
	num_rows_fetched=0;

	if (retval == SQL_NO_DATA)
		return; // UPDATE, INSERT, DELETE that did not affect any rows

	ret(retval, "SQLExecute");
}

// Map from C type to ODBC constants

// This creates a mapping between C integer types, based on sizeof(type),
// and the corresponding ODBC C and SQL types. ODBC C and SQL types are based
// on the integer size, so the mapping is different on 32 and 64 bit platforms!
//
// signed_type() and unsigned_type() figure out the ODBC C type.
// The situation with sql_type() is tricky. We typically select a matching
// sql_type, except that unsigned values may overflow the corresponding
// signed sql value (there are, apparently, no unsigned SQL values), so we
// pick the next higher one.

template<size_t> class LIBCXX_HIDDEN sql_c_size;

template<> class LIBCXX_HIDDEN sql_c_size<2> {

 public:
	typedef uint16_t uint_t;

	static constexpr int signed_type() { return SQL_C_SSHORT; }
	static constexpr int unsigned_type() { return SQL_C_USHORT; }
	static constexpr int sql_type() { return SQL_SMALLINT; }
	static constexpr int sql_type_next() { return SQL_INTEGER; }
};

template<> class LIBCXX_HIDDEN sql_c_size<4> {

 public:

	typedef uint32_t uint_t;

	static constexpr int signed_type() { return SQL_C_SLONG; }
	static constexpr int unsigned_type() { return SQL_C_ULONG; }
	static constexpr int sql_type() { return SQL_INTEGER; }
	static constexpr int sql_type_next() { return SQL_BIGINT; }
};

template<> class LIBCXX_HIDDEN sql_c_size<8> {

 public:

	typedef uint64_t uint_t;

	static constexpr int signed_type() { return SQL_C_SBIGINT; }
	static constexpr int unsigned_type() { return SQL_C_UBIGINT; }
	static constexpr int sql_type() { return SQL_BIGINT; }
	static constexpr int sql_type_next() { return SQL_BIGINT; }
};

// Create strlen_or_ind buffer for SQLBindParameter

statementimplObj::strlen_or_ind_buffer_t::strlen_or_ind_buffer_t()
{
}

statementimplObj::strlen_or_ind_buffer_t::~strlen_or_ind_buffer_t() noexcept
{
}

statementimplObj::strlen_or_ind_buffer_t &
statementimplObj::add_lengths(size_t nvalues, size_t nnullvalues)
{
	if (nvalues != nnullvalues)
		throw EXCEPTION(_TXT(_txt("Size of parameter null vector different than the size of the parameter vector")));

	// Make sure # of values for this parameter is the same as the
	// declared number of values for each parameter.

	auto s=param_status_buf.size();

	if (nvalues != s)
		throw EXCEPTION((std::string)
				gettextmsg(_TXTN(_txtn("%1% parameter value provided",
						       "%1% parameter values provided"),
						 nvalues), nvalues) +
				(std::string)
				gettextmsg(_TXTN(_txtn(" for a statement with %1% value for each parameter",
						       " for a statement with %1% values for each parameter"),
						 s), s));

	strlen_buffer.push_back(strlen_or_ind_buffer_t());

	auto &b=strlen_buffer.back();
	b.strlen_or_ind.resize(nvalues);
	return b;
}

// Create strlen_or_ind parameter to SQLBindParameter for non-string parameters

statementimplObj::strlen_or_ind_buffer_t
&statementimplObj::create_lengths_buffer(const bitflag *nullflags,
					 size_t nvalues,
					 size_t nnullvalues)
{
	auto &buffer=add_lengths(nvalues, nnullvalues);

	buffer.strlen_or_ind.resize(nvalues);

	auto p=&buffer.strlen_or_ind[0];

	for (size_t i=0; i<nvalues; ++i)
		// Non-NULL single parameter values do not specify nullflags
		p[i]=nullflags && nullflags[i] ? SQL_NULL_DATA:0;

	return buffer;
}

SQLLEN *statementimplObj::create_lengths(const bitflag *nullflags,
					 size_t nvalues,
					 size_t nnullvalues)
{
	return &create_lengths_buffer(nullflags, nvalues, nnullvalues)
		.strlen_or_ind[0];
}

// Common function for binding an input parameter.

void statementimplObj::bind_input_parameter(size_t param_num,
					    SQLSMALLINT data_type,
					    SQLSMALLINT parameter_size,
					    SQLSMALLINT decimal_digits,
					    SQLSMALLINT value_type,
					    SQLPOINTER value,
					    SQLLEN value_length,
					    SQLLEN *strlen_or_ind)
{
	auto num_params_val=num_params();

	if (param_num >= num_params_val)
		throw EXCEPTION((std::string)
				gettextmsg(_TXTN(_txtn("More than %1% parameter was specified in execute()",
						       "More than %1% parameters were specified in execute()"),
						 num_params_val),
					   num_params_val));

	if (param_status_buf.empty())
		// vector execute with 0 rows. This is only legal
		// if there are no parameters
		throw EXCEPTION(_TXT(_txt("Parameter vectors are empty")));

	ret(SQLBindParameter(h, param_num+1, // 1-based
			     SQL_PARAM_INPUT,
			     value_type,
			     data_type,
			     parameter_size,
			     decimal_digits,
			     value,
			     value_length,
			     strlen_or_ind), "SQLBindParameter");
}

///////////////////////////////////////////////////////////////////////////////
//
// Bind various types of parameters.

#define LOG_GET_NTH(v,n,o,l)						\
	if (n)								\
		(o) << "NULL";						\
	else								\
		(o) << LIBCXX_NAMESPACE::tostring((v),(l));

// Log bound parameter values. We use LOG_DEBUG() to conditionally execute
// this spaghetti only when needed. Each parameter gets collected into
// logbuffer, and LOG_DEBUG gets an empty string (which logs nothing).

#define LOG_BOUND_PARAMETER_EMIT(vptr,nptr,n,get_v,get_n)		\
	do {								\
		LOG_FUNC_SCOPE(execute::logger);			\
									\
		LOG_DEBUG( (						\
			{						\
				std::ostringstream o;			\
				auto l=LIBCXX_NAMESPACE::locale::base	\
					::environment();		\
									\
				o << ", ";				\
									\
				if ((n) == 1)				\
				{					\
					LOG_GET_NTH(get_v((vptr), (nptr), 0), \
						    get_n((vptr), (nptr), 0), \
						    o, l);		\
				}					\
				else					\
				{					\
					o << "[";			\
					const char *sep="";		\
					for (size_t i=0; i<(n); ++i)	\
					{				\
						o << sep;		\
						LOG_GET_NTH(get_v((vptr),\
								  (nptr), i), \
							    get_n((vptr), \
								  (nptr), i), \
							    o, l);	\
						sep=", ";		\
					}				\
					o << "]";			\
				}					\
									\
				logbuffer += o.str(); "";		\
			}));						\
	} while(0)

#define LOG_NTH_VALUE(vptr,nptr,i)	((vptr)[i])

#define LOG_NTH_ISNULL(vptr,nptr,i)	((nptr) ? (nptr)[i]:0)

#define LOG_BOUND_PARAMETER(vptr,nptr,n)		\
	LOG_BOUND_PARAMETER_EMIT((vptr),(nptr),(n),LOG_NTH_VALUE,LOG_NTH_ISNULL)

#define LOG_NTH_VALUE_ASINT(vptr,nptr,i)	((int)((vptr)[i]))

#define LOG_BOUND_PARAMETER_ASINT(vptr,nptr,n)				\
	LOG_BOUND_PARAMETER_EMIT((vptr),(nptr),(n),LOG_NTH_VALUE_ASINT,LOG_NTH_ISNULL)

void statementimplObj::process_input_parameter(size_t param_number, const bitflag *n, const bitflag *nullflags, size_t nvalues, size_t nnullvalues)
{
	bind_input_parameter(param_number,
			     SQL_BIT, 0, 0,
			     SQL_C_BIT,
			     const_cast<void *>(reinterpret_cast<const void *>(n)),
			     0, create_lengths(nullflags, nvalues,
					       nnullvalues));

	LOG_BOUND_PARAMETER_ASINT(n, nullflags, nvalues);
}

void statementimplObj::process_input_parameter(size_t param_number, const short *n, const bitflag *nullflags, size_t nvalues, size_t nnullvalues)
{
	bind_input_parameter(param_number,
			     sql_c_size<sizeof(*n)>::sql_type(), sizeof(*n), 0,
			     sql_c_size<sizeof(*n)>::signed_type(),
			     const_cast<void *>(reinterpret_cast<const void *>(n)),
			     0, create_lengths(nullflags, nvalues,
					       nnullvalues));
	LOG_BOUND_PARAMETER(n, nullflags, nvalues);
}

void statementimplObj::process_input_parameter(size_t param_number, const unsigned short *n, const bitflag *nullflags, size_t nvalues, size_t nnullvalues)
{
	bind_input_parameter(param_number,
			     (short)(*n) < 0 ?
			     sql_c_size<sizeof(*n)>::sql_type_next() :
			     sql_c_size<sizeof(*n)>::sql_type(), sizeof(*n), 0,
			     sql_c_size<sizeof(*n)>::unsigned_type(),
			     const_cast<void *>(reinterpret_cast<const void *>(n)),
			     0, create_lengths(nullflags, nvalues,
					       nnullvalues));
	LOG_BOUND_PARAMETER(n, nullflags, nvalues);
}

void statementimplObj::process_input_parameter(size_t param_number, const int *n, const bitflag *nullflags, size_t nvalues, size_t nnullvalues)
{
	bind_input_parameter(param_number,
			     sql_c_size<sizeof(*n)>::sql_type(), sizeof(*n), 0,
			     sql_c_size<sizeof(*n)>::signed_type(),
			     const_cast<void *>(reinterpret_cast<const void *>(n)),
			     0, create_lengths(nullflags, nvalues,
					       nnullvalues));
	LOG_BOUND_PARAMETER(n, nullflags, nvalues);
}

void statementimplObj::process_input_parameter(size_t param_number, const unsigned *n, const bitflag *nullflags, size_t nvalues, size_t nnullvalues)
{
	bind_input_parameter(param_number,
			     (int)(*n) < 0 ?
			     sql_c_size<sizeof(*n)>::sql_type_next() :
			     sql_c_size<sizeof(*n)>::sql_type(), sizeof(*n), 0,
			     sql_c_size<sizeof(*n)>::unsigned_type(),
			     const_cast<void *>(reinterpret_cast<const void *>(n)),
			     0, create_lengths(nullflags, nvalues,
					       nnullvalues));
	LOG_BOUND_PARAMETER(n, nullflags, nvalues);
}

void statementimplObj::process_input_parameter(size_t param_number, const long *n, const bitflag *nullflags, size_t nvalues, size_t nnullvalues)
{
	bind_input_parameter(param_number,
			     sql_c_size<sizeof(*n)>::sql_type(), sizeof(*n), 0,
			     sql_c_size<sizeof(*n)>::signed_type(),
			     const_cast<void *>(reinterpret_cast<const void *>(n)),
			     0, create_lengths(nullflags, nvalues,
					       nnullvalues));
	LOG_BOUND_PARAMETER(n, nullflags, nvalues);
}

void statementimplObj::process_input_parameter(size_t param_number, const unsigned long *n, const bitflag *nullflags, size_t nvalues, size_t nnullvalues)
{
	bind_input_parameter(param_number,
			     (long)(*n) < 0 ?
			     sql_c_size<sizeof(*n)>::sql_type_next() :
			     sql_c_size<sizeof(*n)>::sql_type(), sizeof(*n), 0,
			     sql_c_size<sizeof(*n)>::unsigned_type(),
			     const_cast<void *>(reinterpret_cast<const void *>(n)),
			     0, create_lengths(nullflags, nvalues,
					       nnullvalues));
	LOG_BOUND_PARAMETER(n, nullflags, nvalues);
}

void statementimplObj::process_input_parameter(size_t param_number, const long long *n, const bitflag *nullflags, size_t nvalues, size_t nnullvalues)
{
	bind_input_parameter(param_number,
			     sql_c_size<sizeof(*n)>::sql_type(), sizeof(*n), 0,
			     sql_c_size<sizeof(*n)>::signed_type(),
			     const_cast<void *>(reinterpret_cast<const void *>(n)),
			     0, create_lengths(nullflags, nvalues,
					       nnullvalues));
	LOG_BOUND_PARAMETER(n, nullflags, nvalues);
}

void statementimplObj::process_input_parameter(size_t param_number, const unsigned long long *n, const bitflag *nullflags, size_t nvalues, size_t nnullvalues)
{
	bind_input_parameter(param_number,
			     sql_c_size<sizeof(*n)>::sql_type(), sizeof(*n), 0,
			     sql_c_size<sizeof(*n)>::unsigned_type(),
			     const_cast<void *>(reinterpret_cast<const void *>(n)),
			     0, create_lengths(nullflags, nvalues,
					       nnullvalues));
	LOG_BOUND_PARAMETER(n, nullflags, nvalues);
}

void statementimplObj::process_input_parameter(size_t param_number, const float *n, const bitflag *nullflags, size_t nvalues, size_t nnullvalues)
{
	bind_input_parameter(param_number,
			     SQL_REAL, sizeof(*n), 0,
			     SQL_C_FLOAT,
			     const_cast<void *>(reinterpret_cast<const void *>(n)),
			     0, create_lengths(nullflags, nvalues,
					       nnullvalues));
	LOG_BOUND_PARAMETER(n, nullflags, nvalues);
}

void statementimplObj::process_input_parameter(size_t param_number, const double *n, const bitflag *nullflags, size_t nvalues, size_t nnullvalues)
{
	bind_input_parameter(param_number,
			     SQL_DOUBLE, sizeof(*n), 0,
			     SQL_C_DOUBLE,
			     const_cast<void *>(reinterpret_cast<const void *>(n)),
			     0, create_lengths(nullflags, nvalues,
					       nnullvalues));
	LOG_BOUND_PARAMETER(n, nullflags, nvalues);
}

// Translate ymd to DATE_STRUCT

void statementimplObj::process_input_parameter(size_t param_number,
					       const ymd *s,
					       const bitflag *nullflags,
					       size_t nvalues,
					       size_t nnullvalues)
{
	auto &buffers=create_lengths_buffer(nullflags, nvalues, nnullvalues);

	auto buffer=ref<bindBufferObj<std::vector<DATE_STRUCT>>>::create();

	buffer->buffer.reserve(nvalues);

	std::transform(s, s+nvalues,
		       std::back_insert_iterator<std::vector<DATE_STRUCT>>
		       (buffer->buffer),
		       []
		       (const ymd &d)
		       {
			       auto y=d.getYear();
			       DATE_STRUCT ds={(SQLSMALLINT)y,
					       d.getMonth(),
					       d.getDay()};

			       if (ds.year <= 0 || ds.year != y)
				       throw EXCEPTION(_TXT(_txt("Year overflow")));
			       return ds;
		       });

	buffers.extrabuffer=buffer;
	bind_input_parameter(param_number,
			     SQL_TYPE_DATE, 0, 0,
			     SQL_C_TYPE_DATE,
			     const_cast<void *>(reinterpret_cast<const void *>
						(&buffer->buffer[0])),
			     0, &buffers.strlen_or_ind[0]);
	LOG_BOUND_PARAMETER(s, nullflags, nvalues);
}

// Translate hms to TIME_STRUCT

void statementimplObj::process_input_parameter(size_t param_number,
					       const hms *s,
					       const bitflag *nullflags,
					       size_t nvalues,
					       size_t nnullvalues)
{
	auto &buffers=create_lengths_buffer(nullflags, nvalues, nnullvalues);

	auto buffer=ref<bindBufferObj<std::vector<TIME_STRUCT>>>::create();

	buffer->buffer.reserve(nvalues);

	std::transform(s, s+nvalues,
		       std::back_insert_iterator<std::vector<TIME_STRUCT>>
		       (buffer->buffer),
		       []
		       (const hms &t)
		       {
			       TIME_STRUCT ts={(SQLUSMALLINT)t.h,
					       (SQLUSMALLINT)t.m,
					       (SQLUSMALLINT)t.s};

			       if (ts.hour != t.h ||
				   ts.minute != t.m ||
				   ts.second != t.s)
				       throw EXCEPTION(_TXT(_txt("Time of day overflow")));

			       return ts;
		       });

	buffers.extrabuffer=buffer;
	bind_input_parameter(param_number,
			     SQL_TYPE_TIME, 0, 0,
			     SQL_C_TYPE_TIME,
			     const_cast<void *>(reinterpret_cast<const void *>
						(&buffer->buffer[0])),
			     0, &buffers.strlen_or_ind[0]);
	LOG_BOUND_PARAMETER(s, nullflags, nvalues);
}

// C-escape a string, for logging purposes

static std::string log_quote(const std::string &v)
{
	std::ostringstream o;

	o << '"';

	for (const char c:v)
	{
		if (c == '\n')
			o << "\\n";
		else if (c == '\r')
			o << "\\r";
		else if (c == '\t')
			o << "\\t";
		else if (c == 0)
			o << "\\0";
		else if (c < ' ' || c >= 127)
		{
			o << "\\x" << std::hex << std::setw(2)
			  << (int)(unsigned char)c << std::dec << std::setw(0);
		}
		else if (c == '\\' || c == '"')
			o << "\\" << c;
		else
			o << c;
	}
	o << '"';
	return o.str();
}

// Process string input parameters.
// They get specified either as const char *s or as std::strings. Either strp
// or charp is null.

void statementimplObj::process_string_input_parameter(size_t param_number,
						      const std::string *strp,
						      const char * const *charp,
						      const bitflag *nullflags,
						      size_t nvalues,
						      size_t nnullvalues)
{
	size_t l;
	auto &buffers=add_lengths(nvalues, nnullvalues);
	const char *p;

	// Non-NULL single parameter values do not specify nullflags
#define IS_CHAR_NULL(i) ((nullflags && nullflags[i]) || (charp && !charp[i]))

	// We can avoid doing a lot of work for a single value string parameter.

	if (nvalues == 1)
	{
		if (IS_CHAR_NULL(0))
		{
			l=0;
			p=nullptr;
			buffers.strlen_or_ind[0]=SQL_NULL_DATA;
		}
		else
		{
			if (charp)
			{
				l=strlen(*charp);
				p=*charp;
			}
			else
			{
				l=strp->size();
				p=strp->c_str();
			}
			buffers.strlen_or_ind[0]=l;
		}
	}
	else
	{
		// ODBC wants a single buffer for everything. Zoinks.

		// Compute maximum string length

		l=0;

#define INPUT_STRPTR(i) (charp ? charp[i]:strp[i].c_str())
#define INPUT_STRLEN(i) (charp ? strlen(charp[i]):strp[i].size())

		for (size_t i=0; i<nvalues; ++i)
		{
			size_t sl=IS_CHAR_NULL(i) ? 0:INPUT_STRLEN(i);

			if (l < sl)
				l=sl;
		}

		size_t bufsize=l*nvalues;

		// Overflow check :-)
		if (bufsize / nvalues != l || bufsize % nvalues)
			throw EXCEPTION("Buffer for string parameters is bigger than the entire universe");

		auto charbuffer=ref<bindBufferObj<std::vector<char>>>::create();
		buffers.extrabuffer=charbuffer;

		charbuffer->buffer.resize(bufsize);

		// Put everything into a single buffer.

		char *s;
		p=s=l ? &charbuffer->buffer[0]:nullptr;
		auto s_or_i=&buffers.strlen_or_ind[0];

		for (size_t i=0; i<nvalues; ++i, ++s_or_i, s += l)
		{
			if (IS_CHAR_NULL(i))
			{
				*s_or_i=SQL_NULL_DATA;
			}
			else
			{
				size_t sl=INPUT_STRLEN(i);
				*s_or_i=sl;
				memcpy(s, INPUT_STRPTR(i), sl);
			}
		}
	}

	bind_input_parameter(param_number,
			     SQL_CHAR, l, 0,
			     SQL_C_CHAR,
			     const_cast<void *>(reinterpret_cast<const void *>(p)),
			     l, &buffers.strlen_or_ind[0]);

#define GET_STRV_PARAM(v,n,i)			\
	log_quote(charp ? charp[i]:strp[i])
#define GET_STRN_PARAM(v,n,i) IS_CHAR_NULL(i)

	LOG_BOUND_PARAMETER_EMIT(dummy_value, dummy_null, nvalues,
				 GET_STRV_PARAM,
				 GET_STRN_PARAM);
}

void statementimplObj::process_input_parameter(size_t param_number,
					       const std::string *s,
					       const bitflag *nullflags,
					       size_t nvalues,
					       size_t nnullvalues)
{
	process_string_input_parameter(param_number, s, nullptr,
				       nullflags, nvalues, nnullvalues);
}

void statementimplObj::process_input_parameter(size_t param_number,
					       const char *const *s,
					       const bitflag *nullflags,
					       size_t nvalues,
					       size_t nnullvalues)
{
	process_string_input_parameter(param_number, nullptr, s, nullflags,
				       nvalues, nnullvalues);
}

// Plaseholder for insert blobs.

void statementimplObj::process_input_parameter(size_t param_number,
					       const insertblob *blobs,
					       const bitflag *nullflags,
					       size_t nvalues,
					       size_t nnullvalues)
{
	auto &buffer=create_lengths_buffer(nullflags, nvalues, nnullvalues);
	SQLLEN *lengths=&buffer.strlen_or_ind[0];

	// Check if the ODBC driver needs to know how big it is.

	bool need_long_data_len=conn->config_get_need_long_data_len();

	for (size_t i=0; i<nnullvalues; ++i)
	{
		if (lengths[i] == SQL_NULL_DATA)
			continue;

		if (need_long_data_len)
		{
			size_t s=blobs[i]->blobsize();

			if (s == (size_t)-1)
				throw EXCEPTION(_TXT(_txt("Cannot use input iterators for blobs")));

			// Sanity check: s too big?

			std::remove_reference<decltype(lengths[i])>::type v;

			if ((size_t)(decltype(v))s != s ||

			    // SQL_LEN_DATA_AT_EXEC should return a negative
			    // value.

			    (v=SQL_LEN_DATA_AT_EXEC(s)) > 0)
				blobs[i]->blobtoobig();

			lengths[i]=v;

		}
		else
		{
			lengths[i]=SQL_DATA_AT_EXEC;
		}
	}

	buffer.blobptr=blobs;
	buffer.blobindex=0;

	bind_input_parameter(param_number,
			     (*blobs)->sqltype(),
			     sizeof(*blobs), 0,
			     (*blobs)->datatype(),
			     (SQLPOINTER)&buffer, 0, lengths);

#define GET_BLOBV_PARAM(v,n,i) "blob"

	LOG_BOUND_PARAMETER_EMIT(dummy_value, nullflags, nvalues,
				 GET_BLOBV_PARAM,
				 LOG_NTH_ISNULL);

}

size_t statementimplObj::size()
{
	SQLSMALLINT n;

	ret(SQLNumResultCols(h, &n), "SQLNumResultCols");

	return n;
}

void statementimplObj::set_attr_str(SQLINTEGER attribute,
				    const char *attribute_str,
				    const std::string &v)
{
	ret(SQLSetStmtAttr(h, attribute,
			   reinterpret_cast<void *>
			   (const_cast<char *>(v.c_str())), v.size()),
	    attribute_str);
}

void statementimplObj::set_attr_uint(SQLINTEGER attribute,
				     const char *attribute_str,
				     SQLUINTEGER n)
{
	ret(SQLSetStmtAttr(h, attribute,
			   (SQLPOINTER)
			   (sql_c_size<sizeof(SQLPOINTER)>::uint_t)n,
			   SQL_IS_UINTEGER), attribute_str);
}

void statementimplObj::set_attr_ulen(SQLINTEGER attribute,
				     const char *attribute_str,
				     SQLULEN n)
{
	ret(SQLSetStmtAttr(h, attribute,
			   (SQLPOINTER)
			   (sql_c_size<sizeof(SQLPOINTER)>::uint_t)n,
			   SQL_IS_UINTEGER), attribute_str);
}

void statementimplObj::set_attr_ptr(SQLINTEGER attribute,
				    const char *attribute_str,
				    SQLPOINTER ptr)
{
	ret(SQLSetStmtAttr(h, attribute, ptr, 0), attribute_str);
}

std::string statementimplObj::colattribute_str(size_t i,
					       SQLUSMALLINT field_identifier,
					       const char *field_identifier_str,
					       const char *default_value)
{
	SQLSMALLINT n=0;

	auto rc=SQLColAttribute(h, i, field_identifier, nullptr, 0, &n,
				nullptr);

	if (!SQL_SUCCEEDED(rc))
	{
		auto errors=diagnostics(field_identifier_str, h,
					SQL_HANDLE_STMT);

		if (default_value && errors.e->is("HY091"))
		{
			// Most likely unimplemented by the driver, and we
			// have a default value.
			return default_value;
		}
		throw errors.e;
	}

	++n;
	n *= 4;  // TODO: MySQL bug wchar/char conversion.

	SQLCHAR buffer[n];

	ret(SQLColAttribute(h, i, field_identifier, buffer, n, &n, nullptr),
	    field_identifier_str);

	buffer[n]=0; // Possible ODBC bug (SQL_DESC_BASE_TABLE_NAME)

	return reinterpret_cast<char *>(buffer);
}

SQLLEN statementimplObj::colattribute_n(size_t i,
					SQLUSMALLINT field_identifier,
					const char *field_identifier_str)
{
	SQLLEN n=0;

	ret(SQLColAttribute(h, i, field_identifier, nullptr, 0, nullptr, &n),
	    field_identifier_str);
	return n;
}

// Make many calls to SQLColAttribute to describe the columns in the resultset
// and dump the data into a vector of structures that, hopefully, makes sense.

const std::vector<statementimplObj::column> &statementimplObj::get_columns()
{
	if (!have_columns)
	{
		columns.clear();
		columnmap.clear();

		auto n=size();

		columns.reserve(n);

		for (decltype(n) i=0; i<n; )
		{
			++i; // Column numbers are 1-based

#define BV(n) (colattribute_n(i, n, "SQLColAttribute(" #n ")") ? true:false)
#define V(n) colattribute_n(i, n, "SQLColAttribute(" #n ")")
#define S(n) colattribute_str(i, n, "SQLColAttribute(" #n ")", 0)
#define SOPT(n,d) colattribute_str(i, n, "SQLColAttribute(" #n ")", d)
			auto searchable=V(SQL_DESC_SEARCHABLE);

			auto radix=V(SQL_DESC_NUM_PREC_RADIX);

			auto basecolumnname=S(SQL_DESC_BASE_COLUMN_NAME);

			auto nameval=S(SQL_DESC_NAME);

			if (V(SQL_DESC_UNNAMED) != SQL_NAMED)
				nameval=basecolumnname;

			const char *type=type_to_name(V(SQL_DESC_TYPE));

			columns.emplace_back(column(BV(SQL_DESC_AUTO_UNIQUE_VALUE),
						    BV(SQL_DESC_CASE_SENSITIVE),
						    BV(SQL_DESC_FIXED_PREC_SCALE),
						    BV(SQL_DESC_NULLABLE),
						    BV(SQL_DESC_UNSIGNED),
						    BV(SQL_DESC_UPDATABLE),
						    searchable ==
						    SQL_LIKE_ONLY ||
						    searchable ==
						    SQL_PRED_SEARCHABLE,
						    searchable ==
						    SQL_ALL_EXCEPT_LIKE ||
						    searchable ==
						    SQL_PRED_SEARCHABLE,
						    basecolumnname,
						    S(SQL_DESC_BASE_TABLE_NAME),
						    S(SQL_DESC_CATALOG_NAME),

						    type,
						    type_to_name(V(SQL_DESC_CONCISE_TYPE)),
						    S(SQL_DESC_TYPE_NAME),
						    i-1,
						    V(SQL_DESC_DISPLAY_SIZE),
						    V(SQL_DESC_LENGTH),
						    V(SQL_DESC_OCTET_LENGTH),
						    V(SQL_DESC_PRECISION),
						    radix < 0 ? 0:radix,
						    V(SQL_DESC_SCALE),
						    S(SQL_DESC_LABEL),
						    S(SQL_DESC_LITERAL_PREFIX),
						    S(SQL_DESC_LITERAL_SUFFIX),
						    SOPT(SQL_DESC_LOCAL_TYPE_NAME, type),
						    nameval,
						    S(SQL_DESC_SCHEMA_NAME),
						    S(SQL_DESC_TABLE_NAME)));
			auto &last=columns.back();

			columnmap.insert(std::pair<std::string,
					 decltype(last) &>
					 (last.name, last));
		}
		have_columns=true;
	}
	return columns;
}

const statementObj::columnmap_t &statementimplObj::get_columnmap()
{
	get_columns();

	return columnmap;
}

statementimplObj::column::operator std::string() const
{
	std::vector<std::string> props;

	props.reserve(50);

	props.emplace_back("name: " + name);
	props.emplace_back("column: " + base_table_name + "." + base_column_name);
	props.emplace_back("table: " + catalog_name + "." + schema + "." + table);

	props.emplace_back("type: " + std::string(type));
	props.emplace_back("concise type: " + std::string(concise_type));
	props.emplace_back("native type: " + native_type);
	props.emplace_back("localized type: " + local_type_name);
	props.emplace_back("width: " + tostring(width));
	props.emplace_back("length: " + tostring(length));
	props.emplace_back("octet length: " + tostring(octet_length));
	props.emplace_back("precision: " + tostring(precision));
	props.emplace_back("radix: " + tostring(radix));
	props.emplace_back("scale: " + tostring(scale));

	if (autoincrement)
		props.emplace_back("autoincrement");
	if (case_sensitive)
		props.emplace_back("case sensitive");
	if (nullable)
		props.emplace_back("nullable");
	if (is_unsigned)
		props.emplace_back("unsigned");
	if (is_updatable)
		props.emplace_back("updatable");
	if (fixed)
		props.emplace_back("fixed");

	props.emplace_back(searchable_like ? searchable_except_like
			   ? "searchable":"searchable (LIKE)"
			   : searchable_except_like
			   ? "searchable (EXCEPT LIKE)":"not searchable");
	
	props.emplace_back("prefix: " + literal_prefix);
	props.emplace_back("suffix: " + literal_suffix);

	return join(props, ", ");
}

/////////////////////////////////////////////////////////////////////////////
//
// Bound columns often need a helping hand, in form of an intermediate buffer
// of some sort.

statementimplObj::bound_indicator::baseObj::baseObj()
{
}

statementimplObj::bound_indicator::baseObj::~baseObj() noexcept
{
}

void statementimplObj::bound_indicator::baseObj::bind(const bound_indicator &,
						      statementimplObj &s)
{
}

statementimplObj::bound_indicator::bound_indicator(bitflag *nullflag,
						   size_t row_array_size,
						   ref<baseObj> &&dataArg)
	: indicator_bools(nullflag), data(std::move(dataArg))
{
	indicators.resize(row_array_size);
}

statementimplObj::bound_indicator::~bound_indicator() noexcept
{
}

// Base class for bound string buffers. We need to allocate one huge chunk
// of a buffer, then, when we're done, we chop it up into individual
// std::strings.
//
// This is also used to handle bookmark columns, for convenience.

statementimplObj::bound_indicator::stringsbaseObj
::stringsbaseObj(size_t recsize,
		 size_t row_array_size)
{
	size_t total_size=recsize * row_array_size;

	if (total_size / row_array_size != recsize ||
	    total_size % row_array_size)
		throw EXCEPTION("Buffer for string parameters is bigger than the entire universe");

	string_size=recsize;
	string_buffer.resize(total_size);
}

statementimplObj::bound_indicator::stringsbaseObj::~stringsbaseObj() noexcept
{
}

// Bind a string column

statementimplObj::bound_indicator::stringsObj
::stringsObj(std::string *stringsArg,
	     size_t recsize,
	     size_t row_array_size) : stringsbaseObj(recsize, row_array_size),
				      strings(stringsArg)
{
}

statementimplObj::bound_indicator::stringsObj::~stringsObj() noexcept
{
}

// Chop things up into individual std::strings.

void statementimplObj::bound_indicator::stringsObj
::bind(const bound_indicator &b,
       statementimplObj &)
{
	const char *p=&string_buffer[0];
	auto s=strings;
	auto indicators=&b.indicators[0];
	size_t num_rows_fetched=b.indicators.size();

	for (size_t i=0; i<num_rows_fetched;
	     ++i, p += string_size, ++indicators, ++s)
	{
		s->clear();
		if (*indicators != SQL_NULL_DATA)
			s->append(p, *indicators);
	}
}

statementimplObj::bound_indicator::bookmarksObj
::bookmarksObj(bookmark *bookmarksArg,
	       size_t recsize, size_t row_array_size)
	: stringsbaseObj(recsize, row_array_size),
	  bookmarks(bookmarksArg)
{
}

statementimplObj::bound_indicator::bookmarksObj
::~bookmarksObj() noexcept
{
}

// Common base class for binding character and binary blobs

statementimplObj::bound_indicator::blobBaseObj
::blobBaseObj(int datatypeArg, size_t column_numberArg)
	: datatype(datatypeArg), column_number(column_numberArg)
{
}

statementimplObj::bound_indicator::blobBaseObj::~blobBaseObj() noexcept
{
}

void statementimplObj::bound_indicator::blobBaseObj
::bind(const bound_indicator &indicators,
       statementimplObj &statement)
{
	size_t bufsize=fdbaseObj::get_buffer_size();
	char buffer[bufsize];
	SQLLEN retlen;

	for (size_t i=0; i<statement.row_array_size; ++i)
	{
		// Call SQLSetPos only if we really need to. Avoid trolling
		// for driver bugs :-(

		if (statement.row_array_size > 1)
		{
			statement.ret(SQLSetPos(statement.h, i+1, SQL_POSITION,
						SQL_LOCK_NO_CHANGE),
				      "SQLSetPos");
		}

		// Clear the null flag, if given.

		if (indicators.indicator_bools)
			indicators.indicator_bools[i]=0;

		// Repeatedly call SQLGetData(), until we're done.

		bool done=false;

		do
		{
			auto ret=SQLGetData(statement.h, column_number,
					    datatype,
					    (SQLPOINTER)buffer,
					    sizeof(buffer),
					    &retlen);

			if (retlen == SQL_NULL_DATA)
			{
				// This is a NULL value.

				if (indicators.indicator_bools)
					indicators.indicator_bools[i]=1;
				break;
			}

			statement.ret(ret, "SQLGetData");

			size_t c=sizeof(buffer);

			if (retlen != SQL_NO_TOTAL)
			{
				if ((size_t)retlen <= c)
				{
					// Last chunk.
					c=retlen;
					done=true;
				}
			}

			// For char blobs, we want to stop at the \0 byte.

			if (datatype == SQL_C_CHAR)
				c=strnlen(buffer, c);

			chunk(i, buffer, c);
			if (done)
				finish();
		} while (!done);
	}				    
}

void statementimplObj::bound_indicator::bookmarksObj::bind(const
							   bound_indicator &b,
							   statementimplObj
							   &stmt)
{
	// Pull out bookmarks.

	const char *p=&string_buffer[0];
	auto s=bookmarks;
	auto indicators=&b.indicators[0];

	size_t num_rows_fetched=b.indicators.size();

	for (size_t i=0; i<num_rows_fetched;
	     ++i, p += string_size, ++indicators, ++s)
	{
		*s=*indicators != SQL_NULL_DATA
			? vectorptr<unsigned char>
			::create(p, p+*indicators)
			: vectorptr<unsigned char>();
	}
}

statementimplObj::indicator::~indicator() noexcept
{
	if (!installedflag)
		stmt->bound_indicator_list.erase(installed_iter);
}

statementimplObj::indicator::operator SQLLEN *() const
{
	return &installed_iter->indicators[0];
}

void statementimplObj::clear_binds(size_t row_array_sizeArg)
{
	// Initialize list of bound columns

	if (row_array_sizeArg <= 0)
		throw EXCEPTION(_TXT(_txt("Row array size not positive")));

	row_array_size=row_array_sizeArg;
	row_status_array.resize(row_array_size);

	SET_ATTR(SQL_ATTR_ROW_BIND_TYPE, ulen, SQL_BIND_BY_COLUMN);
	SET_ATTR(SQL_ATTR_ROW_ARRAY_SIZE, ulen, row_array_size);
	SET_ATTR(SQL_ATTR_ROW_STATUS_PTR, ptr, &row_status_array[0]);

	ret(SQLFreeStmt(h, SQL_UNBIND), "SQLFreeStmt");
	bound_indicator_list.clear();

	SET_ATTR(SQL_ATTR_ROWS_FETCHED_PTR, ptr, &num_rows_fetched);

	scroll_orientation=SQL_FETCH_NEXT;
	scroll_offset=0;
	scroll_bookmark=bookmark();
}

// End of bound columns, save positioning indicator for the upcoming fetch.

void statementimplObj::bind_all(const fetch_orientation &orientation)
{
	scroll_orientation=orientation.orientation;

	if ((scroll_offset=orientation.offset) != orientation.offset)
		throw EXCEPTION((std::string)
				gettextmsg(_TXT(_txt("Scroll offset of %1% "
						     "exceeds word size")),
					   orientation.offset));
	if (orientation.bookmarkptr)
	{
		scroll_bookmark= *orientation.bookmarkptr;
		ret(SQLSetStmtAttr(h, SQL_ATTR_FETCH_BOOKMARK_PTR,
				   (SQLPOINTER)(&*scroll_bookmark->begin()),
				   scroll_bookmark->size()),
		    "SQLSetStmtAttr(SQL_ATTR_FETCH_BOOKMARK_PTR)");
	}
}

size_t statementimplObj::bind_column_name(const std::string &name)
{
	// Translate column name to number

	auto range=get_columnmap().equal_range(name);

	if (range.first == range.second)
		throw EXCEPTION(gettextmsg(_TXT(_txt("Column %1% not found")),
					   name));
	if (--range.second != range.first)
		throw EXCEPTION(gettextmsg(_TXT(_txt("Column %1% occurs more than once in the resultset")),
					   name));

	return range.first->second.column_number;
}

void statementimplObj::bind_bookmarks(bookmark *bookmarks)
{
	// Bind bookmark column.

	SQLLEN len=colattribute_n(0, SQL_DESC_OCTET_LENGTH,
				  "SQLColAttribute(SQL_DESC_OCTET_LENGTH(Bookmark))");

	auto bind=ref<bound_indicator::bookmarksObj>::create(bookmarks, len,
							     row_array_size);

	indicator ind(this, nullptr, row_array_size, bind);

	ret(SQLBindCol(h, 0, SQL_C_VARBOOKMARK,
		       &bind->string_buffer[0],
		       bind->string_size, ind), "SQLBindCol(Bookmark)");
	ind.installed();
}

///////////////////////////////////////////////////////////////////////////////
//
// Bind various kinds of columns

void statementimplObj::bind_next(size_t column_number, bitflag *n,
				 bitflag *nullflag)
{
	indicator ind(this, nullflag, row_array_size);

	ret(SQLBindCol(h, column_number+1, SQL_C_BIT, n, 0, ind), "SQLBindCol");
	ind.installed();
}

void statementimplObj::bind_next(size_t column_number, short *n, bitflag *nullflag)
{
	indicator ind(this, nullflag, row_array_size);

	ret(SQLBindCol(h, column_number+1,
		       sql_c_size<sizeof(short)>::signed_type(),
		       n, 0, ind), "SQLBindCol");
	ind.installed();
}

void statementimplObj::bind_next(size_t column_number, unsigned short *n, bitflag *nullflag)
{
	indicator ind(this, nullflag, row_array_size);

	ret(SQLBindCol(h, column_number+1,
		       sql_c_size<sizeof(short)>::unsigned_type(),
		       n, 0, ind), "SQLBindCol");
	ind.installed();
}

void statementimplObj::bind_next(size_t column_number, int *n, bitflag *nullflag)
{
	indicator ind(this, nullflag, row_array_size);

	ret(SQLBindCol(h, column_number+1,
		       sql_c_size<sizeof(int)>::signed_type(),
		       n, 0, ind), "SQLBindCol");
	ind.installed();
}

void statementimplObj::bind_next(size_t column_number, unsigned int *n, bitflag *nullflag)
{
	indicator ind(this, nullflag, row_array_size);

	ret(SQLBindCol(h, column_number+1,
		       sql_c_size<sizeof(int)>::unsigned_type(),
		       n, 0, ind), "SQLBindCol");
	ind.installed();
}

void statementimplObj::bind_next(size_t column_number, long *n, bitflag *nullflag)
{
	indicator ind(this, nullflag, row_array_size);

	ret(SQLBindCol(h, column_number+1,
		       sql_c_size<sizeof(long)>::signed_type(),
		       n, 0, ind), "SQLBindCol");
	ind.installed();
}

void statementimplObj::bind_next(size_t column_number, unsigned long *n, bitflag *nullflag)
{
	indicator ind(this, nullflag, row_array_size);

	ret(SQLBindCol(h, column_number+1,
		       sql_c_size<sizeof(long)>::signed_type(),
		       n, 0, ind), "SQLBindCol");
	ind.installed();
}

void statementimplObj::bind_next(size_t column_number, long long *n, bitflag *nullflag)
{
	indicator ind(this, nullflag, row_array_size);

	ret(SQLBindCol(h, column_number+1,
		       sql_c_size<sizeof(long long)>::signed_type(),
		       n, 0, ind), "SQLBindCol");
	ind.installed();
}

void statementimplObj::bind_next(size_t column_number, unsigned long long *n, bitflag *nullflag)
{
	indicator ind(this, nullflag, row_array_size);

	ret(SQLBindCol(h, column_number+1,
		       sql_c_size<sizeof(long long)>::signed_type(),
		       n, 0, ind), "SQLBindCol");
	ind.installed();
}

void statementimplObj::bind_next(size_t column_number, float *n, bitflag *nullflag)
{
	indicator ind(this, nullflag, row_array_size);

	ret(SQLBindCol(h, column_number+1, SQL_C_FLOAT, n, 0, ind), "SQLBindCol");
	ind.installed();
}

void statementimplObj::bind_next(size_t column_number, double *n, bitflag *nullflag)
{
	indicator ind(this, nullflag, row_array_size);

	ret(SQLBindCol(h, column_number+1, SQL_C_DOUBLE, n, 0, ind), "SQLBindCol");
	ind.installed();
}

class statementimplObj::bound_indicator::convert_ymd {

 public:

	static inline ymd convert(const DATE_STRUCT &ds)
	{
		return ymd(ds.year, ds.month, ds.day);
	}
};

void statementimplObj::bind_next(size_t column_number,
				 ymd *s, bitflag *nullflag)
{
	auto bind=ref<bound_indicator::bindTransformObj<
			      DATE_STRUCT, ymd,
			      bound_indicator::convert_ymd>>
		::create(s, row_array_size);

	indicator ind(this, nullflag, row_array_size, bind);

	ret(SQLBindCol(h, column_number+1, SQL_C_DATE,
		       &bind->sqlbuf[0],
		       0, ind), "SQLBindCol");
	ind.installed();
}

class statementimplObj::bound_indicator::convert_hms {

public:

	static inline hms convert(const TIME_STRUCT &tod)
	{
		return hms(tod.hour, tod.minute, tod.second);
	}
};

void statementimplObj::bind_next(size_t column_number, hms *s,
				 bitflag *nullflag)
{
	auto bind=ref<bound_indicator::bindTransformObj<
			      TIME_STRUCT, hms,
			      bound_indicator::convert_hms>>
		::create(s, row_array_size);

	indicator ind(this, nullflag, row_array_size, bind);

	ret(SQLBindCol(h, column_number+1, SQL_C_TIME,
		       &bind->sqlbuf[0],
		       0, ind), "SQLBindCol");
	ind.installed();
}

void statementimplObj::bind_next(size_t i,
				 std::string *s, bitflag *nullflag)
{
	++i; // For the benefit of V()

	auto bind=ref<bound_indicator::stringsObj>
		::create(s, V(SQL_DESC_LENGTH)+1, row_array_size);

	indicator ind(this, nullflag, row_array_size, bind);

	ret(SQLBindCol(h, i, SQL_C_CHAR,
		       &bind->string_buffer[0],
		       bind->string_size, ind), "SQLBindCol");
	ind.installed();
}

void statementimplObj::bind_next(size_t column_number,
				 const fetchblob<char> *factory,
				 bitflag *nullflag)
{
	indicator(this, nullflag, row_array_size,
		  ref<bound_indicator::blobObj<char, SQL_C_CHAR>>
		  ::create(*factory, column_number+1)).installed();
}

void statementimplObj::bind_next(size_t column_number,
				 const fetchblob<unsigned char> *factory,
				 bitflag *nullflag)
{
	indicator(this, nullflag, row_array_size,
		  ref<bound_indicator::blobObj<unsigned char, SQL_C_BINARY>>
		  ::create(*factory, column_number+1)).installed();
}

// Everything leads up to this point.

size_t statementimplObj::fetch_into()
{
	auto rc=SQLFetchScroll(h, scroll_orientation, scroll_offset);

	if (rc == SQL_NO_DATA)
		return 0;

	ret(rc, "SQLFetchScroll");

	// Process bind columns.

	for (const auto &b:bound_indicator_list)
	{
		if (b.indicator_bools)
		{
			// This bound column wanted a NULL indicator

			std::transform(&b.indicators[0],
				       &b.indicators[num_rows_fetched],
				       b.indicator_bools,
				       []
				       (SQLLEN v)
				       {
					       return v == SQL_NULL_DATA;
				       }
				       );
		}

		b.data->bind(b, *this);
	}

	return num_rows_fetched;
}

void statementimplObj::
verify_vector_execute(const std::vector<bitflag> &retvalues)
{
	auto error_count=
		std::count_if(retvalues.begin(), retvalues.end(),
			      []
			      (bitflag f)
			      {
				      return !(f & 1);
			      });

	if (error_count)
		throw diagnostics("SQLExecute", h, SQL_HANDLE_STMT).e;
}

statement statementimplObj::prepare_modify_fetched_row(size_t rownum,
						       const std::string &sql)
{
	if (rownum >= num_rows_fetched)
	{
		if (num_rows_fetched == 0)
			throw EXCEPTION(_TXT(_txt("update_fetched_row called after no rows were fetched")));
		
		throw EXCEPTION((std::string)
				gettextmsg(_TXT(_txt("update_fetched_row called for row #%1% when the last fetched row is row #%2%")),
					   rownum,
					   num_rows_fetched-1));
	}

	if (num_rows_fetched > 1) // Only if necessary
	{
		ret(SQLSetPos(h, rownum+1, SQL_POSITION, SQL_LOCK_NO_CHANGE),
		    "SQLSetPos");
	}

	return conn->prepare(sql + " WHERE CURRENT OF " + get_cursor_name());
}

std::string statementimplObj::get_cursor_name()
{
	SQLSMALLINT length;

	ret(SQLGetCursorName(h, nullptr, 0, &length), "SQLGetCursorName");

	++length;	// TODO: MySQL bug wchar/char conversion.
	length *= 4;
	SQLCHAR buf[length+1];

	ret(SQLGetCursorName(h, buf, length+1, &length), "SQLGetCursorName");

	return std::string(buf, buf+length);
}

bool statementimplObj::more()
{
	auto rc=SQLMoreResults(h);

	if (rc == SQL_NO_DATA)
		return false;
	next_resultset();
	ret(rc, "SQLMoreResults");
	return true;
}

// Prepare for the next resultset. Currently -- not so much stuff is here.

void statementimplObj::next_resultset()
{
	have_columns=false;
}

size_t statementimplObj::row_count()
{
	SQLLEN c;

	ret(SQLRowCount(h, &c), "SQLRowCount");

	return c;
}

template statement connectionObj::execute(const char * &&);
template statement connectionObj::execute(std::string &&);
template statement newstatementObj::execute(const std::string &);
template bitflag statementObj::modify_fetched_row(size_t, const std::string &);

#if 0
{
	{
#endif
	};
};
