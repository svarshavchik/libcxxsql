/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_sql_statementobj_H
#define x_sql_statementobj_H

#include <x/obj.H>
#include <x/ptrfwd.H>
#include <x/chrcasecmp.H>
#include <x/refiteratorfwd.H>
#include <x/sql/connectionfwd.H>
#include <x/sql/newstatementfwd.H>
#include <x/sql/insertblobfwd.H>
#include <x/sql/fetchblobfwd.H>
#include <vector>
#include <unistd.h>

namespace LIBCXX_NAMESPACE {

	class ymd;
	class hms;

	namespace sql {
#if 0
	};
};
#endif

//! A prepared SQL statement

class statementObj : virtual public obj {

public:
	//! Constructor
	statementObj();

	//! Destructor
	~statementObj() noexcept;

	//! Execute the statement.
	virtual bitflag execute()=0;

	//! Number of parameters in the statement
	virtual size_t num_params()=0;

	//! Parameter metadata

	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::sql::statement::base::parameter
	class parameter {

	public:
		//! The datatype, as a string.
		const char *data_type;

		//! Parameter size
		size_t parameter_size;

		//! Decimal digits
		size_t decimal_digits;

		//! Nullable
		bool nullable;

		//! Constructor
		parameter(const char *data_typeArg,
			  size_t parameter_sizeArg,
			  size_t decimal_digitsArg,
			  bool nullable);

		//! Destructor
		~parameter() noexcept;
	};

	//! Return parameters in the SQL statement.

	virtual const std::vector<parameter> &get_parameters()=0;

	//! Execute the statement with parameters.
	template<typename firstArg, typename ...MoreArgs>
	bitflag execute(firstArg &&arg, MoreArgs &&...moreArgs)
	{
		bitflag value;

		begin_execute_params(&value, 1);
		process_execute_params(0, std::forward<firstArg>(arg),
				       std::forward<MoreArgs>(moreArgs)...);
		return value;
	}

	//! Execute the statement with parameters given as vectors.

	template<typename ...Args>
	void execute_vector(//! Return value of each executed row
			    std::vector<bitflag> &retvalues,

			    //! Parameter vectors
			    Args &&...args)
	{
		// Note that connectionObj::execute_vector() needs to clear
		// before prepare(), so it also does the following:

		prep_retvalues(retvalues);
		begin_execute_params(&retvalues[0], retvalues.size());
		process_execute_vector_params(0, std::forward<Args>(args)...);
		verify_vector_execute(retvalues);
	}

	friend class newstatementObj;

	//! Initialize execute version return value buffer

	//! \internal
	//!
	static void prep_retvalues(std::vector<bitflag> &retvaluesArg);

	//! Return number of columns in the statement
	virtual size_t size()=0;

	//! Resultset column metadata

	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::sql::statement::base::column.

	class column {

	public:
		//! Whether the column is a unique autoincrement
		bool autoincrement;

		//! Whether the column is case_sensitive
		bool case_sensitive;

		//! Whether the column is fixed precision
		bool fixed;

		//! Whether the column is nullable
		bool nullable;

		//! Whether the column is unsigned
		bool is_unsigned;

		//! Whether the column is updatable
		bool is_updatable;

		//! This column can be searched using LIKE
		bool searchable_like;

		//! This column can be searched using operators other than LIKE
		bool searchable_except_like;

		//! Base column name
		std::string base_column_name;

		//! Base table name
		std::string base_table_name;

		//! Catalog name
		std::string catalog_name;

		//! The type of the column, like "char", or "integer"
		const char *type;

		//! The concise type of the column, like "char", or "integer"
		const char *concise_type;

		//! The native type of the column
		std::string native_type;

		//! Column number in the result set, 0-based.
		size_t column_number;

		//! Column display size

		//! For character columns, this is the maximum number of
		//! characters. For numeric columns this is the maximum
		//! number of digits.
		size_t width;

		//! Column length

		//! Actual or maximum number of characters in the column.
		size_t length;

		//! Octet length

		//! The maximum size of the buffer needed to hold the
		//! column's value. Generally meaningful for character columns,
		//! gives the maximum number of bytes that are needed to
		//! allocate for the column's value.
		size_t octet_length;

		//! Precision of a numeric column
		size_t precision;

		//! Radix

		//! For numeric columns that are implemented as ordinary
		//! integers, this is 2. For numeric columns that are defined
		//! as an exact numeric type, this is 10, and precision gives
		//! the number of digits in the number.
		size_t radix;

