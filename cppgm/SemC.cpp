#include "std.pch"

#include "ToString.h"

#include "SemC.h"
#include "SymbolTable.h"
#include "Statements.h"

struct SemCParser
{
	SymbolTable symtab;

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
		SemCParser* parser;
		size_t start;
		string function;
		bool failed = false;

		ParseFunction(SemCParser* parser, const string& function)
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

	SemCParser() {}

	void parse(const string& srcfile, vector<PToken>&& tokens_in)
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

		parse_translation_unit(srcfile);
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

	// ====================== BEGIN PARSE FUNCTIONS =============================

	string parse_identifier()
	{
		if (!peek(TT_IDENTIFIER))
			return "";

		string id = peekt()->src.spelling;

		pop();

		return id;
	}

	string parse_ordinary_string_literal()
	{
		PToken tok = pop();

		return tok->as_ordinary_string();
	}

	void parse_translation_unit(const string& srcfile)
	{
		DeclParse(translation_unit);

		symtab.enter_translation_unit(srcfile);

		while (parse_declaration());

		if (!drop(TT_EOF))
			throw logic_error("expected EOF - " + ToString(*peekt()));

		symtab.exit_translation_unit();
	}

	void enter_namespace_definition(const string& id, bool inline_namespace);

	void exit_namespace_definition();

	bool parse_declaration()
	{
		DeclParse(declaration);

		switch (peek())
		{
		case OP_SEMICOLON:
			pop();
			return true;

		case KW_INLINE:
			switch (peek(1))
			{
			case KW_NAMESPACE:
				return parse_namespace_definition();

			default:
				break;
			}
			break;

		case KW_NAMESPACE:
			if (peek(1, TT_IDENTIFIER) && peek(2, OP_ASS))
			{
				auto decl = parse_namespace_alias_definition();

				if (!decl)
					throw logic_error("ill-formed namespace alias definition");

				symtab.process_declaration(decl);
				return true;
			}
			else
				return parse_namespace_definition();

		case KW_USING:
			if (peek(1, KW_NAMESPACE))
			{
				auto decl = parse_using_directive();

				if (!decl)
					throw logic_error("ill-formed using directive");

				symtab.process_declaration(decl);
				return true;
			}
			else if (peek(1, TT_IDENTIFIER) && peek(2, OP_ASS))
			{
				auto decl = parse_alias_declaration();
				if (!decl)
					throw logic_error("ill-formed alias declaration");

				symtab.process_declaration(decl);
				return true;
			}
			else
			{
				auto decl = parse_using_declaration();

				if (!decl)
					throw logic_error("ill-formed using declaration");

				symtab.process_declaration(decl);
				return true;
			}

		case KW_STATIC_ASSERT:
			{
				auto decl = parse_static_assert_declaration();

				if (!decl)
					throw logic_error("ill-formed static_assert declaration");

				symtab.process_declaration(decl);
				return true;
			}

		default:
			break;
		}

		DeclSpecifierSeq* decl_specifier_seq = parse_decl_specifier_seq();

		if (!decl_specifier_seq)
			return func.fail();

		ScopeChanger scope_changer(symtab);

		Declarator* first_declarator = parse_declarator(&scope_changer);

		if (!first_declarator)
			return func.fail();

		if (first_declarator->viable_function_declarator())
		{
			switch (peek())
			{
			case TT_IDENTIFIER:
			case KW_TRY:
			case OP_ASS:
			case OP_COLON:
			case OP_LBRACE:
				return parse_function_definition_suffix(decl_specifier_seq, first_declarator);

			default: /* fallthrough */;
			}
		}

		Entity* entity = symtab.process_simple_declarator(decl_specifier_seq, first_declarator);

		entity->set_initializer(symtab, parse_initializer());

		while (drop(OP_COMMA))
		{
			scope_changer.reset();

			Declarator* declarator = parse_declarator(&scope_changer);

			if (!declarator)
				return func.fail();

			Entity* entity = symtab.process_simple_declarator(decl_specifier_seq, declarator);

			entity->set_initializer(symtab, parse_initializer());
		}

		if (!drop(OP_SEMICOLON))
			return func.fail();

		return true;
	}

	bool parse_function_definition_suffix(DeclSpecifierSeq* decl_specifier_seq, Declarator* function_declarator)
	{
		Entity* entity = symtab.process_simple_declarator(decl_specifier_seq, function_declarator);
		if (peek(0, OP_LBRACE) && peek(1, OP_RBRACE))
		{
			drop(2);

			entity->set_function_definition();

			return true;
		}
		else
			throw logic_error("not yet implemented");
	}

	bool parse_namespace_definition()
	{
		DeclParse(namespace_definition);

		bool inline_namespace = drop(KW_INLINE);

		if (!drop(KW_NAMESPACE))
			throw logic_error("expected namespace keyword");

		string id = parse_identifier();

		symtab.enter_namespace_definition(id, inline_namespace);

		if (!drop(OP_LBRACE))
			throw logic_error("expected { to open namespace definition (" + id + ")");

		func.nest();

		while (parse_declaration());

		if (!drop(OP_RBRACE))
			throw logic_error("expected } to close namespace definition (" + id + ")");

		func.unnest();

		symtab.exit_namespace_definition();

		return true;
	}

	RawExpression* parse_primary_expression()
	{
		DeclParse(primary_expression);

		switch (peek())
		{
		case KW_TRUE:
		case KW_FALSE:
		case KW_NULLPTR:
		case TT_LITERAL:
			return new LiteralExpression(pop());

		case TT_UDSTRING:
		case TT_UDCHARACTER:
		case TT_UDINTEGER:
		case TT_UDFLOATING:
		case KW_THIS:
			throw logic_error("unsupported token type");

		case OP_LPAREN:
			drop();
			func.nest();
			{
				RawExpression* expression = parse_expression();
				if (!expression || !drop(OP_RPAREN))
					return nullptr;

				return expression;
			}

		default:
			QualifiedName* name = parse_qualified_name();

			if (!name)
				return func.fail();

			return new IdExpression(name);
		}
	}

	QualifiedName* parse_qualified_name()
	{
		DeclParse(qualified_name);

		QualifiedName* qualified_name = new QualifiedName(maybe(OP_COLON2));

		do
		{
			string id = parse_identifier();

			if (id.empty())
				return func.fail();

			qualified_name->push_identifier(id);
		}
		while (drop(OP_COLON2));

		return qualified_name;
	}

