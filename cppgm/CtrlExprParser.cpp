#include "std.pch"

#include "CtrlExprParser.h"
#include "Tokenizer.h"

vector<PToken> CtrlExpr_ConvertPreprocessingTokens(const vector<PreprocessingToken>& input)
{
	vector<PToken> output;

	for (const PreprocessingToken& pptoken : input)
	{
		switch (pptoken.pp_type)
		{
		case PP_WHITESPACE:
		case PP_NEW_LINE:
			break;

		case PP_INVALID:
		case PP_HEADER_NAME:
		case PP_STRING_LITERAL:
		case PP_USER_DEFINED_STRING_LITERAL:
		case PP_NONWHITESPACE_CHAR:
		case PP_PLACEMARKER:
		case PP_EOF:
		case PP_USER_DEFINED_CHARACTER_LITERAL:
			throw logic_error("invalid token in controlling expression: " + pptoken.src.spelling);

		case PP_IDENTIFIER:
			output.push_back(make_shared<IdentifierToken>(pptoken.src));
			break;

		case PP_NUMBER:
			{
				PToken token = TokenizePPNumber(pptoken.src);
				if (token->token_type == TT_LITERAL)
					output.push_back(token);
				else
					throw logic_error("invalid token in controlling expression: " + pptoken.src.spelling);
			}
			break;

		case PP_CHARACTER_LITERAL:
			output.push_back(TokenizeCharacterLiteral(pptoken.src));
			break;

		case PP_OP_OR_PUNC:
			{
				ETokenType tt = PPTokenToSimpleTokenType(pptoken.src.spelling);

				if (tt == TT_INVALID)
					throw logic_error("unexpected operator in controlling expression: " + pptoken.src.spelling);
				else
					output.push_back(make_shared<Token>(pptoken.src, tt));
			}
			break;

		default:
			throw logic_error("internal error, unexpected token in ctrlexpr: " + pptoken.src.spelling);
		}
	}

	return output;
}

CtrlExprVal ValFromPpNumber(const PreprocessingToken& pptoken)
{
	if (pptoken.pp_type != PP_NUMBER)
		throw logic_error("expected ppnumber: " + pptoken.src.spelling);

	PToken token = TokenizePPNumber(pptoken.src);
	if (token->token_type != TT_LITERAL)
		throw logic_error("expected literal: " + pptoken.src.spelling);

	LiteralToken* literal = dynamic_cast<LiteralToken*>(token.get());
	if (!literal)
		throw logic_error("internal error: " + pptoken.src.spelling);

	return CtrlExprVal(*literal);
}

string StrFromStringLiteral(SourceInfo src)
{
	PToken token = TokenizeStringLiteral(src);

	if (token->token_type != TT_LITERAL)
		throw logic_error("expected literal: " + src.spelling);

	LiteralToken* literal = dynamic_cast<LiteralToken*>(token.get());
	if (!literal)
		throw logic_error("internal error: " + src.spelling);

	if (!literal->is_array || literal->fundamental_type != FT_CHAR)
		throw logic_error("expected UTF-8 string");

	return string((const char*) literal->data.data());
}