		//! Scale of a numeric column
		size_t scale;

		//! Column label

		//! A descriptive label for the column, used by some
		//! database engines.
		std::string label;

		//! Literal prefix

		//! To be prepended to the column's value, if it's to be
		//! displayed, like an apostrophe for a string value.
		std::string literal_prefix;

		//! Literal suffix

		//! To be appended to the column's value, if it's to be
		//! displayed, like an apostrophe for a string value.
		std::string literal_suffix;

		//! Localized type name

		//! The type of the column, in the current locale (used by
		//! some database drivers).
		std::string local_type_name;

		//! Column name

		//! The official column name, if known
		std::string name;

		//! Schema
		std::string schema;

		//! Table
		std::string table;

		//! Constructor
		column(bool autoincrementArg,
		       bool case_sensitiveArg,
		       bool fixedArg,
		       bool nullableArg,
		       bool is_unsignedArg,
		       bool is_updatableArg,
		       bool searchable_likeArg,
		       bool searchable_except_likeArg,
		       const std::string &base_column_nameArg,
		       std::string &&base_table_nameArg,
		       std::string &&catalog_nameArg,
		       const char *typeArg,
		       const char *concise_typeArg,
		       std::string &&native_typeArg,
		       size_t column_numberArg,
		       size_t widthArg,
		       size_t lengthArg,
		       size_t octet_lengthArg,
		       size_t precisionArg,
		       size_t radixArg,
		       size_t scaleArg,
		       std::string &&labelArg,
		       std::string &&literal_prefixArg,
		       std::string &&literal_suffixArg,
		       std::string &&local_type_nameArg,
		       const std::string &nameArg,
		       std::string &&schemaArg,
		       std::string &&tableArg)
		: autoincrement(autoincrementArg),
			case_sensitive(case_sensitiveArg),
			fixed(fixedArg),
			nullable(nullableArg),
			is_unsigned(is_unsignedArg),
			is_updatable(is_updatableArg),
			searchable_like(searchable_likeArg),
			searchable_except_like(searchable_except_likeArg),
			base_column_name(base_column_nameArg),
			base_table_name(std::move(base_table_nameArg)),
			catalog_name(std::move(catalog_nameArg)),
			type(typeArg),
			concise_type(concise_typeArg),
			native_type(std::move(native_typeArg)),
			column_number(column_numberArg),
			width(widthArg),
			length(lengthArg),
			octet_length(octet_lengthArg),
			precision(precisionArg),
			radix(radixArg),
			scale(scaleArg),
			label(std::move(labelArg)),
			literal_prefix(std::move(literal_prefixArg)),
			literal_suffix(std::move(literal_suffixArg)),
			local_type_name(std::move(local_type_nameArg)),
			name(nameArg),
			schema(std::move(schemaArg)),
			table(std::move(tableArg))
		{
		}

		//! Describe the column
		operator std::string() const;
	};

	//! Return current resultset's columns

	//! Returns a vector of metadata of the columns in the current
	//! resultset.
	//!
	//! See also \ref get_columnmap().
	virtual const std::vector<column> &get_columns()=0;

	//! Resultset column metadata

	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::sql::statement::base::columnmap_t;

	typedef std::multimap<std::string, column &,
			      chrcasecmp::str_less> columnmap_t;

	//! Return current resultset map

	//! This returns the same metadata as get_columns(), expressed as
	//! a multimap keyed by case insensitive column.name.
	//! This is a multimap, in case that more than one column has the
	//! same name.

	virtual const columnmap_t &get_columnmap()=0;

private:
	//! Helper class used to process the first argument to fetch() when it's a bookmark

	template<typename firstArgType> class fetch_first_arg {

	public:

		//! The default template implementation is when the first argument is not a bookmark

		template<typename firstArg, typename ...Args>
		static inline void bind_all(statementObj &stmt,
					    firstArg &&arg,
					    Args && ...args)
		{
			stmt.bind_all(std::forward<firstArg>(arg),
				      std::forward<Args>(args)...);
		}
	};

	//! Helper class used to process the first argument to fetch() when it's a bookmark vector