	RawExpression* parse_postfix_expression()
	{
		DeclParse(postfix_expression);

		RawExpression* postfix_expression = parse_postfix_root();

		if (!postfix_expression)
			return func.fail();

		while (true)
		{
			RawExpression* postfix_expression_next = parse_postfix_suffix(postfix_expression);

			if (!postfix_expression_next)
				break;

			postfix_expression = postfix_expression_next;
		}

		return postfix_expression;
	}

	RawExpression* parse_postfix_root()
	{
		DeclParse(postfix_root);

		switch (peek())
		{
		case KW_DYNAMIC_CAST:
		case KW_STATIC_CAST:
		case KW_REINTERPET_CAST:
		case KW_CONST_CAST:
			{
				PToken keyword = pop();

				if (!drop(OP_LT))
					return func.fail();

				func.nest(true);

				Type* type_id = parse_type_id();

				if (!type_id)
					return func.fail();

				if (!parse_close_angle_bracket())
					return func.fail();

				func.unnest();

				if (!drop(OP_LPAREN))
					return func.fail();

				func.nest();

				RawExpression* operand = parse_expression();

				if (!operand)
					return func.fail();

				if (!drop(OP_RPAREN))
					return func.fail();

				func.unnest();

				ECastCategory ct;

				switch (keyword->token_type)
				{
				case KW_DYNAMIC_CAST: ct = CT_DYNAMIC; break;
				case KW_STATIC_CAST: ct = CT_STATIC; break;
				case KW_REINTERPET_CAST: ct = CT_REINTERPRET; break;
				case KW_CONST_CAST: ct = CT_CONST; break;
				default:
					throw logic_error("internal error, parse_postfix_root");
				}

				return new CastExpression(ct, type_id, operand);
			}

		default:
			break;
		}

		Type* simple_type_specifier = parse_simple_type_specifier();

		if (simple_type_specifier)
		{
			if (!drop(OP_LPAREN))
				return func.fail();

			func.nest();

			ExpressionList* expression_list = parse_expression_list();

			if (!drop(OP_RPAREN))
				return func.fail();

			func.unnest();

			return new CastExpression(simple_type_specifier, expression_list);
		}

		return parse_primary_expression();
	}

	RawExpression* parse_postfix_suffix(RawExpression* lhs)
	{
		DeclParse(postfix_suffix);

		switch (peek())
		{
		case OP_LSQUARE:
			{
				pop();
				func.nest();

				RawExpression* rhs = parse_expression();

				if (!rhs)
					return func.fail();

				if (!drop(OP_RSQUARE))
					return func.fail();

				func.unnest();

				return new BinaryExpression(BO_SUBSCRIPT, lhs, rhs);
			}

		case OP_LPAREN:
			{
				pop();
				func.nest();

				ExpressionList* rhs = parse_expression_list();

				if (!drop(OP_RPAREN))
					return func.fail();

				func.unnest();

				return new FunctionCallExpression(lhs, rhs);
			}

		case OP_INC:
			pop();
			return new UnaryExpression(UO_POSTINC, lhs);

		case OP_DEC:
			pop();
			return new UnaryExpression(UO_POSTDEC, lhs);

		default:
			return func.fail();
		}
	}

	ExpressionList* parse_expression_list()
	{
		DeclParse(expression_list);

		vector<RawExpression*> expressions;

		RawExpression* expression = parse_assignment_expression();

		if (!expression)
			return func.fail();

		expressions.push_back(expression);

		while (drop(OP_COMMA))
		{
			expression = parse_assignment_expression();

			if (!expression)
				return func.fail();

			expressions.push_back(expression);
		}

		return new ExpressionList(move(expressions));
	}

	RawExpression* parse_unary_expression()
	{
		DeclParse(unary_expression);

		switch (peek())
		{
		case KW_SIZEOF:
			{
				pop();

				size_t save = pos;

				if (peek(OP_LPAREN))
				{
					func.nest();

					Type* type = parse_type_id();

					if (type && drop(OP_RPAREN))
					{
						return new SizeOfExpression(type);
					}

					func.unnest();
				}

				jump(save);

				RawExpression* operand = parse_unary_expression();

				if (!operand)
					return func.fail();

				return new SizeOfExpression(operand);
			}

		case OP_INC:
		case OP_DEC:
		case OP_STAR:
		case OP_AMP:
		case OP_PLUS:
		case OP_MINUS:
		case OP_LNOT:
		case OP_COMPL:
			{
				auto op = pop();

				RawExpression* operand = parse_cast_expression();

				if (!operand)
					return func.fail();

				EUnaryOperation uo;

				switch (op->token_type)
				{
				case OP_INC:    uo = UO_PREINC; break;
				case OP_DEC:    uo = UO_PREDEC; break;
				case OP_STAR:   uo = UO_INDIR; break;
				case OP_AMP:    uo = UO_ADDROF; break;
				case OP_PLUS:   uo = UO_POS; break;
				case OP_MINUS:  uo = UO_NEG; break;
				case OP_LNOT:   uo = UO_LNOT; break;
				case OP_COMPL:  uo = UO_COMPL; break;
				default:
					throw logic_error("internal error, parse_unary_expression");
				}

				return new UnaryExpression(uo, operand);
			}

		default:
			return parse_postfix_expression();
		}
	}

	RawExpression* parse_cast_expression()
	{
		DeclParse(cast_expression);

		size_t save = pos;

		if (peek(OP_LPAREN))
		{
			func.nest();

			Type* type = parse_type_id();

			if (type && drop(OP_RPAREN))
			{
				RawExpression* operand = parse_cast_expression();

				if (!operand)
					return func.fail();

				return new CastExpression(type, operand);
			}

			func.unnest();
		}

		jump(save);

		return parse_unary_expression();
	}

	RawExpression* parse_multiplicative_expression()
	{
		DeclParse(multiplicative_expression);

		RawExpression* lhs = parse_cast_expression();

		if (!lhs)
			return func.fail();

		while (true)
		{
			switch (peek())
			{
			case OP_STAR:
			case OP_DIV:
			case OP_MOD:
				{
					auto op = pop();

					RawExpression* rhs = parse_cast_expression();

					if (!rhs)
						return func.fail();

					EBinaryOperation bo;

					switch (op->token_type)
					{
					case OP_STAR: bo = BO_MULT; break;
					case OP_DIV: bo = BO_DIV; break;
					case OP_MOD: bo = BO_MOD; break;
					default:
						throw logic_error("internal error, parse_multiplicative_expression");
					}

					lhs = new BinaryExpression(bo, lhs, rhs);
				}

			default:
				return lhs;
			}
		}
	}

