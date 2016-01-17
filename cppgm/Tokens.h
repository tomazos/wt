#pragma once

#include "ETokenType.h"
#include "EFundamentalType.h"
#include "IPPTokenStream.h"

struct Token;
typedef shared_ptr<Token> PToken;

struct Type;

struct Token
{
	Token(SourceInfo src, ETokenType token_type)
		: token_type(token_type)
		, src(src)
	{}

	ETokenType token_type;
	SourceInfo src;

	string token_type_str() const
	{
		return SimpleTokenTypeToString(token_type);
	}

	virtual const char* as_ordinary_string()
	{
		throw logic_error("expected ordinary string token");
	}

	virtual void write(ostream& o) const
	{
		o << token_type_str();
	}

	Type* get_type() { throw logic_error("interal error, invalid token to get type"); }

	virtual ~Token() {};
};

struct IdentifierToken : Token
{
	IdentifierToken(SourceInfo src)
		: Token(src, TT_IDENTIFIER)
	{}

	virtual void write(ostream& o) const
	{
		o << token_type_str() << ":" << src.spelling;
	}
};

struct LiteralToken : Token
{
	LiteralToken(SourceInfo src, EFundamentalType fundamental_type, const void* pdata, size_t nbytes, ETokenType token_type = TT_LITERAL)
		: Token(src, token_type)
		, fundamental_type(fundamental_type)
		, is_array(false)
		, num_elements(1)
	{
		data.resize(nbytes);
		memcpy(&data[0], pdata, nbytes);
	}

	LiteralToken(SourceInfo src, size_t num_elements, EFundamentalType fundamental_type, const void* pdata, size_t nbytes, ETokenType token_type = TT_LITERAL)
		: Token(src, token_type)
		, fundamental_type(fundamental_type)
		, is_array(true)
		, num_elements(num_elements)
	{
		data.resize(nbytes);
		memcpy(&data[0], pdata, nbytes);
	}

	template<class T> T get() const
	{
		T t;

		if (sizeof(t) != data.size())
				throw logic_error("internal literal error (#1)");
		memcpy(&t, data.data(), data.size());

		return t;
	}

	virtual void write(ostream& o) const
	{
		o << token_type_str() << ":" << src.spelling;
	}

	virtual const char* as_ordinary_string()
	{
		if (!is_array)
			throw logic_error("expected ordinary string token (!is_array)");

		if (fundamental_type != FT_CHAR)
			throw logic_error("expected ordinary string token (!FT_CHAR)");

		return (const char*) &(data[0]);
	}

	EFundamentalType fundamental_type;
	bool is_array;
	size_t num_elements;
	vector<byte> data;

	Type* get_type();
};

struct UserDefinedStringLiteralToken : LiteralToken
{
	UserDefinedStringLiteralToken(SourceInfo src, const string& ud_suffix, size_t num_elements, EFundamentalType fundamental_type, const void* pdata, size_t nbytes)
		: LiteralToken(src, num_elements, fundamental_type, pdata, nbytes, TT_UDSTRING)
		, ud_suffix(ud_suffix)
	{}

	string ud_suffix;
};

struct UserDefinedCharacterLiteralToken : LiteralToken
{
	UserDefinedCharacterLiteralToken(SourceInfo src, const string& ud_suffix, EFundamentalType fundamental_type, const void* pdata, size_t nbytes)
		: LiteralToken(src, fundamental_type, pdata, nbytes, TT_UDCHARACTER)
		, ud_suffix(ud_suffix)
	{}

	string ud_suffix;
};

struct UserDefinedIntegerLiteralToken : Token
{
	UserDefinedIntegerLiteralToken(SourceInfo src, const string& ud_suffix, const string& source)
		: Token(src, TT_UDINTEGER)
		, ud_suffix(ud_suffix)
		, source(source)
	{}

	string ud_suffix;
	string source;
};

struct UserDefinedFloatingLiteralToken : Token
{
	UserDefinedFloatingLiteralToken(SourceInfo src, const string& ud_suffix, const string& source)
		: Token(src, TT_UDFLOATING)
		, ud_suffix(ud_suffix)
		, source(source)
	{}

	string ud_suffix;
	string source;
};