	template<typename firstArgType> class fetch_vectors_first_arg {

	public:

		//! The default template implementation is when the first argument is not a bookmark

		template<typename firstArg, typename ...Args>
		static inline void bind_vectors_all(statementObj &stmt,
						    size_t rowsize,
						    firstArg &&arg,
						    Args && ...args)
		{
			stmt.bind_vectors_all(rowsize,
					      std::forward<firstArg>(arg),
					      std::forward<Args>(args)...);
		}
	};

public:
	//! Fetch a single row.

	template<typename firstArg, typename ...Args>
	bool fetch(firstArg &&arg, Args && ...args)
	{
		clear_binds(1);
		fetch_first_arg<firstArg>
			::bind_all(*this,
				   std::forward<firstArg>(arg),
				   std::forward<Args>(args)...);
		return fetch_into() > 0;
	}

	//! Fetch a vector of rows

	template<typename firstArg, typename ...Args>
	size_t fetch_vectors(//! How many rows to fetch
			     size_t rowsize,

			     //! First argument
			     firstArg && arg,

			     //! Remaining arguments
			     Args && ...args)
	{
		clear_binds(rowsize);

		fetch_vectors_first_arg<firstArg>
			::bind_vectors_all(*this, rowsize,
					   std::forward<firstArg>(arg),
					   std::forward<Args>(args)...);
		return fetch_into();
	}

	//! Fetch all rows into vectors

	template<typename Functor, typename firstArg, typename ...Args>
	void fetch_vectors_all(//! How many rows to fetch, at a time
			       size_t rowsize,

			       //! Functor gets repeatedly invoked.
			       //! Functor received number of rows fetched.

			       Functor &&functor,

			       //! First argument
			       firstArg && arg,

			       //! Columns
			       Args && ...args)
	{
		clear_binds(rowsize);

		fetch_vectors_first_arg<firstArg>
			::bind_vectors_all(*this, rowsize,
					   std::forward<firstArg>(arg),
					   std::forward<Args>(args)...);

		size_t n;

		while ((n=fetch_into()) > 0)
			functor(n);
	}

private:
	//! No more resultset values to bind.

	inline void bind_all()
	{
	}

	//! End fetch() parameters with an explicit scroll indication
	virtual void bind_all(const fetch_orientation &orientation)=0;

	//! Bind a resultset column by column number.

	template<typename argType, typename ...Args>
	inline void bind_all(size_t n, argType &&arg,
			     Args && ...args)
	{
		bind_1(n, std::forward<argType>(arg));
		bind_all(std::forward<Args>(args)...);
	}

	//! Bind a resultset column by column name.

	template<typename argType, typename ...Args>
	inline void bind_all(const std::string &column_name, argType &&arg,
			     Args && ...args)
	{
		bind_1(column_name, std::forward<argType>(arg));
		bind_all(std::forward<Args>(args)...);
	}

	//! Bind multiple resultset columns

	template<typename keyType, typename argType, typename ...Args>
	inline void bind_all(std::map<keyType, argType> &arg,
			     Args && ...args)
	{
		for (auto &a:arg)
			bind_1(a.first, a.second);

		bind_all(std::forward<Args>(args)...);
	}

	//! Bind a single resultset column by name

	template<typename argType>
	inline void bind_1(const std::string &column_name, argType &&arg)
	{
		bind_1(bind_column_name(column_name),
		       std::forward<argType>(arg));
	}

	//! Helper class for binding a column to a value.

	template<typename argType> class bind_column {

	public:
		//! Bind a column, ignoring the null flag

		static inline void bind_next(statementObj &s,
					     size_t n, argType *arg)
		{
			s.bind_next(n, arg, nullptr);
		}
	};

	//! Bind a single resultset column by column number.

	template<typename argType>
	inline void bind_1(size_t n, argType &arg)
	{
		bind_column<argType>::bind_next(*this, n, &arg);
	}

	//! Bind a blob, by name

	//! Blobs can be passed as rvalues, which the above lvalue template
	//! won't match. We'll explicitly match the fetchblob rvalue here,
	//! and do the right thing.

	template<typename char_type>
	inline void bind_1(const std::string &column_name,
			   const fetchblob<char_type> &blob)
	{
		bind_1(bind_column_name(column_name), blob);
	}

	//! Bind a blob, by number

	template<typename char_type>
	inline void bind_1(size_t n,
			   const fetchblob<char_type> &blob)
	{
		bind_column<fetchblob<char_type>>::bind_next(*this, n, &blob);
	}

	//! No more resultset vectors to bind.
	void bind_vectors_all(size_t rowsize)
	{
	}