	RawExpression* parse_additive_expression()
	{
		DeclParse(additive_expression);

		RawExpression* lhs = parse_multiplicative_expression();

		if (!lhs)
			return func.fail();

		while (true)
		{
			switch (peek())
			{
			case OP_PLUS:
			case OP_MINUS:
				{
					auto op = pop();

					RawExpression* rhs = parse_multiplicative_expression();

					if (!rhs)
						return func.fail();

					EBinaryOperation bo;

					if (op->token_type == OP_PLUS)
						bo = BO_ADD;
					else
						bo = BO_SUBTRACT;

					lhs = new BinaryExpression(bo, lhs, rhs);
				}

			default:
				return lhs;
			}
		}
	}

	RawExpression* parse_shift_expression()
	{
		DeclParse(shift_expression);

		RawExpression* lhs = parse_additive_expression();

		if (!lhs)
			return func.fail();

		while (true)
		{
			EBinaryOperation bo;

			if (peek(OP_LSHIFT))
			{
				drop();
				bo = BO_LSHIFT;
			} else if (!in_angle && peek(0, ST_RSHIFT_1) && peek(1, ST_RSHIFT_2))
			{
				drop(2);
				bo = BO_RSHIFT;
			}
			else
				return lhs;

			RawExpression* rhs = parse_additive_expression();

			if (!rhs)
				return func.fail();

			lhs = new BinaryExpression(bo, lhs, rhs);
		}
	}

	RawExpression* parse_relational_expression()
	{
		DeclParse(relational_expression);

		RawExpression* lhs = parse_shift_expression();

		if (!lhs)
			return func.fail();

		while (true)
		{
			switch (peek())
			{
			case OP_GT:
				if (in_angle)
					return lhs;

			case OP_LT:
			case OP_LE:
			case OP_GE:
				{
					auto op = pop();

					RawExpression* rhs = parse_shift_expression();

					if (!rhs)
						return func.fail();

					EBinaryOperation bo;

					switch (op->token_type)
					{
					case OP_GT: bo = BO_GT; break;
					case OP_LT: bo = BO_LT; break;
					case OP_LE: bo = BO_LE; break;
					case OP_GE: bo = BO_GE; break;
					default:
						throw logic_error("internal error, parse_multiplicative_expression");
					}

					lhs = new BinaryExpression(bo, lhs, rhs);
				}

			default:
				return lhs;
			}
		}
	}

	RawExpression* parse_equality_expression()
	{
		DeclParse(equality_expression);

		RawExpression* lhs = parse_relational_expression();

		if (!lhs)
			return func.fail();

		while (true)
		{
			switch (peek())
			{
			case OP_EQ:
			case OP_NE:
				{
					auto op = pop();

					RawExpression* rhs = parse_relational_expression();

					if (!rhs)
						return func.fail();

					EBinaryOperation bo;

					if (op->token_type == OP_EQ)
						bo = BO_EQ;
					else
						bo = BO_NE;

					lhs = new BinaryExpression(bo, lhs, rhs);
				}

			default:
				return lhs;
			}
		}
	}

	RawExpression* parse_and_expression()
	{
		DeclParse(and_expression);

		RawExpression* lhs = parse_equality_expression();

		if (!lhs)
			return func.fail();

		while (true)
		{
			if (peek(OP_AMP))
			{
				drop();

				RawExpression* rhs = parse_equality_expression();

				if (!rhs)
					return func.fail();

				lhs = new BinaryExpression(BO_BAND, lhs, rhs);
			}
			else
				return lhs;
		}
	}

	RawExpression* parse_exclusive_or_expression()
	{
		DeclParse(exclusive_or_expression);

		RawExpression* lhs = parse_and_expression();

		if (!lhs)
			return func.fail();

		while (true)
		{
			if (peek(OP_XOR))
			{
				drop();

				RawExpression* rhs = parse_and_expression();

				if (!rhs)
					return func.fail();

				lhs = new BinaryExpression(BO_XOR, lhs, rhs);
			}
			else
				return lhs;
		}
	}

	RawExpression* parse_inclusive_or_expression()
	{
		DeclParse(inclusive_or_expression);

		RawExpression* lhs = parse_exclusive_or_expression();

		if (!lhs)
			return func.fail();

		while (true)
		{
			if (peek(OP_BOR))
			{
				drop();

				RawExpression* rhs = parse_exclusive_or_expression();

				if (!rhs)
					return func.fail();

				lhs = new BinaryExpression(BO_BOR, lhs, rhs);
			}
			else
				return lhs;
		}
	}

	RawExpression* parse_logical_and_expression()
	{
		DeclParse(logical_and_expression);

		RawExpression* lhs = parse_inclusive_or_expression();

		if (!lhs)
			return func.fail();

		while (true)
		{
			if (peek(OP_LAND))
			{
				drop();

				RawExpression* rhs = parse_inclusive_or_expression();

				if (!rhs)
					return func.fail();

				lhs = new BinaryExpression(BO_LAND, lhs, rhs);
			}
			else
				return lhs;
		}
	}

	RawExpression* parse_logical_or_expression()
	{
		DeclParse(logical_or_expression);

		RawExpression* lhs = parse_logical_and_expression();

		if (!lhs)
			return func.fail();

		while (true)
		{
			if (peek(OP_LOR))
			{
				drop();

				RawExpression* rhs = parse_logical_and_expression();

				if (!rhs)
					return func.fail();

				lhs = new BinaryExpression(BO_LOR, lhs, rhs);
			}
			else
				return lhs;
		}
	}

	RawExpression* parse_conditional_expression(RawExpression* expr = nullptr)
	{
		DeclParse(conditional_expression);

		if (!expr)
		{
			expr = parse_logical_or_expression();

			if (!expr)
				return func.fail();
		}

		while (drop(OP_QMARK))
		{
			RawExpression* ontrue = parse_expression();

			if (!ontrue)
				return func.fail();

			if (!drop(OP_COLON))
				return func.fail();

			RawExpression* onfalse = parse_assignment_expression();

			expr = new ConditionalExpression(expr, ontrue, onfalse);
		}

		return expr;
	}

