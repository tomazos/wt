#pragma once

#include "IPPTokenStream.h"
#include "SourcePositionTracker.h"
#include "Tokens.h"

enum EPreprocessingTokenType
{
	PP_INVALID,
	PP_WHITESPACE,
	PP_NEW_LINE,
	PP_HEADER_NAME,
	PP_IDENTIFIER,
	PP_NUMBER,
	PP_CHARACTER_LITERAL,
	PP_USER_DEFINED_CHARACTER_LITERAL,
	PP_STRING_LITERAL,
	PP_USER_DEFINED_STRING_LITERAL,
	PP_OP_OR_PUNC,
	PP_NONWHITESPACE_CHAR,
	PP_PLACEMARKER,
	PP_EOF
};

inline string EPreprocessingTokenTypeToString(EPreprocessingTokenType t)
{
	switch (t)
	{
	case PP_INVALID: return "PP_INVALID";
	case PP_WHITESPACE: return "PP_WHITESPACE";
	case PP_NEW_LINE: return "PP_NEW_LINE";
	case PP_HEADER_NAME: return "PP_HEADER_NAME";
	case PP_IDENTIFIER: return "PP_IDENTIFIER";
	case PP_NUMBER: return "PP_NUMBER";
	case PP_CHARACTER_LITERAL: return "PP_CHARACTER_LITERAL";
	case PP_USER_DEFINED_CHARACTER_LITERAL: return "PP_USER_DEFINED_CHARACTER_LITERAL";
	case PP_STRING_LITERAL: return "PP_STRING_LITERAL";
	case PP_USER_DEFINED_STRING_LITERAL: return "PP_USER_DEFINED_STRING_LITERAL";
	case PP_OP_OR_PUNC: return "PP_OP_OR_PUNC";
	case PP_NONWHITESPACE_CHAR: return "PP_NONWHITESPACE_CHAR";
	case PP_PLACEMARKER: return "PP_PLACEMARKER";
	case PP_EOF: return "PP_EOF";
	default: throw logic_error("unknown EPreprocessingTokenType");
	}
}

struct PreprocessingToken
{
	PreprocessingToken()
		: pp_type(PP_INVALID)
	{}

	PreprocessingToken(SourceInfo src, EPreprocessingTokenType pp_type)
		: src(src)
		, pp_type(pp_type)
	{}

	SourceInfo src;

	EPreprocessingTokenType pp_type;
	unordered_set<string> blacklist;
	bool available = true;
	bool blessed = false;

	bool operator==(const PreprocessingToken& that) const { return pp_type == that.pp_type && src.spelling == that.src.spelling; }
	bool operator!=(const PreprocessingToken& that) const { return pp_type == that.pp_type && src.spelling != that.src.spelling; }

	bool is_identifier(const string& id) const { return pp_type == PP_IDENTIFIER && src.spelling == id; }
	bool is_varargs() const { return pp_type == PP_IDENTIFIER && src.spelling == "__VA_ARGS__"; }
	bool is_hash() const { return pp_type == PP_OP_OR_PUNC && (src.spelling == "#" || src.spelling == "%:"); }
	bool is_hash2_blessed() const { return blessed && pp_type == PP_OP_OR_PUNC && (src.spelling == "##" || src.spelling == "%:%:"); }
	bool is_hash2_unblessed() const { return !blessed && pp_type == PP_OP_OR_PUNC && (src.spelling == "##" || src.spelling == "%:%:"); }
	bool is_lparen() const { return pp_type == PP_OP_OR_PUNC && src.spelling == "("; }
	bool is_rparen() const { return pp_type == PP_OP_OR_PUNC && src.spelling == ")"; }
	bool is_comma() const { return pp_type == PP_OP_OR_PUNC && src.spelling == ","; }
	bool is_dots() const { return pp_type == PP_OP_OR_PUNC && src.spelling == "..."; }
	bool is_valid() const { return pp_type != PP_INVALID; }
	bool is_ws() const { return pp_type == PP_WHITESPACE; }
	bool is_placemarker() const { return pp_type == PP_PLACEMARKER; }

	string to_string() const
	{
		ostringstream oss;

		oss << EPreprocessingTokenTypeToString(pp_type) << "(" << src.spelling << ")" << (blessed ? "*" : "");

		return oss.str();
	}
};

struct PreprocesingTokenPPTokenStream :  IPPTokenStream
{
	vector<PreprocessingToken> tokens;

	void emit_whitespace_sequence()
	{
		tokens.push_back(PreprocessingToken(SourceInfo(), PP_WHITESPACE));
	}

	void emit_new_line()
	{
		tokens.push_back(PreprocessingToken(SourceInfo(), PP_NEW_LINE));
	}

	void emit_header_name(SourceInfo src)
	{
		tokens.push_back(PreprocessingToken(src, PP_HEADER_NAME));
	}

	void emit_identifier(SourceInfo src)
	{
		tokens.push_back(PreprocessingToken(src, PP_IDENTIFIER));
	}

	void emit_pp_number(SourceInfo src)
	{
		tokens.push_back(PreprocessingToken(src, PP_NUMBER));
	}

	void emit_character_literal(SourceInfo src)
	{
		tokens.push_back(PreprocessingToken(src, PP_CHARACTER_LITERAL));
	}

	void emit_user_defined_character_literal(SourceInfo src)
	{
		tokens.push_back(PreprocessingToken(src, PP_USER_DEFINED_CHARACTER_LITERAL));
	}

	void emit_string_literal(SourceInfo src)
	{
		tokens.push_back(PreprocessingToken(src, PP_STRING_LITERAL));
	}

	void emit_user_defined_string_literal(SourceInfo src)
	{
		tokens.push_back(PreprocessingToken(src, PP_USER_DEFINED_STRING_LITERAL));
	}

	void emit_preprocessing_op_or_punc(SourceInfo src)
	{
		tokens.push_back(PreprocessingToken(src, PP_OP_OR_PUNC));
	}

	void emit_non_whitespace_char(SourceInfo src)
	{
		tokens.push_back(PreprocessingToken(src, PP_NONWHITESPACE_CHAR));
	}

	void emit_eof()
	{
		tokens.push_back(PreprocessingToken(SourceInfo(), PP_EOF));
	}
};

void NormalizeWhitespace(vector<PreprocessingToken>& input);
void CheckVarargs(const vector<PreprocessingToken>& input);
void WritePPTokens(const vector<PreprocessingToken>& tokens, IPPTokenStream& output);

string PpTokensToStr(const vector<PreprocessingToken>& input);