	//! End fetch_vectors() parameters with an explicit scroll indication
	inline void bind_vectors_all(size_t rowsize,
				     const fetch_orientation &orientation)
	{
		bind_all(orientation);
	}

	//! Bind a resultset vector column by column number.

	template<typename argType, typename ...Args>
	void bind_vectors_all(size_t rowsize,
			      size_t n, argType &&arg,
			      Args && ...args)
	{
		bind_1_vector(n, rowsize, std::forward<argType>(arg));
		bind_vectors_all(rowsize, std::forward<Args>(args)...);
	}

	//! Bind a resultset vector column by column name.

	template<typename argType, typename ...Args>
	void bind_vectors_all(size_t rowsize,
			      const std::string &column_name,
			      argType &&arg,
			      Args && ...args)
	{
		bind_1_vector(column_name, rowsize,
			      std::forward<argType>(arg));
		bind_vectors_all(rowsize, std::forward<Args>(args)...);
	}

	//! Bind multiple resultset columns

	template<typename keyType, typename argType, typename ...Args>
	void bind_vectors_all(size_t rowsize,
			      std::map<keyType, argType> &arg,
			      Args && ...args)
	{
		for (auto &a:arg)
		{
			bind_1_vector(a.first, rowsize, a.second);
		}

		bind_vectors_all(rowsize, std::forward<Args>(args)...);
	}

	//! Bind a single resultset column vector by name

	template<typename argType>
	void bind_1_vector(const std::string &column_name,
			   size_t rowsize, argType &&arg)
	{
		bind_1_vector_arg(bind_column_name(column_name), rowsize,
				  std::forward<argType>(arg));
	}

	//! Bind a single resultset column by column number.

	template<typename argType>
	void bind_1_vector(size_t n, size_t rowsize, argType &&arg)
	{
		bind_1_vector_arg(n, rowsize, std::forward<argType>(arg));
	}

	//! Bind a single resultset, without using a null indicator
	template<typename argType>
	void bind_1_vector_arg(size_t n, size_t rowsize,
			       std::vector<argType> &arg)
	{
		arg.resize(rowsize);
		bind_next(n, &arg[0], nullptr);
	}

	//! Bind a single resultset, with a null indicator
	template<typename argType>
	void bind_1_vector_arg(size_t n,
			       size_t rowsize,
			       std::pair<std::vector<argType>,
			       std::vector<bitflag>> &arg)
	{
		arg.first.resize(rowsize);
		arg.second.resize(rowsize);

		bind_next(n, &arg.first[0], &arg.second[0]);
	}

	//! Bind a single resultset blob column, without a null indicator

	//! Blobs are not given as vectors, but as output iterator factories.
	//!
	template<typename char_type>
	void bind_1_vector_arg(size_t n, size_t rowsize,
			       const fetchblob<char_type> &arg)
	{
		bind_next(n, &arg, nullptr);
	}

	//! Bind a single resultset blob column, with a null indicator

	//! \overload
	//!
	template<typename char_type>
	void bind_1_vector_arg(size_t n, size_t rowsize,
			       std::pair<fetchblob<char_type>,
			       std::vector<bitflag>> &arg)
	{
		arg.second.resize(rowsize);
		bind_next(n, &arg.first, &arg.second[0]);
	}

	//! Starting to bind columns.

	virtual void clear_binds(size_t row_array_sizeArg)=0;

	//! Map a column name to number.
	virtual size_t bind_column_name(const std::string &name)=0;

	//! Bind the bookmark column
	virtual void bind_bookmarks(bookmark *bookmarks)=0;

	//! Bind a resultset bitflag column
	virtual void bind_next(size_t column_number, bitflag *n, bitflag *nullflag)=0;

	//! Bind a resultset short column
	virtual void bind_next(size_t column_number, short *n, bitflag *nullflag)=0;

	//! Bind a resultset unsigned short column
	virtual void bind_next(size_t column_number, unsigned short *n, bitflag *nullflag)=0;

	//! Bind a resultset int column
	virtual void bind_next(size_t column_number, int *n, bitflag *nullflag)=0;

	//! Bind a resultset unsigned column
	virtual void bind_next(size_t column_number, unsigned *n, bitflag *nullflag)=0;

	//! Bind a resultset long column
	virtual void bind_next(size_t column_number, long *n, bitflag *nullflag)=0;