	RawExpression* parse_assignment_expression()
	{
		DeclParse(assignment_expression);

		RawExpression* lhs = parse_logical_or_expression();

		if (!lhs)
		{
			return func.fail();
		}

		switch (peek())
		{
		case OP_QMARK:
			return parse_conditional_expression(lhs);

		case OP_ASS:
	    case OP_STARASS:
	    case OP_DIVASS:
	    case OP_MODASS:
	    case OP_PLUSASS:
	    case OP_MINUSASS:
	    case OP_RSHIFTASS:
	    case OP_LSHIFTASS:
	    case OP_BANDASS:
	    case OP_XORASS:
	    case OP_BORASS:
	    	{
	    		PToken op = pop();

	    		RawExpression* rhs = parse_assignment_expression();

	    		if (!rhs)
	    			return func.fail();

	    		EBinaryOperation bo;

	    		switch (op->token_type)
	    		{
	    		case OP_ASS: bo = BO_ASS; break;
	    	    case OP_STARASS: bo = BO_MULTASS; break;
	    	    case OP_DIVASS: bo = BO_DIVASS; break;
	    	    case OP_MODASS: bo = BO_MODASS; break;
	    	    case OP_PLUSASS: bo = BO_ADDASS; break;
	    	    case OP_MINUSASS: bo = BO_SUBTRACTASS; break;
	    	    case OP_RSHIFTASS: bo = BO_RSHIFTASS; break;
	    	    case OP_LSHIFTASS: bo = BO_LSHIFTASS; break;
	    	    case OP_BANDASS: bo = BO_BANDASS; break;
	    	    case OP_XORASS: bo = BO_XORASS; break;
	    	    case OP_BORASS: bo = BO_BORASS; break;
	    	    default:
	    	    	throw logic_error("internal error, parse_assignment_expression");
	    		}

	    		return new BinaryExpression(bo, lhs, rhs);
	    	}

	    default:
	    	return lhs;
		}
	}

	RawExpression* parse_expression()
	{
		DeclParse(expression);

		RawExpression* lhs = parse_assignment_expression();

		if (!lhs)
			return func.fail();

		while (drop(OP_COMMA))
		{
			RawExpression* rhs = parse_assignment_expression();

			if (!rhs)
				return func.fail();

			lhs = new BinaryExpression(BO_COMMA, lhs, rhs);
		}

		return lhs;
	}

	RawExpression* parse_constant_expression()
	{
		return parse_conditional_expression();
	}

	Statement* parse_statement()
	{
		DeclParse(statement);

		if (peek(0, TT_IDENTIFIER) && peek(1, OP_COLON))
			return parse_labeled_statement();

		switch (peek())
		{
		case OP_LBRACE:
			return parse_compound_statement();

		case KW_IF:
		case KW_SWITCH:
			return parse_selection_statement();

		case KW_DO:
		case KW_FOR:
		case KW_WHILE:
			return parse_iteration_statement();

		case KW_BREAK:
		case KW_CONTINUE:
		case KW_GOTO:
		case KW_RETURN:
			return parse_jump_statement();

		default:
			break;
		}

		Statement* declaration_statement = parse_declaration_statement();

		if (declaration_statement)
			return declaration_statement;

		return parse_expression_statement();
	}

	Statement* parse_labeled_statement()
	{
		DeclParse(labeled_statement);

		switch (peek())
		{
		case TT_IDENTIFIER:
			{
				string label = parse_identifier();

				if (!drop(OP_COLON))
					return func.fail();

				Statement* statement = parse_statement();

				if (!statement)
					return func.fail();

				return new LabeledStatement(label, statement);
			}
		case KW_CASE:
			{
				pop();

				RawExpression* expression = parse_constant_expression();

				if (!expression)
					return func.fail();

				if (!drop(OP_COLON))
					return func.fail();

				Statement* statement = parse_statement();

				if (!statement)
					return func.fail();

				return new CaseStatement(expression, statement);
			}

		case KW_DEFAULT:
			{
				pop();

				if (!drop(OP_COLON))
					return func.fail();

				Statement* statement = parse_statement();

				if (!statement)
					return func.fail();

				return new DefaultStatement(statement);
			}

		default:
			return func.fail();
		};
	}

	Statement* parse_expression_statement()
	{
		DeclParse(expression_statement);

		RawExpression* expression = parse_expression();

		if (!drop(OP_SEMICOLON))
			return func.fail();

		if (!expression)
			return new EmptyStatement();
		else
			return new ExpressionStatement(expression);
	}

	Statement* parse_compound_statement()
	{
		DeclParse(compound_statement);

		if (!drop(OP_LBRACE))
			return func.fail();

		func.nest();

		CompoundStatement* compound_statement = new CompoundStatement;

		while (Statement* statement = parse_statement())
			compound_statement->push_statement(statement);

		if (!drop(OP_RBRACE))
			return func.fail();

		func.unnest();

		return compound_statement;
	}

	Statement* parse_selection_statement()
	{
		DeclParse(selection_statement);

		if (peek(KW_SWITCH))
		{
			pop();

			if (!drop(OP_LPAREN))
				return func.fail();

			func.nest();

			Condition* condition = parse_condition();

			if (!condition)
				return func.fail();

			if (!drop(OP_RPAREN))
				return func.fail();

			func.unnest();

			Statement* statement = parse_statement();

			if (!statement)
				return func.fail();

			return new SwitchStatement(condition, statement);
		}
		else if (peek(KW_IF))
		{
			pop();

			if (!drop(OP_LPAREN))
				return func.fail();

			func.nest();

			Condition* condition = parse_condition();

			if (!condition)
				return func.fail();

			if (!drop(OP_RPAREN))
				return func.fail();

			func.unnest();

			Statement* ontrue = parse_statement();

			if (!ontrue)
				return func.fail();

			if (!drop(KW_ELSE))
				return new IfStatement(condition, ontrue);

			Statement* onfalse = parse_statement();

			if (!onfalse)
				return func.fail();

			return new IfStatement(condition, ontrue, onfalse);
		}
		else
			return func.fail();
	}

	ConditionDeclaration* parse_condition_declaration()
	{
		DeclParse(condition_declaration);

		DeclSpecifierSeq* specifiers = parse_decl_specifier_seq();

		if (!specifiers)
			return func.fail();

		Declarator* declarator = parse_declarator();

		if (!declarator)
			return func.fail();

		if (!drop(OP_ASS))
			return func.fail();

		RawExpression* initializer_clause = parse_assignment_expression();

		if (!initializer_clause)
			return func.fail();

		return new ConditionDeclaration(specifiers, declarator, initializer_clause);
	}

