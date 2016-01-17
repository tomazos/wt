#pragma once

struct TokenSequencePostTokenOutputStream : IPostTokenOutputStream
{
	vector<PToken> tokens;

	virtual void emit_invalid(SourceInfo src)
	{
		throw logic_error("invalid token: " + src.spelling);
	}

	virtual void emit_simple(SourceInfo src, ETokenType token_type)
	{
		tokens.push_back(make_shared<Token>(src, token_type));
	}

	virtual void emit_identifier(SourceInfo src)
	{
		tokens.push_back(make_shared<IdentifierToken>(src));
	}

	virtual void emit_literal(SourceInfo src, EFundamentalType type, void* data, size_t nbytes)
	{
		tokens.push_back(make_shared<LiteralToken>(src, type, data, nbytes));
	}

	virtual void emit_literal_array(SourceInfo src, size_t num_elements, EFundamentalType type, void* data, size_t nbytes)
	{
		tokens.push_back(make_shared<LiteralToken>(src, num_elements, type, data, nbytes));
	}

	virtual void emit_user_defined_literal_character(SourceInfo src, const string& ud_suffix, EFundamentalType type, void* data, size_t nbytes)
	{
		tokens.push_back(make_shared<UserDefinedCharacterLiteralToken>(src, ud_suffix, type, data, nbytes));
	}

	virtual void emit_user_defined_literal_string_array(SourceInfo src, const string& ud_suffix, size_t num_elements, EFundamentalType type, void* data, size_t nbytes)
	{
		tokens.push_back(make_shared<UserDefinedStringLiteralToken>(src, ud_suffix, num_elements, type, data, nbytes));
	}

	virtual void emit_user_defined_literal_integer(SourceInfo src, const string& ud_suffix, const string& prefix)
	{
		tokens.push_back(make_shared<UserDefinedIntegerLiteralToken>(src, ud_suffix, prefix));
	}

	virtual void emit_user_defined_literal_floating(SourceInfo src, const string& ud_suffix, const string& prefix)
	{
		tokens.push_back(make_shared<UserDefinedFloatingLiteralToken>(src, ud_suffix, prefix));
	}

	virtual void emit_eof()
	{
		tokens.push_back(make_shared<Token>(SourceInfo(), TT_EOF));
	}
};
