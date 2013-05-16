/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "sql_internal.H"
#include "gettext_in.h"
#include "x/sql/insertblob.H"

namespace LIBCXX_NAMESPACE {
	namespace sql {
#if 0
	}
};
#endif

insertblobObj::insertblobObj()
{
}

insertblobObj::~insertblobObj() noexcept
{
}

template<typename char_type>
insertblobdatatypeObj<char_type>::insertblobdatatypeObj()
{
}

template<typename char_type>
insertblobdatatypeObj<char_type>::~insertblobdatatypeObj() noexcept
{
}

template class insertblobdatatypeObj<char>;
template class insertblobdatatypeObj<unsigned char>;

template<>
int insertblobdatatypeObj<char>::datatype()
{
	return SQL_C_CHAR;
}

template<>
int insertblobdatatypeObj<unsigned char>::datatype()
{
	return SQL_C_BINARY;
}

template<>
int insertblobdatatypeObj<char>::sqltype()
{
	return SQL_LONGVARCHAR;
}

template<>
int insertblobdatatypeObj<unsigned char>::sqltype()
{
	return SQL_LONGVARBINARY;
}

ref<insertblobObj> insertblobBase::createEmpty()
{
	return insertblob::create( (const char *)nullptr,
				   (const char *)nullptr );
}

void insertblobObj::blobtoobig()
{
	throw EXCEPTION(_TXT(_txt("Blob size exceeds maximum limit")));
}

#if 0
{
	{
#endif
	};
};