	Condition* parse_condition()
	{
		DeclParse(condition);

		ConditionDeclaration* condition_declaration = parse_condition_declaration();

		if (condition_declaration)
			return condition_declaration;

		RawExpression* expression = parse_expression();

		if (!expression)
			return func.fail();

		return new ExpressionCondition(expression);
	}

	Statement* parse_iteration_statement()
	{
		DeclParse(iteration_statement);

		switch (peek())
		{
		case KW_WHILE:
			{
				pop();

				if (!drop(OP_LPAREN))
					return func.fail();

				func.nest();

				Condition* condition = parse_condition();

				if (!condition)
					return func.fail();

				if (!drop(OP_RPAREN))
					return func.fail();

				func.unnest();

				Statement* statement = parse_statement();

				if (!statement)
					return func.fail();

				return new WhileStatement(condition, statement);
			}

		case KW_DO:
			{
				pop();

				Statement* statement = parse_statement();

				if (!statement)
					return func.fail();

				if (!drop(KW_WHILE))
					return func.fail();

				if (!drop(OP_LPAREN))
					return func.fail();

				func.nest();

				RawExpression* expression = parse_expression();

				if (!expression)
					return func.fail();

				if (!drop(OP_RPAREN))
					return func.fail();

				func.unnest();

				if (!drop(OP_SEMICOLON))
					return func.fail();

				return new DoStatement(statement, expression);
			}

		case KW_FOR:
			{
				pop();

				if (!drop(OP_LPAREN))
					return func.fail();

				func.nest();

				Statement* for_init_statement = parse_for_init_statement();

				if (!for_init_statement)
					return func.fail();

				Condition* condition = parse_condition();

				if (!drop(OP_SEMICOLON))
					return func.fail();

				RawExpression* expression = parse_expression();

				if (!drop(OP_RPAREN))
					return func.fail();

				func.unnest();

				Statement* statement = parse_statement();

				if (!statement)
					return func.fail();

				return new ForStatement(for_init_statement,
					condition, expression, statement);
			}

		default:
			return func.fail();
		}
	}

	Statement* parse_for_init_statement()
	{
		DeclParse(for_init_statement);

		Declaration* simple_declaration = parse_simple_declaration();

		if (simple_declaration)
			return new DeclarationStatement(simple_declaration);
		else
			return parse_expression_statement();
	}

	Statement* parse_jump_statement()
	{
		DeclParse(jump_statement);

		switch (peek())
		{
		case KW_BREAK:
			pop();

			if (!drop(OP_SEMICOLON))
				return func.fail();

			return new BreakStatement;

		case KW_CONTINUE:
			pop();

			if (!drop(OP_SEMICOLON))
				return func.fail();

			return new ContinueStatement;

		case KW_RETURN:
			{
				pop();

				RawExpression* expression = parse_expression();

				if (!drop(OP_SEMICOLON))
					return func.fail();

				return new ReturnStatement(expression);
			}

		case KW_GOTO:
			{
				pop();

				string id = parse_identifier();

				if (id.empty())
					return func.fail();

				if (!drop(OP_SEMICOLON))
					return func.fail();

				return new GotoStatement(id);
			}

		default:
			return func.fail();
		}
	}

	Statement* parse_declaration_statement()
	{
		Declaration* declaration = parse_block_declaration();

		return new DeclarationStatement(declaration);
	}

	Declaration* parse_block_declaration()
	{
		DeclParse(block_declaration);

		switch (peek())
		{
		case KW_NAMESPACE:
			if (peek(1, TT_IDENTIFIER) && peek(2, OP_ASS))
				return parse_namespace_alias_definition();
			else
				return func.fail();

		case KW_USING:
			if (peek(1, KW_NAMESPACE))
				return parse_using_directive();
			else if (peek(1, TT_IDENTIFIER) && peek(2, OP_ASS))
				return parse_alias_declaration();
			else
				return parse_using_declaration();

		case KW_STATIC_ASSERT:
			return parse_static_assert_declaration();
		}

//		DeclSpecifierSeq* decl_specifier_seq = parse_decl_specifier_seq();
//
//		if (!decl_specifier_seq)
//			return func.fail();
//
//		InitDeclaratorList* init_declarator_list = parse_init_declarator_list();
//
//		if (!drop(OP_SEMICOLON))
//			return func.fail();
//
//		return new SimpleDeclaration(decl_specifier_seq, init_declarator_list);

		// TODO parse simple declaration
		return func.fail();
	}

	AliasDeclaration* parse_alias_declaration()
	{
		DeclParse(alias_declaration);

		if (!drop(KW_USING))
			return func.fail();

		string id = parse_identifier();

		if (id.empty())
			return func.fail();

		if (!drop(OP_ASS))
			return func.fail();

		Type* type_id = parse_type_id();

		if (!type_id)
			return func.fail();

		if (!drop(OP_SEMICOLON))
			return func.fail();

		return new AliasDeclaration(id, type_id);
	}

	Declaration* parse_simple_declaration()
	{
		DeclParse(simple_declaration);

		DeclSpecifierSeq* decl_specifier_seq = parse_decl_specifier_seq();

		if (!decl_specifier_seq)
			return func.fail();

		parse_init_declarator_list();

		if (drop(OP_SEMICOLON))
			return new EmptyDeclaration;
// TODO		SimpleDeclaration(decl_specifier_seq, init_declarator_list);
		else
			return func.fail();
	}

	StaticAssertDeclaration* parse_static_assert_declaration()
	{
		DeclParse(static_assert_declaration);

		if (!drop(KW_STATIC_ASSERT))
			return func.fail();

		if (!drop(OP_LPAREN))
			return func.fail();

		func.nest();

		RawExpression* expression = parse_constant_expression();

		if (!expression)
			return func.fail();

		if (!drop(OP_COMMA))
			return func.fail();

		if (!peek(TT_LITERAL))
			return func.fail();

		string message = parse_ordinary_string_literal();

		if (!drop(OP_RPAREN))
			return func.fail();

		func.unnest();

		if (!drop(OP_SEMICOLON))
			return func.fail();

		return new StaticAssertDeclaration(expression, message);
	}

