#include "std.pch"

#include "ToString.h"

#include "CY86AsmParser.h"
#include "CY86Instruction.h"
#include "ElfHeader.h"

struct CY86AsmParser
{
	vector<PToken> tokens;
	size_t pos = 0;

	vector<pair<string,size_t>> parse_stack;

	size_t deepest_pos = 0;
	vector<pair<string,size_t>> deepest_parse_stack;

	bool in_angle = false;

	void update_deepest(size_t x)
	{
		if (x > deepest_pos)
		{
			deepest_pos = x;
			deepest_parse_stack = parse_stack;
		}
	}

	void enter_parse_stack(const string& function)
	{
		parse_stack.push_back(make_pair(function, pos));
	}

	void exit_parse_stack()
	{
		parse_stack.pop_back();
	}

	struct ParseFunction
	{
		CY86AsmParser* parser;
		size_t start;
		string function;
		bool failed = false;

		ParseFunction(CY86AsmParser* parser, const string& function)
			: parser(parser)
			, function(function)
		{
//			cerr << "DEBUG: > " + function << " " << ToString(*parser->peekt()) << endl;

			parser->enter_parse_stack(function);
			start = parser->pos;
		}

		~ParseFunction()
		{
//			if (failed)
//				cerr << "DEBUG: F " + function << " " << ToString(*parser->peekt()) << endl;
//			else
//				cerr << "DEBUG: < " + function << " " << ToString(*parser->peekt()) << endl;

			parser->exit_parse_stack();
			unnest();
		}

		struct NullPointer_HACK
		{
			template<class T> operator T*() { volatile decltype(nullptr) np = nullptr; return np; }
			operator bool() { volatile bool b = false; return b;}
		};

		NullPointer_HACK fail()
		{
			parser->jump(start);

			failed = true;

			return {};
		}

		bool nested = false;
		bool old_angle;

		bool nest(bool in_angle = false)
		{
			if (nested)
				throw logic_error("ParseFunction double nest");

			old_angle = parser->in_angle;
			parser->in_angle = in_angle;
			nested = true;

			return true;
		}

		bool unnest()
		{
			if (nested)
			{
				nested = false;
				parser->in_angle = old_angle;
			}

			return true;
		}
	};

	CY86AsmParser() {}

	void parse(const string& /* srcfile */, vector<PToken>&& tokens_in)
	{
		tokens.clear();

		pos = 0;

		parse_stack.clear();

		deepest_pos = 0;
		deepest_parse_stack.clear();

		in_angle = false;

		for (const PToken& token : tokens_in)
		{
			if (token->token_type != OP_RSHIFT)
				tokens.push_back(token);
			else
			{
				tokens.push_back(make_shared<Token>(token->src, ST_RSHIFT_1));
				tokens.push_back(make_shared<Token>(token->src, ST_RSHIFT_2));
			}
		}

		parse_program();
	}

	void dump_tokens()
	{
		cout << "DEBUG: " << tokens.size() << " tokens:" << endl;
		for (auto token : tokens)
		{
			token->write(cout);
			cout << endl;
		}
		cout << endl;
	}

	void dump_deepest()
	{
		cout << "DEBUG: deepest call stack:" << endl;
		for (auto f : deepest_parse_stack)
		{
			cout << "    " << f.first << " " << f.second << " ";
			if (f.second < tokens.size())
				tokens[f.second]->write(cout);
			cout << endl;
		}
		if (deepest_pos >= tokens.size())
			cout << "passed the end";
		else
		{
			cout << "ERROR:";
			cout << tokens[deepest_pos]->src.filename << ":" << tokens[deepest_pos]->src.linenum << ": ";
			cout << "examined ";
			tokens[deepest_pos]->write(cout);
			cout << endl;
		}
	}

	pair<string, size_t> get_deepest()
	{
		if (tokens.empty())
			return make_pair("", 0);
		else
			return make_pair(tokens[deepest_pos]->src.filename, tokens[deepest_pos]->src.linenum);
	}

	PToken peekt(size_t i = 0)
	{
		size_t j = pos+i;

		update_deepest(j);

		if (j >= tokens.size())
		{
			static PToken invalid = make_shared<Token>(SourceInfo(), TT_INVALID);
			return invalid;
		}

		return tokens[j];
	}

	ETokenType peek(size_t i = 0)
	{
		return peekt(i)->token_type;
	}

	bool peek(ETokenType token_type)
	{
		return peek(0, token_type);
	}

	bool peek(size_t i, ETokenType token_type)
	{
		return peek(i) == token_type;
	}

	void drop(size_t i = 1)
	{
		pos += i;
		update_deepest(pos);
	}

	bool drop(ETokenType token_type)
	{
		if (peek(token_type))
		{
			drop();
			return true;
		}
		else
			return false;
	}

	void back(size_t i = 1)
	{
		pos -= i;
	}

	void jump(size_t i)
	{
		pos = i;
	}

	PToken pop()
	{
		PToken token = peekt();
		pos++;
		update_deepest(pos);
		return token;
	}

	bool maybe(ETokenType token_type)
	{
		if (peek(token_type))
		{
			pop();
			return true;
		}
		else
			return false;
	}

    #define DeclParse(x) ParseFunction func(this, #x);

	string parse_identifier()
	{
		if (!peek(TT_IDENTIFIER))
			return "";

		string id = peekt()->src.spelling;

		pop();

		return id;
	}

	// ====================== BEGIN PARSE FUNCTIONS =============================


	CY86Program program;

	void parse_program()
	{
		while (!peek(TT_EOF))
		{
			parse_statement();

			if (!drop(OP_SEMICOLON))
				throw logic_error("expected semicolon");
		}
	}