	//! Bind a resultset long column
	virtual void bind_next(size_t column_number, unsigned long *n, bitflag *nullflag)=0;

	//! Bind a resultset long long column
	virtual void bind_next(size_t column_number, long long *n, bitflag *nullflag)=0;

	//! Bind a resultset unsigned long long column
	virtual void bind_next(size_t column_number, unsigned long long *n, bitflag *nullflag)=0;

	//! Bind a resultset float column
	virtual void bind_next(size_t column_number, float *n, bitflag *nullflag)=0;

	//! Bind a resultset double column
	virtual void bind_next(size_t column_number, double *n, bitflag *nullflag)=0;

	//! Bind a resultset date column
	virtual void bind_next(size_t column_number, ymd *s, bitflag *nullflag)=0;

	//! Bind a resultset time column
	virtual void bind_next(size_t column_number, hms *s, bitflag *nullflag)=0;

	//! Bind a resultset string column
	virtual void bind_next(size_t column_number, std::string *s, bitflag *nullflag)=0;

	//! Bind a character blob
	virtual void bind_next(size_t column_number,
			       const fetchblob<char> *factory,
			       bitflag *nullflag)=0;

	//! Bind a binary blob
	virtual void bind_next(size_t column_number,
			       const fetchblob<unsigned char> *factory,
			       bitflag *nullflag)=0;

	//! Fetch bound columns
	virtual size_t fetch_into()=0;

	////////////////////////////////////////////////////////////////////

	//! Begin processing parameters for execute()

	virtual void begin_execute_params(bitflag *statusvec, size_t nrows)=0;

	//! No more parameters

	//! When the end of the parameter list is reached, the
	//! statement gets executed.
	//!
	//! The process_execute_params() chain left all parameters on the stack
	//! and they are still in scope.
	virtual void process_execute_params(size_t param_number)=0;

	//! The next parameter is a NULL

	template<typename ...otherArgs>
	void process_execute_params(size_t param_number,
				    std::nullptr_t nullvalue,
				    otherArgs &&...args)
	{
		// Handle this as a single NULL character string parameter.

		const char *nullp=nullptr;
		bitflag nullflag=1;

		process_input_parameter(param_number, &nullp, &nullflag, 1, 1);
		process_execute_params(param_number+1,
				       std::forward<otherArgs>(args)...);
	}

	//! Helper class for processing non-NULL execute parameters

	template<typename T> class process_execute_non_null_param;

	//! Helper class for processing parameters given indirectly via a list

	template<typename T> struct process_list_parameter;

	//! Pick next parameter to process.

	template<typename Arg, typename ...otherArgs>
	void process_execute_params(size_t param_number,
				    const Arg &arg,
				    otherArgs &&...args)
	{
		process_execute_non_null_param<Arg>
			::process(*this, param_number, &arg,
				  std::forward<otherArgs>(args)...);
	}

	//! No more vector parameters

	void process_execute_vector_params(size_t param_number)
	{
		process_execute_params(param_number);
	}

	//! Vector parameter, all values are not nulls.

	template<typename Arg, typename ...otherArgs>
	void process_execute_vector_params(size_t param_number,
					   const std::vector<Arg> &arg,
					   otherArgs &&...args)
	{
		process_input_parameter(param_number, &arg[0], nullptr,
					arg.size(), arg.size());
		process_execute_vector_params(param_number+1,
					      std::forward<otherArgs>(args)...);
	}

	//! Vector parameter, with a separate null vector

	template<typename Arg, typename ...otherArgs>
	void process_execute_vector_params(size_t param_number,
					   const std::pair<std::vector<Arg>,
					   std::vector<bitflag>> &arg,
					   otherArgs &&...args)
	{
		process_input_parameter(param_number, &arg.first[0],
					&arg.second[0],
					arg.first.size(),
					arg.second.size());
		process_execute_vector_params(param_number+1,
					      std::forward<otherArgs>(args)...);
	}

	//! A list of vector parameters

	template<typename Arg, typename ...otherArgs>
	void process_execute_vector_params(size_t param_number,
					   const std::list<std::vector<Arg>>
					   &arg,
					   otherArgs &&...args)
	{
		for (const auto &a:arg)
		{
			process_input_parameter(param_number, &a[0], nullptr,
						a.size(), a.size());
			++param_number;
		}
		process_execute_vector_params(param_number,
					      std::forward<otherArgs>(args)...);
	}