	Declaration* parse_empty_declaration()
	{
		DeclParse(empty_declaration);

		if (drop(OP_SEMICOLON))
			return new EmptyDeclaration;
		else
			return func.fail();
	}

	DeclSpecifier* parse_decl_specifier(bool type_name_ok,
	                                    bool type_specifier_only = false)
	{
		DeclParse(decl_specifier);

		switch (peek())
		{
		// storage-class-specifier
		case KW_REGISTER:
		case KW_STATIC:
		case KW_THREAD_LOCAL:
		case KW_EXTERN:

		// function-specifier
		case KW_INLINE:

		// pure decl-specifier
		case KW_TYPEDEF:
		case KW_CONSTEXPR:

			if (type_specifier_only)
				return func.fail();

		// simple-type-specifier
		case KW_CHAR:
		case KW_CHAR16_T:
		case KW_CHAR32_T:
		case KW_WCHAR_T:
		case KW_BOOL:
		case KW_SHORT:
		case KW_INT:
		case KW_LONG:
		case KW_SIGNED:
		case KW_UNSIGNED:
		case KW_FLOAT:
		case KW_DOUBLE:
		case KW_VOID:

		// cv-qualifier
		case KW_CONST:
		case KW_VOLATILE:
			{
				auto kw = pop();
				return new KeywordDeclSpecifier(kw->token_type);
			}

		case KW_DECLTYPE:
			return parse_decltype_specifier();

		case OP_COLON2:
		case TT_IDENTIFIER:

			if (type_name_ok)
			{
				QualifiedName* name = parse_qualified_name();

				if (!name)
					return func.fail();

				Type* type = symtab.lookup_type(name);

				if (!type)
					return func.fail();

				return new TypeDeclSpecifier(type);
			}

			return func.fail();

		default:
			return func.fail();
		}
	}

	DeclSpecifierSeq* parse_decl_specifier_seq()
	{
		DeclParse(decl_specifier_seq);

		DeclSpecifierSeq* decl_specifier_seq = new DeclSpecifierSeq;

		bool type_name_ok = true;

		while (true)
		{
			DeclSpecifier* decl_specifier = parse_decl_specifier(type_name_ok);

			if (!decl_specifier)
			{
				if (!decl_specifier_seq->empty())
					return decl_specifier_seq;
				else
					return func.fail();
			}

			if (decl_specifier->is_non_cv_type_specifier())
				type_name_ok = false;

			decl_specifier_seq->push_decl_specifier(decl_specifier);
		}
	}

	DeclSpecifierSeq* parse_type_specifier_seq()
	{
		DeclParse(type_specifier_seq);

		DeclSpecifierSeq* decl_specifier_seq = new DeclSpecifierSeq;

		while (true)
		{
			DeclSpecifier* decl_specifier = parse_decl_specifier(true, true);

			if (!decl_specifier)
			{
				if (decl_specifier_seq->empty())
					return func.fail();

				return decl_specifier_seq;
			}

			decl_specifier_seq->push_decl_specifier(decl_specifier);
		}
	}

	Type* parse_simple_type_specifier()
	{
		DeclParse(simple_type_specifier);

		switch (peek())
		{
		case KW_CHAR:
		case KW_CHAR16_T:
		case KW_CHAR32_T:
		case KW_WCHAR_T:
		case KW_BOOL:
		case KW_SHORT:
		case KW_INT:
		case KW_LONG:
		case KW_SIGNED:
		case KW_UNSIGNED:
		case KW_FLOAT:
		case KW_DOUBLE:
		case KW_VOID:
			return new FundamentalType(pop()->token_type, false, false);

		case KW_DECLTYPE:
			{
				DeclTypeSpecifier* decltype_specifier = parse_decltype_specifier();

				if (!decltype_specifier)
					return func.fail();

				return symtab.deduce_type(decltype_specifier);
			}

		case OP_COLON2:
		case TT_IDENTIFIER:
			{
				QualifiedName* name = parse_qualified_name();

				if (!name)
					return func.fail();

				if (Type* type = symtab.lookup_type(name))
					return type;
				else
					return func.fail();
			}

		default:
			return func.fail();
		}
	}

	DeclTypeSpecifier* parse_decltype_specifier()
	{
		DeclParse(decltype_specifier);

		if (!drop(KW_DECLTYPE))
			return func.fail();

		if (!drop(OP_LPAREN))
			return func.fail();

		func.nest();

		RawExpression* expr = parse_expression();

		if (!expr)
			return func.fail();

		if (!drop(OP_RPAREN))
			return func.fail();

		func.unnest();

		return new DeclTypeSpecifier(expr);
	}

	NamespaceAliasDefinition* parse_namespace_alias_definition()
	{
		DeclParse(namespace_alias_definition);

		if (!drop(KW_NAMESPACE))
			return func.fail();

		string identifier = parse_identifier();

		if (identifier.empty())
			return func.fail();

		if (!drop(OP_ASS))
			return func.fail();

		QualifiedName* name = parse_qualified_name();

		if (!name)
			return func.fail();

		Namespace* ns = symtab.lookup_namespace(name);

		if (!ns)
			return func.fail();

		if (!drop(OP_SEMICOLON))
			return func.fail();

		return new NamespaceAliasDefinition(identifier, ns);
	}

	UsingDeclaration* parse_using_declaration()
	{
		DeclParse(using_declaration);

		if (!drop(KW_USING))
			return func.fail();

		QualifiedName* name = parse_qualified_name();

		if (!drop(OP_SEMICOLON))
			return func.fail();

		return new UsingDeclaration(name);
	}

	UsingDirective* parse_using_directive()
	{
		DeclParse(using_directive);

		if (!drop(KW_USING))
			return func.fail();

		if (!drop(KW_NAMESPACE))
			return func.fail();

		QualifiedName* name = parse_qualified_name();

		if (!name)
			return func.fail();

		if (!drop(OP_SEMICOLON))
			return func.fail();

		Namespace* ns = symtab.lookup_namespace(name);

		if (!ns)
			return func.fail();

		return new UsingDirective(ns);
	}

	InitDeclaratorList* parse_init_declarator_list()
	{
		DeclParse(init_declarator_list);

		InitDeclaratorList* init_declarator_list = new InitDeclaratorList;

		InitDeclarator* init_declarator = parse_init_declarator();

		if (!init_declarator)
			return func.fail();

		init_declarator_list->push_init_declarator(init_declarator);

		while (drop(OP_COMMA))
		{
			init_declarator = parse_init_declarator();

			if (!init_declarator)
				return func.fail();

			init_declarator_list->push_init_declarator(init_declarator);
		}

		return init_declarator_list;
	}

