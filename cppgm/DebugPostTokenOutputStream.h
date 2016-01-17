#pragma once

#include "IPostTokenOutputStream.h"

#include "Text.h"

struct DebugPostTokenOutputStream : IPostTokenOutputStream
{
	virtual void emit_invalid(SourceInfo src)
	{
		cout << "invalid " << src.spelling << endl;
	}

	virtual void emit_simple(SourceInfo src, ETokenType token_type)
	{
		cout << "simple " << src.spelling << " " << SimpleTokenTypeToString(token_type) << endl;
	}

	virtual void emit_identifier(SourceInfo src)
	{
		cout << "identifier " << src.spelling << endl;
	}

	virtual void emit_literal(SourceInfo src, EFundamentalType type, void* data, size_t nbytes)
	{
		cout << "literal " << src.spelling << " " << FundamentalTypeToString(type) << " " << HexDump(data, nbytes) << endl;
	}

	virtual void emit_literal_array(SourceInfo src, size_t num_elements, EFundamentalType type, void* data, size_t nbytes)
	{
		cout << "literal " << src.spelling << " array of " << num_elements << " " << FundamentalTypeToString(type) << " " << HexDump(data, nbytes) << endl;
	}

	virtual void emit_user_defined_literal_character(SourceInfo src, const string& ud_suffix, EFundamentalType type, void* data, size_t nbytes)
	{
		cout << "user-defined-literal " << src.spelling << " " << ud_suffix << " character " << FundamentalTypeToString(type) << " " << HexDump(data, nbytes) << endl;
	}

	virtual void emit_user_defined_literal_string_array(SourceInfo src, const string& ud_suffix, size_t num_elements, EFundamentalType type, void* data, size_t nbytes)
	{
		cout << "user-defined-literal " << src.spelling << " " << ud_suffix << " string array of " << num_elements << " " << FundamentalTypeToString(type) << " " << HexDump(data, nbytes) << endl;
	}

	virtual void emit_user_defined_literal_integer(SourceInfo src, const string& ud_suffix, const string& prefix)
	{
		cout << "user-defined-literal " << src.spelling << " " << ud_suffix << " integer " << prefix << endl;
	}

	virtual void emit_user_defined_literal_floating(SourceInfo src, const string& ud_suffix, const string& prefix)
	{
		cout << "user-defined-literal " << src.spelling << " " << ud_suffix << " floating " << prefix << endl;
	}

	virtual void emit_eof()
	{
		cout << "eof" << endl;
	}
};