	//! A list of vector parameters with null indicators

	template<typename Arg, typename ...otherArgs>
	void process_execute_vector_params(size_t param_number,
					   const std::list<std::pair<
					   std::vector<Arg>,
					   std::vector<bitflag>>> &arg,
					   otherArgs &&...args)
	{
		for (const auto &a:arg)
		{
			process_input_parameter(param_number,
						&a.first[0],
						&a.second[0],
						a.first.size(),
						a.second.size());
			++param_number;
		}
		process_execute_vector_params(param_number,
					      std::forward<otherArgs>(args)...);
	}

	//! Process input parameter
	virtual void process_input_parameter(size_t param_number, const bitflag *n, const bitflag *nullflags, size_t nvalues, size_t nnullvalues)=0;

	//! Process input parameter
	virtual void process_input_parameter(size_t param_number, const short *n, const bitflag *nullflags, size_t nvalues, size_t nnullvalues)=0;

	//! Process input parameter
	virtual void process_input_parameter(size_t param_number, const unsigned short *n, const bitflag *nullflags, size_t nvalues, size_t nnullvalues)=0;

	//! Process input parameter
	virtual void process_input_parameter(size_t param_number, const int *n, const bitflag *nullflags, size_t nvalues, size_t nnullvalues)=0;

	//! Process input parameter
	virtual void process_input_parameter(size_t param_number, const unsigned *n, const bitflag *nullflags, size_t nvalues, size_t nnullvalues)=0;

	//! Process input parameter
	virtual void process_input_parameter(size_t param_number, const long *n, const bitflag *nullflags, size_t nvalues, size_t nnullvalues)=0;

	//! Process input parameter
	virtual void process_input_parameter(size_t param_number, const unsigned long *n, const bitflag *nullflags, size_t nvalues, size_t nnullvalues)=0;

	//! Process input parameter
	virtual void process_input_parameter(size_t param_number, const long long *n, const bitflag *nullflags, size_t nvalues, size_t nnullvalues)=0;

	//! Process input parameter
	virtual void process_input_parameter(size_t param_number, const unsigned long long *n, const bitflag *nullflags, size_t nvalues, size_t nnullvalues)=0;

	//! Process input parameter
	virtual void process_input_parameter(size_t param_number, const float *n, const bitflag *nullflags, size_t nvalues, size_t nnullvalues)=0;

	//! Process input parameter
	virtual void process_input_parameter(size_t param_number, const double *n, const bitflag *nullflags, size_t nvalues, size_t nnullvalues)=0;

	//! Process input parameter
	virtual void process_input_parameter(size_t param_number, const ymd *s, const bitflag *nullflags, size_t nvalues, size_t nnullvalues)=0;

	//! Process input parameter
	virtual void process_input_parameter(size_t param_number, const hms *s, const bitflag *nullflags, size_t nvalues, size_t nnullvalues)=0;

	//! Process input parameter
	virtual void process_input_parameter(size_t param_number, const std::string *s, const bitflag *nullflags, size_t nvalues, size_t nnullvalues)=0;

	//! Process input parameter
	virtual void process_input_parameter(size_t param_number, const char **s, const bitflag *nullflags, size_t nvalues, size_t nnullvalues)=0;

	//! Process a literal string for an input parameter.

	template<size_t n>
	void process_input_parameter(size_t param_number,
				     const char (*array)[n],
				     const bitflag *nullflags,
				     size_t nvalues,
				     size_t nnullvalues)
	{
		const char *p=&**array;

		process_input_parameter(param_number, &p, nullflags,
					nvalues, nnullvalues);
	}

	//! Process input parameter
	virtual void process_input_parameter(size_t param_number, const insertblob *blobs, const bitflag *nullflags, size_t nvalues, size_t nnullvalues)=0;

	//! Some drivers do not throw an exception on a partial vector insert failure
	virtual void verify_vector_execute(const std::vector<bitflag> &retvalues)=0;
};

//! Specialization

//! Process fetch() arguments when the first one is a bookmark.

template<>
class statementObj::fetch_first_arg<bookmark &> {

public:

	//! Special processing for the bookmark column

	template<typename ...Args>
	static inline void bind_all(statementObj &stmt,
				    bookmark &bookmarkArg,
				    Args && ...args)
	{
		stmt.bind_bookmarks(&bookmarkArg);
		stmt.bind_all(std::forward<Args>(args)...);
	}
};