	InitDeclarator* parse_init_declarator()
	{
		DeclParse(init_declarator);

		Declarator* declarator = parse_declarator();

		if (!declarator)
			return func.fail();

		Initializer* initializer = parse_initializer();

		return new InitDeclarator(declarator, initializer);
	}

	Initializer* parse_initializer()
	{
		DeclParse(initializer);

		switch (peek())
		{
		case OP_LPAREN:
			{
				pop();

				func.nest();

				ExpressionList* expr_list = parse_expression_list();

				if (!expr_list)
					return func.fail();

				if (!drop(OP_RPAREN))
					return func.fail();

				func.unnest();

				return new DirectInitializer(expr_list);
			}
		case OP_ASS:
			{
				pop();

				RawExpression* expr = parse_assignment_expression();

				if (!expr)
				{
					return func.fail();
				}

				return new CopyInitializer(expr);
			}

		default:
			return func.fail();
		}
	}

	Declarator* parse_declarator(ScopeChanger* scope_changer = nullptr)
	{
		DeclParse(declarator);

		Declarator* declarator = new Declarator;

		bool prefix_done = false;

		while (!prefix_done)
		{
			switch (peek())
			{
			case OP_STAR:
			case OP_AMP:
			case OP_LAND:
				{
					PtrOperator* ptr_operator = parse_ptr_operator();

					if (!ptr_operator)
						return func.fail();

					declarator->push_ptr_operator(ptr_operator);

					break;
				}

			case OP_LPAREN:
				{
					pop();

					func.nest();

					Declarator* inner_declarator = parse_declarator();

					if (!inner_declarator)
						return func.fail();

					if (!drop(OP_RPAREN))
						return func.fail();

					func.unnest();

					declarator->push_inner_declarator(inner_declarator);

					prefix_done = true;
					break;
				}

			case OP_COLON2:
			case TT_IDENTIFIER:
				{
					QualifiedName* name = parse_qualified_name();

					if (!name)
						return func.fail();

					if (!name->unqualified())
					{
						if (!scope_changer)
							throw logic_error("qualified name not allowed in this context");

						set<Entity*> entities = scope_changer->enter_qualified_declarator_id(name);

						declarator->push_entities(entities);
					}

					declarator->push_declarator_id(name);
					prefix_done = true;
					break;
				}

			default:
				return func.fail();
			}
		}

		bool suffix_done = false;

		while (!suffix_done)
		{
			switch (peek())
			{
			case OP_LSQUARE:
				{
					pop();

					func.nest();

					RawExpression* expr = parse_expression();

					if (!drop(OP_RSQUARE))
						return func.fail();

					func.unnest();

					declarator->push_array(expr);
					break;
				}

			case OP_LPAREN:
				{
					size_t save = pos;

					pop();

					func.nest();

					ParameterDeclarationClause* parameter_declaration_clause = parse_parameter_declaration_clause();

					if (!drop(OP_RPAREN))
					{
						jump(save);
						func.unnest();
						suffix_done = true;
						break;
					}

					func.unnest();

					declarator->push_parameter_declaration_clause(parameter_declaration_clause);
					break;
				}

			default:
				suffix_done = true;
				break;
			}
		}

		return declarator;
	}

	PtrOperator* parse_ptr_operator()
	{
		DeclParse(ptr_operator);

		switch (peek())
		{
		case OP_STAR:
			if ((peek(1, KW_CONST) && peek(2, KW_VOLATILE)) || (peek(1, KW_VOLATILE) && peek(2, KW_CONST)))
			{
				drop(3);
				return new PtrOperator(PO_CONST_VOLATILE_POINTER);
			}
			else if (peek(1, KW_CONST))
			{
				drop(2);
				return new PtrOperator(PO_CONST_POINTER);
			}
			else if (peek(1, KW_VOLATILE))
			{
				drop(2);
				return new PtrOperator(PO_VOLATILE_POINTER);
			}
			else
			{
				drop(1);
				return new PtrOperator(PO_POINTER);
			}

		case OP_AMP:
			drop(1);
			return new PtrOperator(PO_LVALUE_REFERENCE);

		case OP_LAND:
			drop(1);
			return new PtrOperator(PO_RVALUE_REFERENCE);

		default:
			return func.fail();
		}
	}

	Type* parse_type_id()
	{
		DeclParse(type_id);

		DeclSpecifierSeq* type_specifier_seq = parse_type_specifier_seq();

		if (!type_specifier_seq)
			return func.fail();

		Declarator* declarator = parse_abstract_declarator();

		return symtab.make_type_id(type_specifier_seq, declarator);
	}

	ParameterDeclarationClause* parse_parameter_declaration_clause()
	{
		DeclParse(parameter_declaration_clause);

		ParameterDeclarationClause* parameter_declaration_clause = new ParameterDeclarationClause;

		if (peek(0, KW_VOID) && peek(1, OP_RPAREN))
		{
			drop();

			return parameter_declaration_clause;
		}

		if (drop(OP_DOTS))
		{
			parameter_declaration_clause->set_ellipse();
			return parameter_declaration_clause;
		}

		if (peek(OP_RPAREN))
			return parameter_declaration_clause;

		ParameterDeclaration* parameter_declaration = parse_parameter_declaration();

		if (!parameter_declaration)
			return func.fail();

		parameter_declaration_clause->push_parameter_declaration(parameter_declaration);

		while (true)
		{
			switch (peek())
			{
			case OP_RPAREN:
				return parameter_declaration_clause;

			case OP_COMMA:
				if (peek(1, OP_DOTS))
				{
					drop(2);
					parameter_declaration_clause->set_ellipse();

					return parameter_declaration_clause;
				}
				else
				{
					drop();

					ParameterDeclaration* parameter_declaration = parse_parameter_declaration();

					if (!parameter_declaration)
						return func.fail();

					parameter_declaration_clause->push_parameter_declaration(parameter_declaration);
				}
				break;

			case OP_DOTS:
				drop();
				parameter_declaration_clause->set_ellipse();
				return parameter_declaration_clause;

			default:
				return func.fail();
			}
		}
	}

