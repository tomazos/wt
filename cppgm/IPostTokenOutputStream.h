#pragma once

#include "ETokenType.h"
#include "EFundamentalType.h"
#include "IPPTokenStream.h"

struct IPostTokenOutputStream
{
	virtual void emit_invalid(SourceInfo src) = 0;
	// output: invalid <source>

	virtual void emit_simple(SourceInfo src, ETokenType token_type) = 0;
	// output: simple <source> <token_type>

	virtual void emit_identifier(SourceInfo src) = 0;
	// output: identifier <source>

	virtual void emit_literal(SourceInfo src, EFundamentalType type, void* data, size_t nbytes) = 0;
	// output: literal <source> <type> <hexdump(data,nbytes)>

	virtual void emit_literal_array(SourceInfo src, size_t num_elements, EFundamentalType type, void* data, size_t nbytes) = 0;
	// output: literal <source> array of <num_elements> <type> <hexdump(data,nbytes)>

	virtual void emit_user_defined_literal_character(SourceInfo src, const string& ud_suffix, EFundamentalType type, void* data, size_t nbytes) = 0;
	// output: user-defined-literal <source> <ud_suffix> character <type> <hexdump(data,nbytes)>

	virtual void emit_user_defined_literal_string_array(SourceInfo src, const string& ud_suffix, size_t num_elements, EFundamentalType type, void* data, size_t nbytes) = 0;
	// output: user-defined-literal <source> <ud_suffix> string array of <num_elements> <type> <hexdump(data, nbytes)>

	virtual void emit_user_defined_literal_integer(SourceInfo src, const string& ud_suffix, const string& prefix) = 0;
	// output: user-defined-literal <source> <ud_suffix> <prefix>

	virtual void emit_user_defined_literal_floating(SourceInfo src, const string& ud_suffix, const string& prefix) = 0;
	// output: user-defined-literal <source> <ud_suffix> <prefix>

	virtual void emit_eof() = 0;
	// output : eof

	virtual ~IPostTokenOutputStream() {}
};