//! Specialization

//! Process fetch_vectors() arguments when the first one is a bookmark.

template<>
class statementObj::fetch_vectors_first_arg<std::vector<bookmark> &> {

public:

	//! Special processing for the bookmark column

	template<typename ...Args>
	static inline void bind_vectors_all(statementObj &stmt,
					    size_t rowsize,
					    std::vector<bookmark> &bookmarkArg,
					    Args && ...args)
	{
		bookmarkArg.resize(rowsize);
		stmt.bind_bookmarks(&bookmarkArg[0]);
		stmt.bind_vectors_all(rowsize,
				      std::forward<Args>(args)...);
	}
};

//! Process an execute() parameter that's not a NULL

template<typename T> class statementObj::process_execute_non_null_param {

public:

	//! This is the default implementation, the parameter is not a list.

	template<typename ...OtherArgs>
	static void process(statementObj &s,
			    size_t param_number,
			    const T *parameter,
			    OtherArgs && ...otherArgs)
	{
		s.process_input_parameter(param_number, parameter,
					  nullptr, 1, 1);

		// Onto the next parameter.

		s.process_execute_params(param_number+1,
					 std::forward<OtherArgs>
					 (otherArgs)...);
	}
};

//! Specialization

template<typename argType>
class statementObj::bind_column<std::pair<argType, bitflag>> {

 public:
	//! Bind a column, and a null flag

	static inline void bind_next(statementObj &s,
				     size_t n,
				     std::pair<argType, bitflag> *arg)
	{
		s.bind_next(n, &arg->first, &arg->second);
	}
};

//! Specialization, binding a blob without a null flag

//! We need this specialization to accept a blob that gets passed directly
//! as an argument to fetch(), so it is a const.

template<typename char_type>
class statementObj::bind_column<fetchblob<char_type>> {

 public:

	//! Bind the blob column

	static inline void bind_next(statementObj &s,
				     size_t n,
				     const fetchblob<char_type> *arg)
	{
		s.bind_next(n, arg, nullptr);
	}
};

//! Specialization

//! The parameter is a list. Invoke process_list_parameter to process it
//! further.

template<typename T>
class statementObj::process_execute_non_null_param<std::list<T>> {

 public:

	//! Process a std::list that's passed as a parameter to
	//! execute().

	template<typename ...OtherArgs>
		static void process(statementObj &s,
				    size_t param_number,
				    const std::list<T> *parameters,
				    OtherArgs && ...otherArgs)
	{
		param_number=process_list_parameter<T>
			::process(s, param_number, parameters);

		// Onto the next parameter.

		s.process_execute_params(param_number,
					 std::forward<otherArgs>(otherArgs)...);
	}
};

//! Helper class for processing parameters given indirectly via a list

template<typename T> class statementObj::process_list_parameter {
public:

	//! Process each value in the vector as a parameter
	static size_t process(statementObj &s,
			      size_t param_number,
			      const std::list<T> *p)
	{
		for (auto b=p->begin(), e=p->end();
		     b != e; ++b)
		{
			s.process_input_parameter(param_number, &*b,
						  nullptr, 1, 1);
			++param_number;
		}
		return param_number;
	}
};

//! Specialization

//! Pass a vector of std::pair<type, bool>. If the bool value is true, this
//! specifies a NULL value for the input parameter, otherwise it's type.first.

template<typename valueType>
struct statementObj::process_list_parameter<std::pair<valueType, bitflag>> {

 public:

	//! Process a vector of value/bool flag tuples.

	static size_t process(statementObj &s,
			      size_t param_number,
			      const std::list<std::pair<valueType, bitflag>> *p)
	{
		for (auto b=p->begin(), e=p->end(); b != e; ++b)
		{
			s.process_input_parameter(param_number,
						  &b->first,
						  &b->second, 1, 1);
			++param_number;
		}
		return param_number;
	}
};

//! Undefined/unimplemented. Use std::string<std::vector>

template<typename param_type, typename ...otherArgs>
void process_execute_params(size_t param_number,
			    const std::vector<const char *> &parameters,
			    otherArgs &&...args);

//! Undefined/unimplemented. Use std::string<std::vector>

template<typename param_type, typename ...otherArgs>
void process_execute_params(size_t param_number,
			    const std::vector<char *> &parameters,
			    otherArgs &&...args);

#if 0
{
	{
#endif
	}
}
#endif
