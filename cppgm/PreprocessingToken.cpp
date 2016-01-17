#include "std.pch"

#include "PreprocessingToken.h"

void WritePPTokens(const vector<PreprocessingToken>& tokens, IPPTokenStream& output)
{
	for (const PreprocessingToken& token : tokens)
	{
		switch (token.pp_type)
		{
		case PP_INVALID: throw logic_error("invalid token output by preprocessor");

		case PP_WHITESPACE: output.emit_whitespace_sequence(); break;

		case PP_NEW_LINE: output.emit_new_line(); break;

		case PP_HEADER_NAME: output.emit_header_name(token.src); break;

		case PP_IDENTIFIER: output.emit_identifier(token.src); break;

		case PP_NUMBER: output.emit_pp_number(token.src); break;

		case PP_CHARACTER_LITERAL: output.emit_character_literal(token.src); break;

		case PP_USER_DEFINED_CHARACTER_LITERAL: output.emit_user_defined_character_literal(token.src); break;

		case PP_STRING_LITERAL: output.emit_string_literal(token.src); break;

		case PP_USER_DEFINED_STRING_LITERAL: output.emit_user_defined_string_literal(token.src); break;

		case PP_OP_OR_PUNC: output.emit_preprocessing_op_or_punc(token.src); break;

		case PP_NONWHITESPACE_CHAR: output.emit_non_whitespace_char(token.src); break;

		case PP_EOF: output.emit_eof(); break;

		default: throw logic_error("internal error: unknown token.pp_type");
		}
	}
}

void NormalizeWhitespace(vector<PreprocessingToken>& input)
{
	vector<PreprocessingToken> output;

	for (const PreprocessingToken& token : input)
	{
		switch (token.pp_type)
		{
		case PP_PLACEMARKER:
			continue;

		case PP_WHITESPACE:
		case PP_NEW_LINE:
			if (output.size() > 0 && output.back().pp_type != PP_WHITESPACE)
				output.push_back(PreprocessingToken(SourceInfo(), PP_WHITESPACE));
			break;

		case PP_INVALID:
			throw logic_error("invalid token: " + token.src.spelling);

		default:

			output.push_back(token);
		}
	}

	if (output.size() > 0 && output.back().pp_type == PP_WHITESPACE)
		output.pop_back();

	swap(input,output);
}

void CheckVarargs(const vector<PreprocessingToken>& input)
{
	for (const PreprocessingToken& token : input)
		if (token.is_varargs())
			throw logic_error("__VA_ARGS__ token in text-lines: " + token.src.spelling);
}

string PpTokensToStr(const vector<PreprocessingToken>& input)
{
	ostringstream oss;

	for (auto x : input)
		oss << x.to_string() << " ";

	string s = oss.str();

	if (!s.empty())
		s.pop_back();

	return s;
}