	void parse_statement()
	{
		if (peek(1, OP_COLON))
		{
			string label = parse_identifier();

			if (label.empty())
				throw logic_error("expected label");

			add_label(label);

			drop(1);

			parse_statement();
		}
		else
		{
			if (peek(0,TT_LITERAL) || peek(0, OP_MINUS))
			{
				parse_literal_statement();
			}
			else if (peek(0, TT_IDENTIFIER))
			{
				parse_instruction();
			}
			else
				throw logic_error("unexpected token");
		}
	}

	vector<string> labels;

	void add_label(const string& label)
	{
		labels.push_back(label);
	}

	void parse_literal_statement()
	{
		CY86Literal literal = parse_literal();

		program.add_literal_statement(literal);
		program.add_labels(labels);
		labels.clear();
	}


	CY86Literal parse_literal()
	{
		bool negate = false;

		if (peek(OP_MINUS))
		{
			negate = true;
			drop(1);
		}
		else if (peek(OP_PLUS))
		{
			drop(1);
		}

		if (!peek(TT_LITERAL))
			throw logic_error("expected literal");

		PToken token = peekt();
		drop(1);

		ECY86LiteralType literal_type = YLT_OTHER;

		LiteralToken& literal_token = dynamic_cast<LiteralToken&>(*token);

		if (!literal_token.is_array)
			switch (literal_token.fundamental_type)
			{
			case FT_SIGNED_CHAR:
			case FT_SHORT_INT:
			case FT_INT:
			case FT_LONG_INT:
			case FT_LONG_LONG_INT:
			case FT_WCHAR_T:
			case FT_CHAR:
				literal_type = YLT_SIGNEDINTEGER;
				break;

			case FT_UNSIGNED_CHAR:
			case FT_UNSIGNED_SHORT_INT:
			case FT_UNSIGNED_INT:
			case FT_UNSIGNED_LONG_INT:
			case FT_UNSIGNED_LONG_LONG_INT:
			case FT_CHAR16_T:
			case FT_CHAR32_T:
				literal_type = YLT_UNSIGNEDINTEGER;
				break;

			case FT_FLOAT:
			case FT_DOUBLE:
			case FT_LONG_DOUBLE:
				literal_type = YLT_FLOATING;
				break;

			default:
				break;
			};

		CY86Literal literal(literal_type, literal_token.data,
		                    SizeOfFundamentalType(literal_token.fundamental_type));

		if (negate)
			literal.negate();

		return literal;
	}

	void parse_instruction()
	{
		string opcode = parse_identifier();

		vector<CY86Operand*> operands;

		while (!peek(OP_SEMICOLON))
			operands.push_back(parse_operand());

		program.add_instruction(opcode, operands);
		program.add_labels(labels);

		labels.clear();
	}

	CY86Operand* parse_operand()
	{
		if (peek(TT_IDENTIFIER))
		{
			string id = parse_identifier();

			auto reg = StringToCY86Register(id);

			if (reg != YR_UNKNOWN)
				return new CY86Register(reg);
			else
				return new CY86LabelImmediate(id, 0);
		}
		else if (peek(TT_LITERAL))
		{
			CY86Literal literal = parse_literal();
			return new CY86AbsoluteImmediate(literal);
		}
		else if (peek(OP_LPAREN))
		{
			drop(1);

			CY86Operand* operand = parse_immediate();

			if (drop(OP_RPAREN))
				return operand;
			else
				throw logic_error("expected )");
		}
		else if (peek(OP_LSQUARE))
		{
			drop(1);

			CY86Operand* operand = parse_memory();

			if (drop(OP_RSQUARE))
				return operand;
			else
				throw logic_error("expected ]");
		}
		else
			throw logic_error("unexpected token");
	}

	signed long int parse_delta()
	{
		signed long int delta = 0;
		if (peek(OP_PLUS) || peek(OP_MINUS))
		{
			CY86Literal literal = parse_literal();
			delta = literal.to_delta64();
		}
		return delta;
	}

	CY86Operand* parse_immediate()
	{
		if (peek(TT_IDENTIFIER))
		{
			string id = parse_identifier();

			if (StringToCY86Register(id) != YR_UNKNOWN)
				throw logic_error("unexpected register");

			signed long int delta = parse_delta();

			return new CY86LabelImmediate(id, delta);
		}
		else if (peek(TT_LITERAL) || peek(OP_MINUS))
		{
			CY86Literal literal = parse_literal();

			return new CY86AbsoluteImmediate(literal);
		}
		else
			throw logic_error("unexpected token");
	}

	CY86Operand* parse_memory()
	{
		if (peek(TT_IDENTIFIER))
		{
			string id = parse_identifier();

			auto reg = StringToCY86Register(id);

			signed long int delta = parse_delta();

			if (reg == YR_UNKNOWN)
				return new CY86LabelMemory(id, delta);
			else
				return new CY86RegisterMemory(reg, delta);
		}
		else if (peek(TT_LITERAL))
		{
			CY86Literal literal = parse_literal();

			auto address = (unsigned long int) literal.to_delta64();

			return new CY86AbsoluteMemory(address);
		}
		else
			throw logic_error("unexpected token");
	}
};


CY86Asm::CY86Asm()
	: parser(new CY86AsmParser)
{}

CY86Asm::~CY86Asm()
{
	delete parser;
}

void CY86Asm::parse(const string& srcfile, vector<PToken>&& tokens)
{
	parser->parse(srcfile, move(tokens));
}

void CY86Asm::dump(ostream&)
{

}

void CY86Asm::output_program(ostream& out)
{
	vector<byte> data = parser->program.assemble();
	out.write((char*) &data[0], data.size());
}

pair<string, size_t> CY86Asm::error_loc()
{
	return parser->get_deepest();
}

