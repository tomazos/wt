#pragma once

#include "IPPTokenStream.h"

#include "ETokenType.h"
#include "IPostTokenOutputStream.h"
#include "Tokens.h"
#include "Tokenizer.h"
#include "IPPTokenStream.h"
#include "CtrlExprParser.h"

// mock implementation of IsDefinedIdentifier for PA3
// return true iff first code point is odd
inline bool PA3Mock_IsDefinedIdentifier(const string& identifier)
{
	if (identifier.empty())
		return false;
	else
		return identifier[0] % 2;
}

struct CtrlExprInputStream : IPPTokenStream
{
	vector<PToken> tokens;
	bool has_error = false;

	void error(const string& message)
	{
		cerr << "ERROR: " << message << endl;
		has_error = true;
	}

	void emit_whitespace_sequence() {}

	virtual void emit_new_line()
	{
		if (has_error)
			cout << "error" << endl;
		else
		{
			try
			{
				if (!tokens.empty())
				{
					CtrlExprParser parser(tokens, PA3Mock_IsDefinedIdentifier);

					CtrlExprVal val = parser.parse();

					val.dump();
				}
			}
			catch (exception& e)
			{
				error(e.what());
				cout << "error" << endl;
			}
		}

		tokens.clear();
		has_error = false;
	}

	virtual void emit_header_name(SourceInfo)
	{
		error("header-name in controlling expression");
	}

	virtual void emit_non_whitespace_char(SourceInfo)
	{
		error("non-whitespace-char in controlling expression");
	}

	virtual void emit_identifier(SourceInfo src)
	{
		tokens.push_back(make_shared<IdentifierToken>(src));
	}

	virtual void emit_preprocessing_op_or_punc(SourceInfo src)
	{
		ETokenType tt = PPTokenToSimpleTokenType(src.spelling);

		if (tt == TT_INVALID)
			error("unexpected operator in controlling expression");
		else
			tokens.push_back(make_shared<Token>(src, tt));
	}

	virtual void emit_pp_number(SourceInfo src)
	{
		try
		{
			PToken token = TokenizePPNumber(src);
			switch (token->token_type)
			{
			case TT_LITERAL:
			{
				tokens.push_back(token);
			}
			break;

			case TT_UDINTEGER:
			case TT_UDFLOATING:
			{
				error("user-defined-literal in controlling expression");
			}
			break;

			default:
				error("unknown pp-number in controlling expression");
			}
		}
		catch (exception& e)
		{
			error(e.what());
		}
	}

	virtual void emit_character_literal(SourceInfo src)
	{
		try
		{
			tokens.push_back(TokenizeCharacterLiteral(src));
		}
		catch (exception& e)
		{
			error(e.what());
		}
	}

	virtual void emit_user_defined_character_literal(SourceInfo)
	{
		error("user-defined-literal in controlling expression");
	}


	virtual void emit_string_literal(SourceInfo)
	{
		error("string-literal in controlling expression");
	}

	virtual void emit_user_defined_string_literal(SourceInfo)
	{
		error("user-defined-literal in controlling expression");
	}

	virtual void emit_eof()
	{
		cout << "eof" << endl;
	}
};