	ParameterDeclaration* parse_parameter_declaration()
	{
		DeclParse(parameter_declaration);

		DeclSpecifierSeq* decl_specifier_seq = parse_decl_specifier_seq();

		if (!decl_specifier_seq)
			return func.fail();

		Declarator* declarator = parse_parameter_declarator();

		if (!declarator)
			return func.fail();

		if (!drop(OP_ASS))
			return new ParameterDeclaration(decl_specifier_seq, declarator);

		RawExpression* default_argument = parse_assignment_expression();

		if (!default_argument)
			return func.fail();

		return new ParameterDeclaration(decl_specifier_seq, declarator, default_argument);
	}

	Declarator* parse_parameter_declarator()
	{
		DeclParse(parameter_declarator);

		Declarator* declarator = new Declarator;

		bool prefix_done = false;

		while (!prefix_done)
		{
			switch (peek())
			{
			case OP_COMMA:
			case OP_DOTS:
			case OP_RPAREN:
				return declarator;

			case OP_STAR:
			case OP_AMP:
			case OP_LAND:
				{
					PtrOperator* ptr_operator = parse_ptr_operator();

					if (!ptr_operator)
						return func.fail();

					declarator->push_ptr_operator(ptr_operator);

					break;
				}

			case OP_LPAREN:
				{
					pop();
					func.nest();

					ParameterDeclarationClause* parameter_declaration_clause = parse_parameter_declaration_clause();

					if (parameter_declaration_clause && drop(OP_RPAREN))
					{
						func.unnest();
						declarator->push_parameter_declaration_clause(parameter_declaration_clause);
					}
					else
					{
						Declarator* inner_declarator = parse_parameter_declarator();

						if (!inner_declarator)
							return func.fail();

						if (!drop(OP_RPAREN))
							return func.fail();

						func.unnest();

						declarator->push_inner_declarator(inner_declarator);
					}

					prefix_done = true;
					break;
				}

			case OP_COLON2:
			case TT_IDENTIFIER:
				{
					QualifiedName* name = parse_qualified_name();

					if (!name)
						return func.fail();

					declarator->push_declarator_id(name);
					prefix_done = true;
					break;
				}

			default:
				return func.fail();
			}
		}

		bool suffix_done = false;

		while (!suffix_done)
		{
			switch (peek())
			{
			case OP_LSQUARE:
				{
					pop();

					func.nest();

					RawExpression* expr = parse_expression();

					if (!drop(OP_RSQUARE))
						return func.fail();

					func.unnest();

					declarator->push_array(expr);
					break;
				}

			case OP_LPAREN:
				{
					size_t save = pos;
					pop();
					func.nest();

					ParameterDeclarationClause* parameter_declaration_clause = parse_parameter_declaration_clause();

					if (!parameter_declaration_clause || !drop(OP_RPAREN))
					{
						jump(save);
						func.unnest();
						suffix_done = true;
						break;
					}

					func.unnest();

					declarator->push_parameter_declaration_clause(parameter_declaration_clause);
					break;
				}

			default:
				suffix_done = true;
				break;
			}
		}

		return declarator;

	}

	Declarator* parse_abstract_declarator()
	{
		DeclParse(abstract_declarator);

		Declarator* declarator = new Declarator;

		bool prefix_done = false;

		while (!prefix_done)
		{
			switch (peek())
			{
			case OP_ASS:
			case OP_COMMA:
			case OP_DOTS:
			case OP_GT:
			case OP_RPAREN:
			case OP_SEMICOLON:
			case ST_RSHIFT_1:
			case ST_RSHIFT_2:
				return declarator;

			case OP_STAR:
			case OP_AMP:
			case OP_LAND:
				{
					PtrOperator* ptr_operator = parse_ptr_operator();

					if (!ptr_operator)
						return func.fail();

					declarator->push_ptr_operator(ptr_operator);

					break;
				}

			case OP_LPAREN:
				{
					pop();
					func.nest();

					ParameterDeclarationClause* parameter_declaration_clause = parse_parameter_declaration_clause();

					if (parameter_declaration_clause && drop(OP_RPAREN))
					{
						func.unnest();
						declarator->push_parameter_declaration_clause(parameter_declaration_clause);
					}
					else
					{
						Declarator* inner_declarator = parse_abstract_declarator();

						if (!inner_declarator)
							return func.fail();

						if (!drop(OP_RPAREN))
							return func.fail();

						func.unnest();

						declarator->push_inner_declarator(inner_declarator);
					}

					prefix_done = true;
					break;
				}

			case OP_COLON2:
			case TT_IDENTIFIER:
				{
					QualifiedName* name = parse_qualified_name();

					if (!name)
						return func.fail();

					declarator->push_declarator_id(name);
					prefix_done = true;
					break;
				}

			default:
				return func.fail();
			}
		}

		bool suffix_done = false;

		while (!suffix_done)
		{
			switch (peek())
			{
			case OP_LSQUARE:
				{
					pop();

					func.nest();

					RawExpression* expr = parse_expression();

					if (!drop(OP_RSQUARE))
						return func.fail();

					func.unnest();

					declarator->push_array(expr);
					break;
				}

			case OP_LPAREN:
				{
					size_t save = pos;
					pop();
					func.nest();

					ParameterDeclarationClause* parameter_declaration_clause = parse_parameter_declaration_clause();

					if (!parameter_declaration_clause || !drop(OP_RPAREN))
					{
						jump(save);
						func.unnest();
						suffix_done = true;
						break;
					}

					func.unnest();

					declarator->push_parameter_declaration_clause(parameter_declaration_clause);
					break;
				}

			default:
				suffix_done = true;
				break;
			}
		}

		return declarator;

	}

	bool parse_close_angle_bracket()
	{
		DeclParse(close_angle_bracket);

		switch (peek())
		{
		case OP_GT:
		case ST_RSHIFT_1:
		case ST_RSHIFT_2:
			pop();
			return true;

		default:
			return func.fail();
		}
	}
};


SemC::SemC()
	: parser(new SemCParser)
{}

SemC::~SemC()
{
	delete parser;
}

void SemC::parse(const string& srcfile, vector<PToken>&& tokens)
{
	parser->parse(srcfile, move(tokens));
}

void SemC::dump(ostream& out)
{
	parser->symtab.dump(out);
}

void SemC::mock_nsinit(ostream& out)
{
	parser->symtab.mock_nsinit(out);
}

pair<string, size_t> SemC::error_loc()
{
	return parser->get_deepest();
}

