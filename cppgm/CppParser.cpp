#include "std.pch"

#include "CppParser.h"
#include "AST.h"

bool PA6_IsClassName(const string& identifier)
{
	return identifier.find('C') != string::npos;
}

bool PA6_IsTemplateName(const string& identifier)
{
	return identifier.find('T') != string::npos;
}

bool PA6_IsTypedefName(const string& identifier)
{
	return identifier.find('Y') != string::npos;
}

bool PA6_IsEnumName(const string& identifier)
{
	return identifier.find('E') != string::npos;
}

bool PA6_IsNamespaceName(const string& identifier)
{
	return identifier.find('N') != string::npos;
}

struct CppParser
{
	vector<PToken> tokens;
	size_t pos = 0;

	vector<pair<string,size_t>> parse_stack;

	size_t deepest_pos = 0;
	vector<pair<string,size_t>> deepest_parse_stack;

	bool trace;

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
		CppParser* parser;
		size_t start;
		string function;
		bool failed = false;

		ParseFunction(CppParser* parser, const string& function)
			: parser(parser)
			, function(function)
		{
			if (parser->trace)
			{
				cout << "TRACE: " << string(2*parser->parse_stack.size(), ' ') << " --> " << function << " ";
				parser->peekt()->write(cout);
				cout << endl;
			}

			parser->enter_parse_stack(function);
			start = parser->pos;
		}

		~ParseFunction()
		{
			parser->exit_parse_stack();

			if (parser->trace)
			{
				cout << "TRACE: " << string(2*parser->parse_stack.size(), ' ') << " <-- " << (failed ? "BAD" : "OK") << " " << function << " ";
				parser->peekt()->write(cout);
				cout << endl;
			}

			unnest();
		}

		AST fail()
		{
			parser->jump(start);

			failed = true;

			return nullptr;
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

	CppParser(vector<PToken>&& tokens, bool trace)
		: tokens(move(tokens))
		, trace(trace)
	{}

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

	AST maybe(ETokenType token_type)
	{
		if (peek(token_type))
			return pop();
		else
			return nullptr;
	}

    #define DeclParse(x) ParseFunction func(this, #x); AST x;

	template<class F>
	AST parse_star(F f)
	{
		DeclParse(star);

		for (size_t i = 0; true; i++)
		{
			auto x = (this->*f)();

			if (!x)
			{
				if (i == 0)
					return astempty;
				else
					return star;
			}

			star[i] = x;
		}
	}

	template<class F>
	AST parse_plus(F f)
	{
		DeclParse(plus);

		for (size_t i = 0; true; i++)
		{
			auto x = (this->*f)();

			if (!x)
			{
				if (i == 0)
					return func.fail();
				else
					return plus;
			}

			plus[i] = x;
		}
	}

	// ====================== BEGIN PARSE FUNCTIONS =============================

	AST parse_namespace_name()
	{
		DeclParse(namespace_name);

		if (!peek(TT_IDENTIFIER))
			return func.fail();

		if (!PA6_IsNamespaceName(peekt()->src.spelling))
			return func.fail();

		return pop();
	}

	AST parse_translation_unit()
	{
		DeclParse(translation_unit);

		translation_unit = parse_star(&CppParser::parse_declaration);

		if (!drop(TT_EOF))
			return func.fail();

		return translation_unit;
	}

	AST parse_primary_expression()
	{
		DeclParse(primary_expression);

		switch (peek())
		{
		case KW_TRUE:
		case KW_FALSE:
		case KW_NULLPTR:
		case TT_LITERAL:
		case TT_UDSTRING:
		case TT_UDCHARACTER:
		case TT_UDINTEGER:
		case TT_UDFLOATING:
		case KW_THIS:
			return pop();

		case OP_LPAREN:
			drop();
			func.nest();
			{
				auto expression = parse_expression();
				if (!expression || !drop(OP_RPAREN))
					return func.fail();

				return expression;
			}
		}

		if (primary_expression["id_expression"] = parse_id_expression())
			return primary_expression;

		if (primary_expression["lambda-expression"] = parse_lambda_expression())
			return primary_expression;

		return func.fail();
	}

	AST parse_type_name()
	{
		DeclParse(type_name);

		if (!peek(TT_IDENTIFIER))
			return func.fail();

		if (type_name["class-name"] = parse_class_name())
			return type_name;
		if (type_name["simple-template-id"] = parse_simple_template_id())
			return type_name;
		else if (type_name["enum-name"] = parse_enum_name())
			return type_name;
		else if (type_name["typedef-name"] = parse_typedef_name())
			return type_name;
		else
			return func.fail();

		auto simple_template_id = parse_simple_template_id();

		if (simple_template_id)
		{
			type_name["simple-template-id"] = simple_template_id;
			return type_name;
		}

		type_name["TT_IDENTIFIER"] = pop();

		return type_name;
	}

	AST parse_enum_name()
	{
		DeclParse(enum_name);

		if (!peek(TT_IDENTIFIER))
			return func.fail();

		if (!PA6_IsEnumName(peekt()->src.spelling))
			return func.fail();

		return pop();
	}

	AST parse_typedef_name()
	{
		DeclParse(typedef_name);

		if (!peek(TT_IDENTIFIER))
			return func.fail();

		if (!PA6_IsTypedefName(peekt()->src.spelling))
			return func.fail();

		return pop();
	}

	AST parse_class_name()
	{
		DeclParse(type_name);

		if (!peek(TT_IDENTIFIER))
			return func.fail();

		if (!PA6_IsClassName(peekt()->src.spelling))
			return func.fail();

		auto simple_template_id = parse_simple_template_id();

		if (simple_template_id)
		{
			type_name["simple-template-id"] = simple_template_id;
			return type_name;
		}

		type_name["TT_IDENTIFIER"] = pop();

		return type_name;
	}

	AST parse_simple_template_id()
	{
		DeclParse(simple_template_id);

		if (!peek(TT_IDENTIFIER))
			return func.fail();

		if (!PA6_IsTemplateName(peekt()->src.spelling))
			return func.fail();

		simple_template_id["TT_IDENTIFIER"] = pop();

		if (!drop(OP_LT))
			return func.fail();

		func.nest(true);

		if (!(simple_template_id["template-argument-list"] = parse_template_argument_list()))
			simple_template_id["template-argument-list"] = astempty;

		if (!parse_close_angle_bracket())
			return func.fail();

		func.unnest();

		return simple_template_id;
	}

	AST parse_id_expression()
	{
		DeclParse(id_expression);

		if (id_expression["qualified-id"] = parse_qualified_id())
			return id_expression;

		if (id_expression["unqualified-id"] = parse_unqualified_id())
			return id_expression;

		return func.fail();
	}

	AST parse_unqualified_id()
	{
		DeclParse(unqualified_id);

		switch (peek())
		{
		case KW_OPERATOR:
			if (unqualified_id["template-id"] = parse_template_id())
				return unqualified_id;

			if (unqualified_id["operator-function-id"] = parse_operator_function_id())
				return unqualified_id;

			if (unqualified_id["conversion-function-id"] = parse_conversion_function_id())
				return unqualified_id;

			if (unqualified_id["literal-operator-id"] = parse_literal_operator_id())
				return unqualified_id;

			return func.fail();

		case OP_COMPL:
			unqualified_id["OP_COMPL"] = pop();

			if (unqualified_id["decltype-specifier"] = parse_decltype_specifier())
				return unqualified_id;

			if (unqualified_id["class-name"] = parse_class_name())
				return unqualified_id;

			return func.fail();

		case TT_IDENTIFIER:
			if (unqualified_id["template-id"] = parse_template_id())
				return unqualified_id;

			unqualified_id["TT_IDENTIFIER"] = pop();

			return unqualified_id;

		default:
			return func.fail();
		}
	}

	AST parse_qualified_id()
	{
		DeclParse(qualified_id);

		if (qualified_id["nested-name-specifier"] = parse_nested_name_specifier())
		{
			qualified_id["KW_TEMPLATE"] = maybe(KW_TEMPLATE);
			if (qualified_id["unqualified-id"] = parse_unqualified_id())
				return qualified_id;

			return func.fail();
		}

		if (!peek(OP_COLON2))
			return func.fail();

		qualified_id["OP_COLON2"] = pop();

		switch (peek())
		{
		case KW_OPERATOR:
			if (qualified_id["template-id"] = parse_template_id())
				return qualified_id;

			if (qualified_id["operator-function-id"] = parse_operator_function_id())
				return qualified_id;

			if (qualified_id["conversion-function-id"] = parse_conversion_function_id())
				return qualified_id;

			if (qualified_id["literal-operator-id"] = parse_literal_operator_id())
				return qualified_id;

			return func.fail();

		case TT_IDENTIFIER:
			if (qualified_id["template-id"] = parse_template_id())
				return qualified_id;

			qualified_id["TT_IDENTIFIER"] = pop();

			return qualified_id;

		default:
			return func.fail();
		}
	}

	AST parse_nested_name_specifier()
	{
		DeclParse(nested_name_specifier);

		if (!(nested_name_specifier["nested-name-specifier-root"] = parse_nested_name_specifier_root()))
			return func.fail();

		nested_name_specifier["nested-name-specififier-suffix-seq"] =
			parse_star(&CppParser::parse_nested_name_specifier_suffix);

		return nested_name_specifier;
	}

	AST parse_nested_name_specifier_root()
	{
		DeclParse(nested_name_specifier_root);

		switch (peek())
		{
		case KW_DECLTYPE:
			if (!(nested_name_specifier_root["decltype-specifier"] = parse_decltype_specifier()))
				return func.fail();

			if (!drop(OP_COLON2))
				return func.fail();

			return nested_name_specifier_root;

		case OP_COLON2:
			nested_name_specifier_root["OP_COLON2"] = pop();

		default:
			if (nested_name_specifier_root["type-name"] = parse_type_name())
				break;

			if (nested_name_specifier_root["namespace-name"] = parse_namespace_name())
				break;

			return func.fail();
		}

		if (!drop(OP_COLON2))
			return func.fail();

		return nested_name_specifier_root;
	}

	AST parse_nested_name_specifier_suffix()
	{
		DeclParse(nested_name_specifier_suffix);

		switch (peek())
		{
		case KW_TEMPLATE:
			nested_name_specifier_suffix["KW_TEMPLATE"] = pop();

			if (nested_name_specifier_suffix["simple-template-id"] = parse_simple_template_id())
				break;

			return func.fail();

		case TT_IDENTIFIER:

			if (nested_name_specifier_suffix["simple-template-id"] = parse_simple_template_id())
				break;

			nested_name_specifier_suffix["TT_IDENTIFIER"] = pop();

			break;

		default:
			return func.fail();
		}

		if (!drop(OP_COLON2))
			return func.fail();

		return nested_name_specifier_suffix;
	}

	AST parse_lambda_expression()
	{
		DeclParse(lambda_expression);

		auto lambda_introducer = parse_lambda_introducer();

		if (!lambda_introducer)
			return func.fail();

		lambda_expression["lambda-introducer"] = lambda_introducer;
		lambda_expression["lambda-declarator"] = parse_lambda_declarator();

		auto compound_statement = parse_compound_statement();

		if (!compound_statement)
			return func.fail();

		lambda_expression["compound-statement"] = compound_statement;

		return lambda_expression;
	}

	AST parse_lambda_introducer()
	{
		DeclParse(lambda_introducer);

		if (!drop(OP_LSQUARE))
			return func.fail();

		auto lambda_capture = parse_lambda_capture();

		if (!lambda_capture)
			lambda_capture = astempty;

		lambda_introducer["lambda-capture"] = lambda_capture;

		if (!drop(OP_RSQUARE))
			return func.fail();

		return lambda_introducer;
	}

	AST parse_lambda_capture()
	{
		DeclParse(lambda_capture);

		auto capture_list = parse_capture_list();

		if (capture_list)
		{
			lambda_capture["capture-list"] = capture_list;
			return lambda_capture;
		}

		auto capture_default = parse_capture_default();

		if (!capture_default)
			return func.fail();

		lambda_capture["capture-default"] = capture_default;

		if (!drop(OP_COMMA))
			return lambda_capture;

		capture_list = parse_capture_list();

		if (!capture_list)
			return func.fail();

		lambda_capture["capture-list"] = capture_list;
		return lambda_capture;
	}

	AST parse_capture_default()
	{
		DeclParse(capture_default);

		switch (peek())
		{
		case OP_AMP:
		case OP_ASS:
			return pop();

		default:
			return func.fail();
		}
	}

	AST parse_capture_list()
	{
		DeclParse(capture_list);

		auto capture_dots = parse_capture_dots();

		if (!capture_dots)
			return func.fail();

		capture_list[0] = capture_dots;

		size_t i = 1;
		while (peek(OP_COMMA))
		{
			drop();

			capture_dots = parse_capture_dots();

			if (!capture_dots)
				return func.fail();

			capture_list[i++] = capture_dots;
		}

		return capture_list;
	}

	AST parse_capture_dots()
	{
		DeclParse(capture_dots);

		auto capture = parse_capture();

		if (!capture)
			return func.fail();

		capture_dots["capture"] = capture;

		if (peek(OP_DOTS))
			capture_dots["OP_DOTS"] = pop();

		return capture_dots;
	}

	AST parse_capture()
	{
		DeclParse(capture);

		switch (peek())
		{
		case KW_THIS:
			return pop();

		case OP_AMP:
			capture["OP_AMP"] = pop();
			// fallthrough

		default:
			if (!peek(TT_IDENTIFIER))
				return func.fail();

			capture["TT_IDENTIFIER"] = pop();

			return capture;
		}
	}

	AST parse_lambda_declarator()
	{
		DeclParse(lambda_declarator);

		if (!drop(OP_LPAREN))
			return func.fail();

		func.nest();

		lambda_declarator["parameter-declaration-clause"] = parse_parameter_declaration_clause();

		if (!drop(OP_RPAREN))
			return func.fail();

		func.unnest();


		lambda_declarator["KW_MUTABLE"] = maybe(KW_MUTABLE);
		lambda_declarator["exception-specification"] = parse_exception_specification();
		lambda_declarator["attribute-specifier-seq"] = parse_attribute_specifier_seq();
		lambda_declarator["trailing-return-type"] = parse_trailing_return_type();

		return lambda_declarator;
	}

	AST parse_postfix_expression()
	{
		DeclParse(postfix_expression);

		if (!(postfix_expression["postfix-root"] = parse_postfix_root()))
			return func.fail();

		if ((postfix_expression["postfix-suffix"] = parse_star(&CppParser::parse_postfix_suffix)).empty())
			return postfix_expression["postfix-root"];

		return postfix_expression;
	}

	AST parse_postfix_root()
	{
		DeclParse(postfix_root);

		switch (peek())
		{
		case KW_DYNAMIC_CAST:
		case KW_STATIC_CAST:
		case KW_REINTERPET_CAST:
		case KW_CONST_CAST:
			postfix_root["cast"] = pop();

			if (!drop(OP_LT))
				return func.fail();

			func.nest(true);

			if (!(postfix_root["type-id"] = parse_type_id()))
				return func.fail();

			if (!parse_close_angle_bracket())
				return func.fail();

			func.unnest();

			if (!drop(OP_LPAREN))
				return func.fail();

			func.nest();

			if (!(postfix_root["expression"] = parse_expression()))
				return func.fail();

			if (!drop(OP_RPAREN))
				return func.fail();

			func.unnest();

			return postfix_root;

		case KW_TYPEID:
			postfix_root["KW_TYPEID"] = pop();
			{

				if (!drop(OP_LPAREN))
					return func.fail();

				size_t save = pos;

				func.nest();

				if ((postfix_root["type-id"] = parse_type_id()) && drop(OP_RPAREN))
				{
					func.unnest();
					return postfix_root;
				}

				jump(save);

				if ((postfix_root["expression"] = parse_expression()) && drop(OP_RPAREN))
				{
					func.unnest();
					return postfix_root;
				}
			}
			return func.fail();
		}

		size_t save = pos;

		auto simple_type_specifier = parse_simple_type_specifier();

		if (simple_type_specifier)
		{
			auto braced_init_list = parse_braced_init_list();

			if (braced_init_list)
			{
				postfix_root["simple-type-specifier"] = simple_type_specifier;
				postfix_root["braced-init-list"] = braced_init_list;
				return postfix_root;
			}

			if (drop(OP_LPAREN))
			{
				func.nest();

				auto expression_list = parse_expression_list();

				if (drop(OP_RPAREN))
				{
					func.unnest();

					postfix_root["simple-type-specifier"] = simple_type_specifier;
					postfix_root["expression-list"] = expression_list;
					return postfix_root;
				}
				func.unnest();
			}

			jump(save);
		}

		auto typename_specifier = parse_typename_specifier();

		if (typename_specifier)
		{
			auto braced_init_list = parse_braced_init_list();

			if (braced_init_list)
			{
				postfix_root["typename-specifier"] = typename_specifier;
				postfix_root["braced-init-list"] = braced_init_list;
				return postfix_root;
			}

			if (drop(OP_LPAREN))
			{
				func.nest();

				auto expression_list = parse_expression_list();

				if (drop(OP_RPAREN))
				{
					func.unnest();

					postfix_root["typename-specifier"] = typename_specifier;
					postfix_root["expression-list"] = expression_list;
					return postfix_root;
				}

				func.unnest();
			}

			jump(save);
		}

		if (!(postfix_root["primary-expression"] = parse_primary_expression()))
			return func.fail();

		return postfix_root;
	}

	AST parse_postfix_suffix()
	{
		DeclParse(postfix_suffix);

		switch (peek())
		{
		case OP_LSQUARE:
			postfix_suffix["op"] = pop();
			func.nest();

			if ((postfix_suffix["braced-init-list"] = parse_braced_init_list()) ||
				(postfix_suffix["expression"] = parse_expression()))
				if (drop(OP_RSQUARE))
				{
					func.unnest();
					return postfix_suffix;
				}

			return func.fail();

		case OP_LPAREN:
			postfix_suffix["op"] = pop();
			func.nest();

			postfix_suffix["expression-list"] = parse_expression_list();
			if (drop(OP_RPAREN))
				return postfix_suffix;

			return func.fail();

		case OP_DOT:
		case OP_ARROW:
			postfix_suffix["op"] = pop();
			if (postfix_suffix["pseudo-destructor-name"] = parse_pseudo_destructor_name())
				return postfix_suffix;

			postfix_suffix["KW_TEMPLATE"] = maybe(KW_TEMPLATE);

			if (postfix_suffix["id-expression"] = parse_id_expression())
				return postfix_suffix;

			return func.fail();

		case OP_INC:
		case OP_DEC:
			postfix_suffix["op"] = pop();
			return postfix_suffix;

		default:
			return func.fail();
		}
	}

	AST parse_expression_list()
	{
		return parse_initializer_list();
	}

	AST parse_pseudo_destructor_name()
	{
		DeclParse(pseudo_destructor_name);

		if (!(pseudo_destructor_name["nested-name-specifier"] = parse_nested_name_specifier()))
		{
			if (!drop(OP_COMPL))
				return func.fail();

			if (pseudo_destructor_name["decltype-specifier"] = parse_decltype_specifier())
				return pseudo_destructor_name;

		}
		else
		{
			if (!drop(OP_COMPL))
				return func.fail();
		}

		if (pseudo_destructor_name["type-name"] = parse_type_name())
			return pseudo_destructor_name;

		return func.fail();
	}

	AST parse_unary_expression()
	{
		DeclParse(unary_expression);

		auto postfix_expression = parse_postfix_expression();

		if (postfix_expression)
			return postfix_expression;

		if (unary_expression["unary-operator"] = parse_unary_operator())
		{
			if (unary_expression["cast-expression"] = parse_cast_expression())
				return unary_expression;
			else
				return func.fail();
		}

		if (peek(KW_SIZEOF))
		{
			unary_expression["KW_SIZEOF"] = pop();

			if (unary_expression["OP_DOTS"] = maybe(OP_DOTS))
			{
				if (!drop(OP_LPAREN))
					return func.fail();

				func.nest();

				if (!peek(TT_IDENTIFIER))
					return func.fail();

				unary_expression["TT_IDENTIFIER"] = pop();

				if (!drop(OP_RPAREN))
					return func.fail();

				func.unnest();

				return unary_expression;

			}

			size_t x = pos;

			if (peek(OP_LPAREN))
			{
				drop();
				func.nest();

				auto type_id = parse_type_id();
				if (type_id && drop(OP_RPAREN))
				{
					unary_expression["type-id"] = type_id;
					return unary_expression;
				}

				func.unnest();
			}

			jump(x);

			if (unary_expression["unary-expression"] = parse_unary_expression())
				return unary_expression;
			else
				return func.fail();
		}

		if (peek(KW_ALIGNOF))
		{
			unary_expression["KW_ALIGNOF"] = pop();

			if (!drop(OP_LPAREN))
				return func.fail();

			func.nest();

			if (!(unary_expression["type-id"] = parse_type_id()))
				return func.fail();

			if (!drop(OP_RPAREN))
				return func.fail();

			func.unnest();

			return unary_expression;
		}

		if (unary_expression["noexcept-expression"] = parse_noexcept_expression())
			return unary_expression;

		if (unary_expression["new-expression"] = parse_new_expression())
			return unary_expression;

		if (unary_expression["delete-expression"] = parse_delete_expression())
			return unary_expression;

		return func.fail();
	}

	AST parse_unary_operator()
	{
		DeclParse(unary_operator);

		switch (peek())
		{
		case OP_INC:
		case OP_DEC:
		case OP_STAR:
		case OP_AMP:
		case OP_PLUS:
		case OP_MINUS:
		case OP_LNOT:
		case OP_COMPL:
			return pop();

		default:
			return func.fail();
		}
	}

	AST parse_new_expression()
	{
		DeclParse(new_expression);

		new_expression["OP_COLON2"] = maybe(OP_COLON2);

		if (!drop(KW_NEW))
			return func.fail();

		new_expression["new-placement"] = parse_new_placement();

		if (drop(OP_LPAREN))
		{
			func.nest();

			if (!(new_expression["type-id"] = parse_type_id()))
				return func.fail();

			if (!drop(OP_RPAREN))
				return func.fail();

			func.unnest();
		}
		else
		{
			if (!(new_expression["new-type-id"] = parse_new_type_id()))
				return func.fail();
		}

		new_expression["new-initializer"] = parse_new_initializer();

		return new_expression;
	}

	AST parse_new_placement()
	{
		DeclParse(new_placement);

		if (!drop(OP_LPAREN))
			return func.fail();

		func.nest();

		if (!(new_placement["expression-list"] = parse_expression_list()))
			return func.fail();

		if (!drop(OP_RPAREN))
			return func.fail();

		func.unnest();

		return new_placement;
	}

	AST parse_new_type_id()
	{
		DeclParse(new_type_id);

		if (!(new_type_id["type-specifier-seq"] = parse_plus(&CppParser::parse_type_specifier)))
			return func.fail();

		new_type_id["attribute-specifier-seq"] = parse_attribute_specifier_seq();

		new_type_id["new-declarator"] = parse_new_declarator();

		return new_type_id;
	}

	AST parse_new_declarator()
	{
		DeclParse(new_declarator);

		new_declarator["ptr-operator-seq"] = parse_star(&CppParser::parse_ptr_operator);
		new_declarator["noptr-new-declarator"] = parse_noptr_new_declarator();

		if (!new_declarator)
			return func.fail();

		return new_declarator;
	}

	AST parse_noptr_new_declarator()
	{
		DeclParse(noptr_new_declarator);

		if (!drop(OP_LSQUARE))
			return func.fail();

		func.nest();

		if (!(noptr_new_declarator["expression"] = parse_expression()))
			return func.fail();

		if (!drop(OP_RSQUARE))
			return func.fail();

		func.unnest();

		noptr_new_declarator["attribute-specifier-seq"] = parse_attribute_specifier_seq();

		noptr_new_declarator["noptr-new-declarator"] = parse_noptr_new_declarator();

		return noptr_new_declarator;
	}

	AST parse_new_initializer()
	{
		DeclParse(new_initializer);

		switch (peek())
		{
		case OP_LPAREN:
			new_initializer["OP_LPAREN"] = pop();
			func.nest();

			new_initializer["expression-list"] = parse_expression_list();

			if (!drop(OP_RPAREN))
				return func.fail();

			return new_initializer;

		case OP_LBRACE:
			if (new_initializer["braced-init-list"] = parse_braced_init_list())
				return new_initializer;
			else
				return func.fail();

		default:
			return func.fail();
		}
	}

	AST parse_delete_expression()
	{
		DeclParse(delete_expression);

		delete_expression["OP_COLON2"] = maybe(OP_COLON2);

		if (!drop(KW_DELETE))
			return func.fail();

		if (peek(OP_LSQUARE))
		{
			delete_expression["OP_LSQUARE"] = pop();

			func.nest();

			if (!drop(OP_RSQUARE))
				return func.fail();

			func.unnest();
		}

		if (delete_expression["cast-expression"] = parse_cast_expression())
			return delete_expression;
		else
			return func.fail();
	}

	AST parse_noexcept_expression()
	{
		DeclParse(noexcept_expression);

		if (!drop(KW_NOEXCEPT))
			return func.fail();

		if (!drop(OP_LPAREN))
			return func.fail();

		func.nest();

		if (!(noexcept_expression["expression"] = parse_expression()))
			return func.fail();

		if (!drop(OP_RPAREN))
			return func.fail();

		func.unnest();

		return noexcept_expression;
	}

	AST parse_cast_expression()
	{
		DeclParse(cast_expression);

		if (cast_expression["cast-operator"] = parse_cast_operator())
		{
			if (cast_expression["cast-expression"] = parse_cast_expression())
				return cast_expression;
			else
				return func.fail();
		}
		else
			return parse_unary_expression();
	}

	AST parse_cast_operator()
	{
		DeclParse(cast_operator);

		if (!drop(OP_LPAREN))
			return func.fail();

		func.nest();

		if (!(cast_operator["type-id"] = parse_type_id()))
			return func.fail();

		if (!drop(OP_RPAREN))
			return func.fail();

		func.unnest();

		return cast_operator;
	}

	AST parse_pm_expression()
	{
		DeclParse(pm_expression);

		if (!(pm_expression = parse_cast_expression()))
			return func.fail();

		while (true)
		{
			AST next_pm_expression;

			size_t start_next = pos;

			if (!(next_pm_expression["pm-operator"] = parse_pm_operator()))
				return pm_expression;

			if (!(next_pm_expression["rhs"] = parse_cast_expression()))
			{
				jump(start_next);
				return pm_expression;
			}

			next_pm_expression["lhs"] = move(pm_expression);

			swap(next_pm_expression, pm_expression);
		}
	}

	AST parse_pm_operator()
	{
		DeclParse(pm_operator);

		switch (peek())
		{
		case OP_DOTSTAR:
		case OP_ARROWSTAR:
			return pop();

		default:
			return func.fail();
		}
	}

	AST parse_multiplicative_expression()
	{
		DeclParse(multiplicative_expression);

		if (!(multiplicative_expression = parse_pm_expression()))
			return func.fail();

		while (true)
		{
			AST next_multiplicative_expression;

			size_t start_next = pos;

			if (!(next_multiplicative_expression["multiplicative-operator"] = parse_multiplicative_operator()))
				return multiplicative_expression;

			if (!(next_multiplicative_expression["rhs"] = parse_pm_expression()))
			{
				jump(start_next);
				return multiplicative_expression;
			}

			next_multiplicative_expression["lhs"] = move(multiplicative_expression);

			swap(next_multiplicative_expression, multiplicative_expression);
		}
	}

	AST parse_multiplicative_operator()
	{
		DeclParse(multiplicative_operator);

		switch (peek())
		{
		case OP_STAR:
		case OP_DIV:
		case OP_MOD:
			return pop();

		default:
			return func.fail();
		}
	}

	AST parse_additive_expression()
	{
		DeclParse(additive_expression);

		if (!(additive_expression = parse_multiplicative_expression()))
			return func.fail();

		while (true)
		{
			AST next_additive_expression;

			size_t start_next = pos;

			if (!(next_additive_expression["additive-operator"] = parse_additive_operator()))
				return additive_expression;

			if (!(next_additive_expression["rhs"] = parse_multiplicative_expression()))
			{
				jump(start_next);
				return additive_expression;
			}

			next_additive_expression["lhs"] = move(additive_expression);

			swap(next_additive_expression, additive_expression);
		}
	}

	AST parse_additive_operator()
	{
		DeclParse(additive_operator);

		switch (peek())
		{
		case OP_PLUS:
		case OP_MINUS:
			return pop();

		default:
			return func.fail();
		}
	}

	AST parse_shift_expression()
	{
		DeclParse(shift_expression);

		if (!(shift_expression = parse_additive_expression()))
			return func.fail();

		while (true)
		{
			AST next_shift_expression;

			size_t start_next = pos;

			if (!(next_shift_expression["shift-operator"] = parse_shift_operator()))
				return shift_expression;

			if (!(next_shift_expression["rhs"] = parse_additive_expression()))
			{
				jump(start_next);
				return shift_expression;
			}

			next_shift_expression["lhs"] = move(shift_expression);

			swap(next_shift_expression, shift_expression);
		}
	}

	AST parse_shift_operator()
	{
		DeclParse(shift_operator);

		if (peek(OP_LSHIFT))
			return pop();

		if (in_angle)
			return func.fail();

		if (peek(0, ST_RSHIFT_1) && peek(1, ST_RSHIFT_2))
		{
			shift_operator["OP_RSHIFT"] = pop();
			drop();

			return shift_operator;
		}

		return func.fail();
	}

	AST parse_relational_expression()
	{
		DeclParse(relational_expression);

		if (!(relational_expression = parse_shift_expression()))
			return func.fail();

		while (true)
		{
			AST next_relational_expression;

			size_t start_next = pos;

			if (!(next_relational_expression["relational-operator"] = parse_relational_operator()))
				return relational_expression;

			if (!(next_relational_expression["rhs"] = parse_shift_expression()))
			{
				jump(start_next);
				return relational_expression;
			}

			next_relational_expression["lhs"] = move(relational_expression);

			swap(next_relational_expression, relational_expression);
		}
	}

	AST parse_relational_operator()
	{
		DeclParse(relational_operator);

		switch (peek())
		{
		case OP_GT:
			if (in_angle)
				return func.fail();

		case OP_LT:
		case OP_LE:
		case OP_GE:
			return pop();

		default:
			return func.fail();
		}
	}

	AST parse_equality_expression()
	{
		DeclParse(equality_expression);

		if (!(equality_expression = parse_relational_expression()))
			return func.fail();

		while (true)
		{
			AST next_equality_expression;

			size_t start_next = pos;

			if (!(next_equality_expression["equality-operator"] = parse_equality_operator()))
				return equality_expression;

			if (!(next_equality_expression["rhs"] = parse_relational_expression()))
			{
				jump(start_next);
				return equality_expression;
			}

			next_equality_expression["lhs"] = move(equality_expression);

			swap(next_equality_expression, equality_expression);
		}
	}

	AST parse_equality_operator()
	{
		DeclParse(equality_operator);

		switch (peek())
		{
		case OP_EQ:
		case OP_NE:
			return pop();

		default:
			return func.fail();
		}
	}

	AST parse_and_expression()
	{
		DeclParse(and_expression);

		if (!(and_expression = parse_equality_expression()))
			return func.fail();

		while (true)
		{
			AST next_and_expression;

			size_t start_next = pos;

			if (!(next_and_expression["and-operator"] = maybe(OP_AMP)))
				return and_expression;

			if (!(next_and_expression["rhs"] = parse_equality_expression()))
			{
				jump(start_next);
				return and_expression;
			}

			next_and_expression["lhs"] = move(and_expression);

			swap(next_and_expression, and_expression);
		}
	}

	AST parse_exclusive_or_expression()
	{
		DeclParse(exclusive_or_expression);

		if (!(exclusive_or_expression = parse_and_expression()))
			return func.fail();

		while (true)
		{
			AST next_exclusive_or_expression;

			size_t start_next = pos;

			if (!(next_exclusive_or_expression["exclusive-or-operator"] = maybe(OP_XOR)))
				return exclusive_or_expression;

			if (!(next_exclusive_or_expression["rhs"] = parse_and_expression()))
			{
				jump(start_next);
				return exclusive_or_expression;
			}

			next_exclusive_or_expression["lhs"] = move(exclusive_or_expression);

			swap(next_exclusive_or_expression, exclusive_or_expression);
		}
	}

	AST parse_inclusive_or_expression()
	{
		DeclParse(inclusive_or_expression);

		if (!(inclusive_or_expression = parse_exclusive_or_expression()))
			return func.fail();

		while (true)
		{
			AST next_inclusive_or_expression;

			size_t start_next = pos;

			if (!(next_inclusive_or_expression["inclusive-or-operator"] = maybe(OP_BOR)))
				return inclusive_or_expression;

			if (!(next_inclusive_or_expression["rhs"] = parse_exclusive_or_expression()))
			{
				jump(start_next);
				return inclusive_or_expression;
			}

			next_inclusive_or_expression["lhs"] = move(inclusive_or_expression);

			swap(next_inclusive_or_expression, inclusive_or_expression);
		}
	}

	AST parse_logical_and_expression()
	{
		DeclParse(logical_and_expression);

		if (!(logical_and_expression = parse_inclusive_or_expression()))
			return func.fail();

		while (true)
		{
			AST next_logical_and_expression;

			size_t start_next = pos;

			if (!(next_logical_and_expression["logical-and-operator"] = maybe(OP_LAND)))
				return logical_and_expression;

			if (!(next_logical_and_expression["rhs"] = parse_inclusive_or_expression()))
			{
				jump(start_next);
				return logical_and_expression;
			}

			next_logical_and_expression["lhs"] = move(logical_and_expression);

			swap(next_logical_and_expression, logical_and_expression);
		}
	}

	AST parse_logical_or_expression()
	{
		DeclParse(logical_or_expression);

		if (!(logical_or_expression = parse_logical_and_expression()))
			return func.fail();

		while (true)
		{
			AST next_logical_or_expression;

			size_t start_next = pos;

			if (!(next_logical_or_expression["logical-or-operator"] = maybe(OP_LOR)))
				return logical_or_expression;

			if (!(next_logical_or_expression["rhs"] = parse_logical_and_expression()))
			{
				jump(start_next);
				return logical_or_expression;
			}

			next_logical_or_expression["lhs"] = move(logical_or_expression);

			swap(next_logical_or_expression, logical_or_expression);
		}
	}

	AST parse_conditional_expression(AST logical_or_expression = nullptr)
	{
		DeclParse(conditional_expression);

		if (!logical_or_expression)
		{
			if (!(logical_or_expression = parse_logical_or_expression()))
				return func.fail();
		}

		if (!drop(OP_QMARK))
			return logical_or_expression;

		conditional_expression["condition"] = move(logical_or_expression);

		if (!(conditional_expression["ontrue"] = parse_expression()))
			return func.fail();

		if (!drop(OP_COLON))
			return func.fail();

		if (!(conditional_expression["onfalse"] = parse_assignment_expression()))
			return func.fail();

		return conditional_expression;
	}

	AST parse_assignment_expression()
	{
		DeclParse(assignment_expression);

		if (peek(KW_THROW))
		{
			if (assignment_expression["throw-expression"] = parse_throw_expression())
				return assignment_expression;
			else
				return func.fail();
		}

		auto logical_or_expression = parse_logical_or_expression();

		if (!logical_or_expression)
			return func.fail();

		if (peek(OP_QMARK))
		{
			auto conditional_expression = parse_conditional_expression(logical_or_expression);

			if (!conditional_expression)
				return func.fail();
			else
				return conditional_expression;
		}

		size_t after_lhs = pos;

		auto assignment_operator = parse_assignment_operator();

		if (!assignment_operator)
			return logical_or_expression;

		assignment_expression["lhs"] = logical_or_expression;
		assignment_expression["assignment-operator"] = assignment_operator;

		if (!(assignment_expression["initializer-clause"] = parse_initializer_clause()))
		{
			jump(after_lhs);
			return logical_or_expression;
		}

		return assignment_expression;
	}

	AST parse_assignment_operator()
	{
		DeclParse(assignment_operator);

		switch (peek())
		{
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
	    	return pop();

		default:
			return func.fail();
		}
	}

	AST parse_expression()
	{
		DeclParse(expression);

		if (!(expression = parse_assignment_expression()))
			return func.fail();

		while (true)
		{
			AST next_expression;

			size_t start_next = pos;

			if (!(next_expression["comma-operator"] = maybe(OP_COMMA)))
				return expression;

			if (!(next_expression["rhs"] = parse_assignment_expression()))
			{
				jump(start_next);
				return expression;
			}

			next_expression["lhs"] = move(expression);

			swap(next_expression, expression);
		}
	}

	AST parse_constant_expression()
	{
		return parse_conditional_expression();
	}

	AST parse_statement()
	{
		DeclParse(statement);

		if (statement["labeled-statement"] = parse_labeled_statement())
			return statement;

		if (statement["try-block"] = parse_try_block())
			return statement;

		if (statement["declaration-statement"] = parse_declaration_statement())
			return statement;

		statement["attribute-specifier-seq"] = parse_attribute_specifier_seq();

		if (statement["compound-statement"] = parse_compound_statement())
			return statement;

		if (statement["selection-statement"] = parse_selection_statement())
			return statement;

		if (statement["iteration-statement"] = parse_iteration_statement())
			return statement;

		if (statement["jump-statement"] = parse_jump_statement())
			return statement;

		if (statement["expression-statement"] = parse_expression_statement())
			return statement;

		return func.fail();
	}

	AST parse_labeled_statement()
	{
		DeclParse(labeled_statement);

		labeled_statement["attribute-specifier-seq"] = parse_attribute_specifier_seq();

		switch (peek())
		{
		case TT_IDENTIFIER:
			labeled_statement["TT_IDENTIFIER"] = pop();
			break;

		case KW_CASE:
			labeled_statement["KW_CASE"] = pop();
			if (!(labeled_statement["constant-expression"] = parse_constant_expression()))
				return func.fail();

			break;


		case KW_DEFAULT:
			labeled_statement["KW_DEFAULT"] = pop();
			break;

		default:
			return func.fail();
		}

		if (!drop(OP_COLON))
			return func.fail();

		if (!(labeled_statement["statement"] = parse_statement()))
			return func.fail();

		return labeled_statement;
	}

	AST parse_expression_statement()
	{
		DeclParse(expression_statement);

		expression_statement["expression"] = parse_expression();

		if (!peek(OP_SEMICOLON))
			return func.fail();

		expression_statement["OP_SEMICOLON"] = pop();

		return expression_statement;
	}

	AST parse_compound_statement()
	{
		DeclParse(compound_statement);

		if (!drop(OP_LBRACE))
			return func.fail();

		func.nest();

		compound_statement = parse_star(&CppParser::parse_statement);

		if (!drop(OP_RBRACE))
			return func.fail();

		func.unnest();

		return compound_statement;
	}

	AST parse_selection_statement()
	{
		DeclParse(selection_statement);

		if (peek(KW_SWITCH))
		{
			selection_statement["KW_SWITCH"] = pop();

			if (!drop(OP_LPAREN))
				return func.fail();

			func.nest();

			if (!(selection_statement["condition"] = parse_condition()))
				return func.fail();

			if (!drop(OP_RPAREN))
				return func.fail();

			func.unnest();

			if (!(selection_statement["statement"] = parse_statement()))
				return func.fail();

			return selection_statement;
		}

		if (peek(KW_IF))
		{
			selection_statement["KW_IF"] = pop();

			if (!drop(OP_LPAREN))
				return func.fail();

			func.nest();

			if (!(selection_statement["condition"] = parse_condition()))
				return func.fail();

			if (!drop(OP_RPAREN))
				return func.fail();

			func.unnest();

			if (!(selection_statement["ontrue"] = parse_statement()))
				return func.fail();

			if (!drop(KW_ELSE))
				return selection_statement;

			if (!(selection_statement["onfalse"] = parse_statement()))
				return func.fail();

			return selection_statement;
		}

		return func.fail();
	}

	AST parse_condition_declaration()
	{
		DeclParse(condition_declaration);

		condition_declaration["attribute-specifier-seq"] = parse_attribute_specifier_seq();

		if ((condition_declaration["decl-specifier-seq"] = parse_decl_specifier_seq()) &&
			(condition_declaration["declarator"] = parse_declarator()))
		{
			if (peek(OP_ASS))
			{
				drop();

				if (condition_declaration["initializer-clause"] = parse_initializer_clause())
					return condition_declaration;
			}
			else
			{
				if (condition_declaration["braced-init-list"] = parse_braced_init_list())
					return condition_declaration;
			}
		}

		return func.fail();
	}

	AST parse_condition()
	{
		DeclParse(condition);

		if (condition = parse_condition_declaration())
			return condition;

		if (condition = parse_expression())
			return condition;

		return func.fail();
	}

	AST parse_iteration_statement()
	{
		DeclParse(iteration_statement);

		switch (peek())
		{
		case KW_WHILE:
			iteration_statement["while"] = pop();

			if (!drop(OP_LPAREN))
				return func.fail();

			func.nest();

			if (!(iteration_statement["condition"] = parse_condition()))
				return func.fail();

			if (!drop(OP_RPAREN))
				return func.fail();

			func.unnest();

			if (!(iteration_statement["statement"] = parse_statement()))
				return func.fail();

			return iteration_statement;

		case KW_DO:
			iteration_statement["do"] = pop();

			if (!(iteration_statement["statement"] = parse_statement()))
				return func.fail();

			if (!drop(KW_WHILE))
				return func.fail();

			if (!drop(OP_LPAREN))
				return func.fail();

			func.nest();

			if (!(iteration_statement["condition"] = parse_expression()))
				return func.fail();

			if (!drop(OP_RPAREN))
				return func.fail();

			func.unnest();

			if (!drop(OP_SEMICOLON))
				return func.fail();

			return iteration_statement;

		case KW_FOR:
		{
			iteration_statement["for"] = pop();

			if (!drop(OP_LPAREN))
				return func.fail();

			func.nest();

			size_t after_paren = pos;

			auto for_range_declaration = parse_for_range_declaration();

			if (for_range_declaration && drop(OP_COLON))
			{
				iteration_statement["for-range-declaration"] = for_range_declaration;

				if (!(iteration_statement["for-range-initializer"] = parse_for_range_initializer()))
					return func.fail();

				if (!drop(OP_RPAREN))
					return func.fail();

				func.unnest();

				if (!(iteration_statement["statement"] = parse_statement()))
					return func.fail();

				return iteration_statement;
			}

			jump(after_paren);

			// for-init

			if (!(iteration_statement["for-init-statement"] = parse_for_init_statement()))
				return func.fail();

			iteration_statement["condition"] = parse_condition();

			if (!drop(OP_SEMICOLON))
				return func.fail();

			iteration_statement["expression"] = parse_expression();

			if (!drop(OP_RPAREN))
				return func.fail();

			func.unnest();

			if (!(iteration_statement["statement"] = parse_statement()))
				return func.fail();

			return iteration_statement;
		}

		default:
			return func.fail();

		}
	}

	AST parse_for_init_statement()
	{
		DeclParse(for_init_statement);

		auto simple_declaration = parse_simple_declaration();

		if (simple_declaration)
			return simple_declaration;
		else
			return parse_expression_statement();
	}

	AST parse_for_range_declaration()
	{
		DeclParse(for_range_declaration);

		for_range_declaration["attribute-specifier-seq"] = parse_attribute_specifier_seq();

		if (!(for_range_declaration["decl-specififer-seq"] = parse_decl_specifier_seq()))
			return func.fail();

		if (!(for_range_declaration["declarator"] = parse_declarator()))
			return func.fail();

		return for_range_declaration;
	}

	AST parse_for_range_initializer()
	{
		DeclParse(for_range_initializer);

		if (peek(OP_LBRACE))
			return parse_braced_init_list();
		else
			return parse_expression();
	}

	AST parse_jump_statement()
	{
		DeclParse(jump_statement);

		switch (peek())
		{
		case KW_BREAK:
		case KW_CONTINUE:
			jump_statement["jump"] = pop();
			if (!drop(OP_SEMICOLON))
				return func.fail();
			return jump_statement;

		case KW_RETURN:
			jump_statement["jump"] = pop();
			if (peek(OP_LBRACE))
				jump_statement["braced-init-list"] = parse_braced_init_list();
			else
				jump_statement["expression"] = parse_expression();

			if (!drop(OP_SEMICOLON))
				return func.fail();
			return jump_statement;

		case KW_GOTO:
			jump_statement["jump"] = pop();
			if (!peek(TT_IDENTIFIER))
				return func.fail();
			jump_statement["identifier"] = pop();

			if (!drop(OP_SEMICOLON))
				return func.fail();

			return jump_statement;

		default:
			return func.fail();
		}
	}

	AST parse_declaration_statement()
	{
		return parse_block_declaration();
	}

	AST parse_declaration()
	{
		DeclParse(declaration);

		if (declaration["empty-declaration"] = parse_empty_declaration())
			return declaration;

		if (declaration["function-definition"] = parse_function_definition())
			return declaration;

		if (declaration["template-declaration"] = parse_template_declaration())
			return declaration;

		if (declaration["explicit-instantiation"] = parse_explicit_instantiation())
			return declaration;

		if (declaration["explicit-specialization"] = parse_explicit_specialization())
			return declaration;

		if (declaration["linkage-specification"] = parse_linkage_specification())
			return declaration;

		if (declaration["namespace-definition"] = parse_namespace_definition())
			return declaration;

		if (declaration["attribute-declaration"] = parse_attribute_declaration())
			return declaration;

		if (declaration = parse_block_declaration())
			return declaration;

		return func.fail();
	}

	AST parse_block_declaration()
	{
		DeclParse(block_declaration);

		if (block_declaration["simple-declaration"] = parse_simple_declaration())
			return block_declaration;

		if (block_declaration["asm-definition"] = parse_asm_definition())
			return block_declaration;

		if (block_declaration["namespace-alias-definition"] = parse_namespace_alias_definition())
			return block_declaration;

		if (block_declaration["using-declaration"] = parse_using_declaration())
			return block_declaration;

		if (block_declaration["using-directive"] = parse_using_directive())
			return block_declaration;

		if (block_declaration["static_assert-declaration"] = parse_static_assert_declaration())
			return block_declaration;

		if (block_declaration["alias-declaration"] = parse_alias_declaration())
			return block_declaration;

		if (block_declaration["opaque-enum-declaration"] = parse_opaque_enum_declaration())
			return block_declaration;

		return func.fail();
	}

	AST parse_alias_declaration()
	{
		DeclParse(alias_declaration);

		if (!drop(KW_USING))
			return func.fail();

		if (!peek(TT_IDENTIFIER))
			return func.fail();

		alias_declaration["identifier"] = pop();

		alias_declaration["attribute-specifier-seq"] = parse_attribute_specifier_seq();

		if (!drop(OP_ASS))
			return func.fail();

		if (!(alias_declaration["type-id"] = parse_type_id()))
			return func.fail();

		if (!drop(OP_SEMICOLON))
			return func.fail();

		return alias_declaration;
	}

	AST parse_simple_declaration()
	{
		DeclParse(simple_declaration);

		simple_declaration["attribute-specifier-seq"] = parse_attribute_specifier_seq();

		if (!(simple_declaration["decl-specifier-seq"] = parse_decl_specifier_seq()))
			return func.fail();

		simple_declaration["init-declarator-list"] = parse_init_declarator_list();

		if (!drop(OP_SEMICOLON))
			return func.fail();

		if (!simple_declaration["attribute-specifier-seq"].empty())
			if (!simple_declaration["init-declarator-list"])
				return func.fail();

		return simple_declaration;
	}

	AST parse_static_assert_declaration()
	{
		DeclParse(static_assert_declaration);

		if (!drop(KW_STATIC_ASSERT))
			return func.fail();

		if (!drop(OP_LPAREN))
			return func.fail();

		func.nest();

		if (!(static_assert_declaration["constant-expression"] = parse_constant_expression()))
			return func.fail();

		if (!drop(OP_COMMA))
			return func.fail();

		if (!peek(TT_LITERAL))
			return func.fail();

		static_assert_declaration["literal"] = pop();

		if (!drop(OP_RPAREN))
			return func.fail();

		func.unnest();

		if (!drop(OP_SEMICOLON))
			return func.fail();

		return static_assert_declaration;
	}

	AST parse_empty_declaration()
	{
		DeclParse(empty_declaration);

		if (drop(OP_SEMICOLON))
			return astempty;
		else
			return func.fail();
	}

	AST parse_attribute_declaration()
	{
		DeclParse(attribute_declaration);

		attribute_declaration = parse_attribute_specifier_seq();

		if (!drop(OP_SEMICOLON))
			return func.fail();

		return attribute_declaration;
	}

	AST parse_decl_specifier()
	{
		DeclParse(decl_specifier);

		switch (peek())
		{
		case KW_FRIEND:
		case KW_TYPEDEF:
		case KW_CONSTEXPR:
		    return pop();
		}

		if (decl_specifier["storage-class-specifier"] = parse_storage_class_specifier())
			return decl_specifier;
		else if (decl_specifier["type-specifier"] = parse_type_specifier())
			return decl_specifier;
		else if (decl_specifier["function-specifier"] = parse_function_specifier())
			return decl_specifier;
		else
			return func.fail();
	}

	AST parse_decl_specifier_seq()
	{
		DeclParse(decl_specifier_seq);

		bool found = false;
		size_t i = 0;
		for (; true; i++)
		{
			auto save = pos;
			auto decl_specifier = parse_decl_specifier();

			if (!decl_specifier)
				break;

			if (found && decl_specifier.has("type-specifier") &&
				decl_specifier["type-specifier"].has("type-name"))
			{
				jump(save);
				break;
			}

			if (decl_specifier.has("type-specifier") &&
				!decl_specifier["type-specifier"].has("cv-qualifier"))
				found = true;

			decl_specifier_seq[i] = decl_specifier;
		}

		decl_specifier_seq["attribute-specifier-seq"] = parse_attribute_specifier_seq();

		if (i == 0)
			return func.fail();

		return decl_specifier_seq;
	}

	AST parse_storage_class_specifier()
	{
		DeclParse(storage_class_specifier);

		switch (peek())
		{
		case KW_REGISTER:
		case KW_STATIC:
		case KW_THREAD_LOCAL:
		case KW_EXTERN:
		case KW_MUTABLE:
			return pop();

		default:
			return func.fail();
		}
	}

	AST parse_function_specifier()
	{
		DeclParse(function_specifier);

		switch (peek())
		{
		case KW_INLINE:
		case KW_VIRTUAL:
		case KW_EXPLICIT:
			return pop();

		default:
			return func.fail();
		}
	}

	AST parse_type_specifier()
	{
		DeclParse(type_specifier);

		type_specifier.setp("type-specifier");

		if (type_specifier["class-specifier"] = parse_class_specifier())
			return type_specifier;

		if (type_specifier["enum-specifier"] = parse_enum_specifier())
			return type_specifier;

		if (type_specifier = parse_trailing_type_specifier())
			return type_specifier;

		return func.fail();
	}

	AST parse_trailing_type_specifier()
	{
		DeclParse(trailing_type_specifier);

		if (trailing_type_specifier = parse_simple_type_specifier())
			return trailing_type_specifier;

		if (trailing_type_specifier["elaborated-type-specifier"] = parse_elaborated_type_specifier())
			return trailing_type_specifier;

		if (trailing_type_specifier["typename-specifier"] = parse_typename_specifier())
			return trailing_type_specifier;

		if (trailing_type_specifier["cv-qualifier"] = parse_cv_qualifier())
			return trailing_type_specifier;

		return func.fail();
	}

	AST parse_type_specifier_seq()
	{
		DeclParse(type_specifier_seq);

		if (!(type_specifier_seq = parse_plus(&CppParser::parse_type_specifier)))
			return func.fail();

		type_specifier_seq["attribute-specifier-seq"] = parse_attribute_specifier_seq();

		return type_specifier_seq;
	}

	AST parse_simple_type_specifier()
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
		case KW_AUTO:
			return pop();

		case KW_DECLTYPE:
			if (simple_type_specifier = parse_decltype_specifier())
				return simple_type_specifier;
			else
				return func.fail();
		}

		if (simple_type_specifier["nested_name_specifier"] = parse_nested_name_specifier())
		{
			if (peek(KW_TEMPLATE))
			{
				simple_type_specifier["KW_TEMPLATE"] = pop();
				if (simple_type_specifier["simple-template-id"] = parse_simple_template_id())
					return simple_type_specifier;
				else
					return func.fail();
			}
		}

		if (simple_type_specifier["type-name"] = parse_type_name())
			return simple_type_specifier.setp("type-name");
		else
			return func.fail();
	}

	AST parse_decltype_specifier()
	{
		DeclParse(decltype_specifier);

		if (!peek(KW_DECLTYPE))
			return func.fail();

		decltype_specifier["KW_DECLTYPE"] = pop();

		if (!drop(OP_LPAREN))
			return func.fail();

		func.nest();

		if (!(decltype_specifier["expression"] = parse_expression()))
			return func.fail();

		if (!drop(OP_RPAREN))
			return func.fail();

		func.unnest();

		return decltype_specifier;
	}

	AST parse_elaborated_type_specifier()
	{
		DeclParse(elaborated_type_specifier);

		switch (peek())
		{
		case KW_CLASS:
		case KW_STRUCT:
		case KW_UNION:
			elaborated_type_specifier["class-key"] = parse_class_key();
			elaborated_type_specifier["attribute-specifier-seq"] = parse_attribute_specifier_seq();
			elaborated_type_specifier["nested-name-specifier"] = parse_nested_name_specifier();
			elaborated_type_specifier["KW_TEMPLATE"] = maybe(KW_TEMPLATE);
			if (elaborated_type_specifier["simple-template-id"] = parse_simple_template_id())
				return elaborated_type_specifier;

			if (elaborated_type_specifier["TT_IDENTIFIER"] = maybe(TT_IDENTIFIER))
				return elaborated_type_specifier;

			return func.fail();

		case KW_ENUM:
			elaborated_type_specifier["KW_ENUM"] = pop();
			elaborated_type_specifier["nested-name-specifier"] = parse_nested_name_specifier();
			if (elaborated_type_specifier["TT_IDENTIFIER"] = maybe(TT_IDENTIFIER))
				return elaborated_type_specifier;

			return func.fail();
		default:
			return func.fail();
		}
	}

	AST parse_enum_specifier()
	{
		DeclParse(enum_specifier);

		if (!(enum_specifier["enum-head"] = parse_enum_head()))
			return func.fail();

		if (!drop(OP_LBRACE))
			return func.fail();

		func.nest();

		if (enum_specifier["enumerator-list"] = parse_enumerator_list())
			drop(OP_COMMA);

		if (!drop(OP_RBRACE))
			return func.fail();

		func.unnest();

		return enum_specifier;
	}

	AST parse_enum_head()
	{
		DeclParse(enum_head);

		if (!(enum_head["enum-key"] = parse_enum_key()))
			return func.fail();

		enum_head["attribute-specifier-seq"] = parse_attribute_specifier_seq();

		bool nns = bool(enum_head["nested-name-specifier"] = parse_nested_name_specifier());

		bool id = bool(enum_head["TT_IDENTIFIER"] = maybe(TT_IDENTIFIER));

		if (nns && !id)
			return func.fail();

		enum_head["enum-base"] = parse_enum_base();

		return enum_head;
	}

	AST parse_opaque_enum_declaration()
	{
		DeclParse(opaque_enum_declaration);

		if (!(opaque_enum_declaration["enum-key"] = parse_enum_key()))
			return func.fail();

		opaque_enum_declaration["attribute-specifier-seq"] = parse_attribute_specifier_seq();

		if (!(opaque_enum_declaration["TT_IDENTIFIER"] = maybe(TT_IDENTIFIER)))
			return func.fail();

		opaque_enum_declaration["enum-base"] = parse_enum_base();

		if (!drop(OP_SEMICOLON))
			return func.fail();

		return opaque_enum_declaration;
	}

	AST parse_enum_key()
	{
		DeclParse(enum_key);

		if (!peek(KW_ENUM))
			return func.fail();

		enum_key["KW_ENUM"] = pop();

		if (peek(KW_CLASS))
		{
			enum_key["KW_CLASS"] = pop();
			return enum_key;
		}

		if (peek(KW_STRUCT))
		{
			enum_key["KW_STRUCT"] = pop();
			return enum_key;
		}

		return enum_key;
	}

	AST parse_enum_base()
	{
		DeclParse(enum_base);

		if (!drop(OP_COLON))
			return func.fail();

		if (enum_base["type-specifier-seq"] = parse_type_specifier_seq())
			return enum_base;
		else
			return func.fail();
	}

	AST parse_enumerator_list()
	{
		DeclParse(enumerator_list);

		size_t i = 0;

		if (!(enumerator_list[i++] = parse_enumerator_definition()))
			return func.fail();

		while (true)
		{
			size_t last = pos;
			if (!drop(OP_COMMA) || !(enumerator_list[i++] = parse_enumerator_definition()))
			{
				jump(last);
				return enumerator_list;
			}
		}
	}

	AST parse_enumerator_definition()
	{
		DeclParse(enumerator_definition);

		if (!(enumerator_definition["enumerator"] = parse_enumerator()))
			return func.fail();

		if (peek(OP_ASS))
		{
			size_t start = pos;
			drop();

			if (!(enumerator_definition["constant-expression"] = parse_constant_expression()))
			{
				jump(start);
				return enumerator_definition;
			}
		}

		return enumerator_definition;
	}

	AST parse_enumerator()
	{
		DeclParse(enumerator);

		return maybe(TT_IDENTIFIER);
	}

	AST parse_namespace_definition()
	{
		DeclParse(namespace_definition);

		namespace_definition["KW_INLINE"] = maybe(KW_INLINE);

		if (!drop(KW_NAMESPACE))
			return func.fail();

		namespace_definition["TT_IDENTIFIER"] = maybe(TT_IDENTIFIER);

		if (!drop(OP_LBRACE))
			return func.fail();

		func.nest();

		if (!(namespace_definition["namespace-body"] = parse_namespace_body()))
			return func.fail();

		if (!drop(OP_RBRACE))
			return func.fail();

		func.unnest();

		return namespace_definition;
	}

	AST parse_namespace_body()
	{
		DeclParse(namespace_body);

		return parse_star(&CppParser::parse_declaration);
	}

	AST parse_namespace_alias_definition()
	{
		DeclParse(namespace_alias_definition);

		if (!drop(KW_NAMESPACE))
			return func.fail();

		if (!(namespace_alias_definition["TT_IDENTIFIER"] = maybe(TT_IDENTIFIER)))
			return func.fail();

		if (!drop(OP_ASS))
			return func.fail();

		if (!(namespace_alias_definition["qualified-namespace-specifier"] = parse_qualified_namespace_specifier()))
			return func.fail();

		if (!drop(OP_SEMICOLON))
			return func.fail();

		return namespace_alias_definition;
	}

	AST parse_qualified_namespace_specifier()
	{
		DeclParse(qualified_namespace_specifier);

		qualified_namespace_specifier["nested-name-specifier"] = parse_nested_name_specifier();

		if (qualified_namespace_specifier["namespace-name"] = parse_namespace_name())
			return qualified_namespace_specifier;
		else
			return func.fail();
	}

	AST parse_using_declaration()
	{
		DeclParse(using_declaration);

		if (!drop(KW_USING))
			return func.fail();

		size_t start = pos;
		if (peek(OP_COLON2))
		{
			auto op_colon2 = pop();
			auto unqualified_id = parse_unqualified_id();
			if (drop(OP_SEMICOLON))
			{
				using_declaration["OP_COLON2"] = op_colon2;
				using_declaration["unqualified-id"] = unqualified_id;
				return using_declaration;
			}
		}
		jump(start);

		using_declaration["KW_TYPENAME"] = maybe(KW_TYPENAME);

		if (!(using_declaration["nested-name-specifier"] = parse_nested_name_specifier()))
			return func.fail();

		if (!(using_declaration["unqualified-id"] = parse_unqualified_id()))
			return func.fail();

		if (!drop(OP_SEMICOLON))
			return func.fail();

		return using_declaration;
	}

	AST parse_using_directive()
	{
		DeclParse(using_directive);

		using_directive["attribute-specifier-seq"] = parse_attribute_specifier_seq();

		if (!drop(KW_USING))
			return func.fail();

		if (!drop(KW_NAMESPACE))
			return func.fail();

		using_directive["nested-name-specifier"] = parse_nested_name_specifier();

		if (!(using_directive["TT_IDENTIFIER"] = maybe(TT_IDENTIFIER)))
			return func.fail();

		if (!drop(OP_SEMICOLON))
			return func.fail();

		return using_directive;
	}

	AST parse_asm_definition()
	{
		DeclParse(asm_definition);

		if (!drop(KW_ASM))
			return func.fail();

		if (!drop(OP_LPAREN))
			return func.fail();

		func.nest();

		if (!(asm_definition["TT_LITERAL"] = maybe(TT_LITERAL)))
			return func.fail();

		if (!drop(OP_RPAREN))
			return func.fail();

		if (!drop(OP_SEMICOLON))
			return func.fail();

		func.unnest();

		return asm_definition;
	}

	AST parse_linkage_specification()
	{
		DeclParse(linkage_specification);

		if (!drop(KW_EXTERN))
			return func.fail();

		if (!(linkage_specification["TT_LITERAL"] = maybe(TT_LITERAL)))
			return func.fail();

		if (drop(OP_LBRACE))
		{
			func.nest();

			linkage_specification["declaration-seq"] = parse_star(&CppParser::parse_declaration);

			if (!drop(OP_RBRACE))
				return func.fail();

			func.unnest();

			return linkage_specification;
		}

		if (linkage_specification["declaration"] = parse_declaration())
			return linkage_specification;
		else
			return func.fail();
	}

	AST parse_attribute_specifier_seq()
	{
		DeclParse(attribute_specifier_seq);

		attribute_specifier_seq = parse_star(&CppParser::parse_attribute_specifier);

		if (attribute_specifier_seq.empty())
			return func.fail();
		else
			return attribute_specifier_seq;
	}

	AST parse_attribute_specifier()
	{
		DeclParse(attr);

		if (peek(0, OP_LSQUARE) && peek(1, OP_LSQUARE))
		{
			drop(2);

			func.nest();

			attr["attribute-list"] = parse_attribute_list();

			if (peek(0, OP_RSQUARE) && peek(1, OP_RSQUARE))
			{
				drop(2);

				func.unnest();
				return attr;
			}

			return func.fail();
		}

		attr["alignment-specifier"] = parse_alignment_specifier();

		return attr;
	}

	AST parse_alignment_specifier()
	{
		DeclParse(alignment_specifier);

		if (!peek(0, KW_ALIGNAS) || !peek(1, OP_LPAREN))
			return func.fail();

		drop(2);

		func.nest();

		size_t arg_start = pos;

		auto type_id = parse_type_id();

		auto op_dots = maybe(OP_DOTS);

		if (type_id && peek(OP_RPAREN))
		{
			drop();

			func.unnest();
			alignment_specifier["type-id"] = type_id;
			alignment_specifier["OP_DOTS"] = op_dots;
			return alignment_specifier;
		}

		jump(arg_start);

		auto assignment_expression = parse_assignment_expression();

		op_dots = maybe(OP_DOTS);

		if (assignment_expression && peek(OP_RPAREN))
		{
			drop();

			func.unnest();

			alignment_specifier["assignment-expression"] = assignment_expression;
			alignment_specifier["OP_DOTS"] = op_dots;
			return alignment_specifier;
		}

		return func.fail();
	}

	AST parse_attribute_list()
	{
		DeclParse(attribute_list);

		attribute_list[0] = parse_attribute_part();

		size_t i = 1;
		while (peek(OP_COMMA))
		{
			drop();
			attribute_list[i++] = parse_attribute_part();
		}
		return attribute_list;
	}

	AST parse_attribute_part()
	{
		DeclParse(attribute_part);

		auto attribute = parse_attribute();

		if (!attribute)
			return astempty;

		attribute_part["attribute"] = attribute;

		if (peek(OP_DOTS))
			attribute_part["OP_DOTS"] = pop();

		return attribute_part;
	}

	AST parse_attribute()
	{
		DeclParse(attribute);

		auto attribute_token = parse_attribute_token();

		if (!attribute_token)
			return func.fail();

		attribute["attribute-token"] = attribute_token;
		attribute["attribute-argument-clause"] = parse_attribute_argument_clause();

		return attribute;
	}

	AST parse_attribute_token()
	{
		DeclParse(attribute_token);

		if (!peek(TT_IDENTIFIER))
			return func.fail();

		auto attribute_scoped_token = parse_attribute_scoped_token();

		if (attribute_scoped_token)
		{
			attribute_token["attribute-scoped-token"] = attribute_scoped_token;
			return attribute_token;
		}

		attribute_token["TT_IDENTIFER"] = pop();

		return attribute_token;
	}

	AST parse_attribute_scoped_token()
	{
		DeclParse(attribute_scoped_token);

		if (!peek(0, TT_IDENTIFIER) || !peek(1, OP_COLON2) || !peek(2, TT_IDENTIFIER))
			return func.fail();

		attribute_scoped_token["attribute-namespace"] = pop();
		pop();
		attribute_scoped_token["TT_IDENTIFER"] = pop();

		return attribute_scoped_token;
	}

	AST parse_attribute_argument_clause()
	{
		DeclParse(attribute_argument_clause);

		if (!drop(OP_LPAREN))
			return func.fail();

		func.nest();

		for (size_t i = 0; true; i++)
		{
			auto balanced_token = parse_balanced_token();

			if (!balanced_token)
				break;

			attribute_argument_clause[i] = balanced_token;
		}

		if (!drop(OP_RPAREN))
			return func.fail();

		func.unnest();

		return attribute_argument_clause;
	}

	AST parse_balanced_token()
	{
		DeclParse(balanced_token);

		switch (peek())
		{
		case OP_LPAREN:
		case OP_LSQUARE:
		case OP_LBRACE:
		{
			auto lbracket = pop();

			func.nest();

			balanced_token["lbracket"] = lbracket;

			for (size_t i = 0; true; i++)
			{
				auto inner_balanced_token = parse_balanced_token();

				if (!inner_balanced_token)
					break;

				balanced_token[i] = inner_balanced_token;
			}

			auto rbracket = pop();

			if (
				(lbracket->token_type == OP_LPAREN  && rbracket->token_type != OP_RPAREN) ||
				(lbracket->token_type == OP_LSQUARE && rbracket->token_type != OP_RSQUARE) ||
				(lbracket->token_type == OP_LBRACE  && rbracket->token_type != OP_RBRACE)
			)
				return func.fail();

			balanced_token["rbracket"] = rbracket;

			return balanced_token;
		}

		case OP_RPAREN:
		case OP_RSQUARE:
		case OP_RBRACE:
		case TT_INVALID:
		case TT_EOF:
			return func.fail();

		default:
			return pop();
		}
	}

	AST parse_init_declarator_list()
	{
		DeclParse(init_declarator_list);

		if (!(init_declarator_list[0] = parse_init_declarator()))
			return func.fail();

		size_t i = 1;
		while (true)
		{
			size_t save = pos;
			if (!drop(OP_COMMA))
				return init_declarator_list;

			if (!(init_declarator_list[i++] = parse_init_declarator()))
			{
				jump(save);
				return init_declarator_list;
			}
		}
	}

	AST parse_init_declarator()
	{
		DeclParse(init_declarator);

		if (!(init_declarator["declarator"] = parse_declarator()))
			return func.fail();

		init_declarator["initializer"] = parse_initializer();

		return init_declarator;
	}

	AST parse_declarator()
	{
		DeclParse(declarator);

		declarator["ptr-operator-seq"] = parse_star(&CppParser::parse_ptr_operator);

		if (!(declarator["noptr-declarator"] = parse_noptr_declarator()))
			return func.fail();

		declarator["trailing-return-type"] = parse_trailing_return_type();

		if (!declarator["parse-operator-seq"].empty() && declarator["trailing-return-type"])
			return func.fail();

		return declarator;
	}

	AST parse_ptr_declarator()
	{
		DeclParse(ptr_declarator);

		ptr_declarator["ptr-operator-seq"] = parse_star(&CppParser::parse_ptr_operator);

		if (ptr_declarator["noptr-declarator"] = parse_noptr_declarator())
			return ptr_declarator;
		else
			return func.fail();
	}

	AST parse_noptr_declarator()
	{
		DeclParse(noptr_declarator);

		if (!(noptr_declarator["noptr-declarator-root"] = parse_noptr_declarator_root()))
			return func.fail();

		noptr_declarator["noptr-declartor-suffix-seq"] = parse_star(&CppParser::parse_noptr_declarator_suffix);

		return noptr_declarator;
	}

	AST parse_noptr_declarator_root()
	{
		DeclParse(noptr_declarator_root);

		if (peek(OP_LPAREN))
		{
			noptr_declarator_root["OP_LPAREN"] = pop();

			func.nest();

			if (!(noptr_declarator_root["ptr-declarator"] = parse_ptr_declarator()))
				return func.fail();

			if (!(noptr_declarator_root["OP_RPAREN"] = maybe(OP_RPAREN)))
				return func.fail();

			func.unnest();

			return noptr_declarator_root;
		}

		if (!(noptr_declarator_root["declarator-id"] = parse_declarator_id()))
			return func.fail();

		noptr_declarator_root["attribute-specifier-seq"] = parse_attribute_specifier_seq();

		return noptr_declarator_root;
	}

	AST parse_noptr_declarator_suffix()
	{
		DeclParse(noptr_declarator_suffix);

		switch (peek())
		{
		case OP_LSQUARE:
			noptr_declarator_suffix["OP_LSQUARE"] = pop();
			func.nest();
			noptr_declarator_suffix["constant-expression"] = parse_constant_expression();
			if (noptr_declarator_suffix["OP_RSQUARE"] = maybe(OP_RSQUARE))
				func.unnest();
			noptr_declarator_suffix["attribute-specifier-seq"] = parse_attribute_specifier_seq();

			if (!noptr_declarator_suffix["OP_RSQUARE"])
				return func.fail();


			return noptr_declarator_suffix;

		case OP_LPAREN:
			if (noptr_declarator_suffix["parameters-and-qualifiers"] = parse_parameters_and_qualifiers())
				return noptr_declarator_suffix;
			else
				return func.fail();

		default:
			return func.fail();
		}
	}

	AST parse_parameters_and_qualifiers()
	{
		DeclParse(parameters_and_qualifiers);

		if (!drop(OP_LPAREN))
			return func.fail();

		func.nest();

		if (!(parameters_and_qualifiers["parameter-declaration-clause"] = parse_parameter_declaration_clause()))
			return func.fail();

		if (!drop(OP_RPAREN))
			return func.fail();

		func.unnest();

		parameters_and_qualifiers["cv-qualifer-seq"] = parse_star(&CppParser::parse_cv_qualifier);
		parameters_and_qualifiers["ref-qualifier"] = parse_ref_qualifier();
		parameters_and_qualifiers["exception-specification"] = parse_exception_specification();
		parameters_and_qualifiers["attribute-specifier-seq"] = parse_attribute_specifier_seq();

		return parameters_and_qualifiers;
	}

	AST parse_trailing_return_type()
	{
		DeclParse(trailing_return_type);

		if (!drop(OP_ARROW))
			return func.fail();

		auto trailing_type_specifier_seq = parse_plus(&CppParser::parse_trailing_type_specifier);

		if (!trailing_type_specifier_seq)
			return func.fail();

		trailing_return_type["trailing-type-specifier-seq"] = trailing_type_specifier_seq;

		trailing_return_type["attribute-specifier-seq"] = parse_attribute_specifier_seq();

		trailing_return_type["abstract-declarator"] = parse_abstract_declarator();

		return trailing_return_type;
	}


	AST parse_ptr_operator()
	{
		DeclParse(ptr_operator);

		auto p = peek();

		switch (p)
		{
		case OP_STAR:
		case OP_AMP:
		case OP_LAND:
			ptr_operator["op"] = pop();
			ptr_operator["attribute-specifier-seq"] = parse_attribute_specifier_seq();

			if (p == OP_STAR)
				ptr_operator["cv-qualifier-seq"] = parse_star(&CppParser::parse_cv_qualifier);

			return ptr_operator;
		}


		if (!(ptr_operator["nested-name-specifier"] = parse_nested_name_specifier()))
			return func.fail();

		if (!(ptr_operator["op"] = maybe(OP_STAR)))
			return func.fail();

		ptr_operator["attribute-specifier-seq"] = parse_attribute_specifier_seq();
		ptr_operator["cv-qualifier-seq"] = parse_star(&CppParser::parse_cv_qualifier);

		return ptr_operator;
	}

	AST parse_cv_qualifier()
	{
		DeclParse(cv_qualifier);

		switch (peek())
		{
		case KW_CONST:
		case KW_VOLATILE:
			return pop();

		default:
			return func.fail();
		}
	}

	AST parse_ref_qualifier()
	{
		DeclParse(ref_qualifier);

		switch (peek())
		{
		case OP_AMP:
		case OP_LAND:
			return pop();

		default:
			return func.fail();
		}
	}

	AST parse_declarator_id()
	{
		DeclParse(declarator_id);

		declarator_id["OP_DOTS"] = maybe(OP_DOTS);

		if (declarator_id["id-expression"] = parse_id_expression())
			return declarator_id;
		else
			return func.fail();
	}

	AST parse_type_id()
	{
		DeclParse(type_id);

		if (!(type_id["type-specifier-seq"] = parse_type_specifier_seq()))
			return func.fail();

		type_id["abstract-declarator"] = parse_abstract_declarator();

		return type_id;
	}

	AST parse_abstract_declarator()
	{
		DeclParse(abstract_declarator);

		if (abstract_declarator["abstract-pack-declarataor"] = parse_abstract_pack_declarator())
			return abstract_declarator;

		size_t save = pos;

		auto noptr_abstract_declarator = parse_noptr_abstract_declarator();

		auto trailing_return_type = parse_trailing_return_type();

		if (trailing_return_type)
		{
			abstract_declarator["noptr-abstract-declarator"] = noptr_abstract_declarator;
			abstract_declarator["trailing-return-type"] = trailing_return_type;

			return abstract_declarator;
		}

		jump(save);

		if (abstract_declarator["ptr-abstract-declarator"] = parse_ptr_abstract_declarator())
			return abstract_declarator;

		return func.fail();
	}

	AST parse_ptr_abstract_declarator()
	{
		DeclParse(ptr_abstract_declarator);

		ptr_abstract_declarator["ptr-operator-seq"] = parse_plus(&CppParser::parse_ptr_operator);

		ptr_abstract_declarator["noptr-abstract-declarator"] = parse_noptr_abstract_declarator();

		if (ptr_abstract_declarator["ptr-operator-seq"] || ptr_abstract_declarator["noptr-abstract-declarator"])
			return ptr_abstract_declarator;
		else
			return func.fail();
	}

	AST parse_noptr_abstract_declarator()
	{
		DeclParse(noptr_abstract_declarator);

		noptr_abstract_declarator["noptr-abstract-declarator-root"] = parse_noptr_abstract_declarator_root();

		noptr_abstract_declarator["noptr-declarator-suffix-seq"] = parse_plus(&CppParser::parse_noptr_declarator_suffix);

		if (noptr_abstract_declarator["noptr-abstract-declarator-root"]
		   || noptr_abstract_declarator["noptr-declarator-suffix-seq"])
			return noptr_abstract_declarator;
		else
			return func.fail();
	}

	AST parse_noptr_abstract_declarator_root()
	{
		DeclParse(noptr_abstract_declarator_root);

		size_t save = pos;
		if (peek(OP_LPAREN))
		{
			auto lparen = pop();

			func.nest();

			auto ptr_abstract_declarator = parse_ptr_abstract_declarator();

			if (ptr_abstract_declarator && peek(OP_RPAREN))
			{
				noptr_abstract_declarator_root["OP_LPAREN"] = lparen;
				noptr_abstract_declarator_root["ptr-abstract-declarator"] = ptr_abstract_declarator;
				noptr_abstract_declarator_root["OP_RPAREN"] = pop();

				func.unnest();
				return noptr_abstract_declarator_root;
			}

			jump(save);
			func.unnest();
		}

		if (noptr_abstract_declarator_root["noptr-declarator-suffix"] = parse_noptr_declarator_suffix())
			return noptr_abstract_declarator_root;
		else
			return func.fail();
	}

	AST parse_abstract_pack_declarator()
	{
		DeclParse(abstract_pack_declarator);

		abstract_pack_declarator["ptr-operator-seq"] = parse_plus(&CppParser::parse_ptr_operator);

		if (abstract_pack_declarator["noptr-abstract-pack-declarator"] = parse_noptr_abstract_pack_declarator())
			return abstract_pack_declarator;
		else
			return func.fail();
	}

	AST parse_noptr_abstract_pack_declarator()
	{
		DeclParse(noptr_abstract_pack_declarator);

		if (noptr_abstract_pack_declarator["OP_DOTS"] = maybe(OP_DOTS))
		{
			noptr_abstract_pack_declarator["noptr-declarator-suffix-seq"] =
				parse_plus(&CppParser::parse_noptr_declarator_suffix);

			return noptr_abstract_pack_declarator;
		}
		else
			return func.fail();
	}

	AST parse_parameter_declaration_clause()
	{
		DeclParse(parameter_declaration_clause);

		auto parameter_declaration_list = parse_parameter_declaration_list();

		if (!parameter_declaration_list)
		{
			if (peek(OP_DOTS))
			{
				parameter_declaration_clause["OP_DOTS"] = pop();
				return parameter_declaration_clause;
			}
			else
				return astempty;
		}
		else
		{
			parameter_declaration_clause["parameter-declaration-list"] = parameter_declaration_list;

			if (peek(0, OP_DOTS))
			{
				parameter_declaration_clause["OP_COMMA"] = pop();
			}
			else if (peek(0, OP_COMMA) && peek(1, OP_DOTS))
			{
				parameter_declaration_clause["OP_COMMA"] = pop();
				parameter_declaration_clause["OP_DOTS"] = pop();
			}

			return parameter_declaration_clause;
		}
	}

	AST parse_parameter_declaration_list()
	{
		DeclParse(parameter_declaration_list);

		if (!(parameter_declaration_list[0] = parse_parameter_declaration()))
			return func.fail();

		for (size_t i = 1; true; i++)
		{
			size_t save = pos;

			if (!drop(OP_COMMA))
				return parameter_declaration_list;

			if (!(parameter_declaration_list[i] = parse_parameter_declaration()))
			{
				jump(save);

				return parameter_declaration_list;
			}
		}
	}

	AST parse_parameter_declaration()
	{
		DeclParse(parameter_declaration);

		parameter_declaration["attribute-specifier-seq"] = parse_attribute_specifier_seq();

		if (!(parameter_declaration["decl-specifier-seq"] = parse_decl_specifier_seq()))
			return func.fail();

		size_t start = pos;

		auto abstract_declarator = parse_abstract_declarator();

		size_t end_abstract_declarator = pos;

		jump(start);

		auto declarator = parse_declarator();

		size_t end_declarator = pos;

		jump(start);

		size_t longest_end = max(end_abstract_declarator, end_declarator);

		if (longest_end > start)
		{
			jump(longest_end);

			if (longest_end == end_abstract_declarator)
				parameter_declaration["abstract-declarator"] = abstract_declarator;
			else
				parameter_declaration["declarator"] = declarator;
		}

		if (!drop(OP_ASS))
			return parameter_declaration;

		if (parameter_declaration["initializer-clause"] = parse_initializer_clause())
			return parameter_declaration;
		else
			return func.fail();
	}

	AST parse_function_definition()
	{
		DeclParse(function_definition);

		function_definition["attribute-specifier-seq"] = parse_attribute_specifier_seq();
		if (!(function_definition["decl-specifier-seq"] = parse_decl_specifier_seq()))
			return func.fail();

		if (!(function_definition["declarator"] = parse_declarator()))
			return func.fail();
		function_definition["virt-specifier-seq"] = parse_star(&CppParser::parse_virt_specifier);
		if (!(function_definition["function-body"] = parse_function_body()))
			return func.fail();

		return function_definition;
	}

	AST parse_function_body()
	{
		DeclParse(function_body);

		switch (peek())
		{
		case KW_TRY:
			if (function_body["function-try-block"] = parse_function_try_block())
				return function_body;
			else
				return func.fail();

		case OP_ASS:
			if (peek(1, KW_DEFAULT) && peek(2, OP_SEMICOLON))
			{
				function_body["KW_DEFAULT"] = pop();
				drop();
				return function_body;
			}

			if (peek(1, KW_DELETE) && peek(2, OP_SEMICOLON))
			{
				function_body["KW_DELETE"] = pop();
				drop();
				return function_body;
			}

			return func.fail();

		case OP_COLON:
			function_body["ctor-initializer"] = parse_ctor_initializer();
		case OP_LBRACE:
			if (!(function_body["compound-statement"] = parse_compound_statement()))
				return func.fail();

			return function_body;

		default:
			return func.fail();
		}
	}

	AST parse_initializer()
	{
		DeclParse(initializer);

		switch (peek())
		{
		case OP_ASS:
			drop();
			if (!(initializer["initializer-clause"] = parse_initializer_clause()))
				return func.fail();

			return initializer;

		case OP_LBRACE:
			if (!(initializer["braced-init-list"] = parse_braced_init_list()))
				return func.fail();

			return initializer;

		case OP_LPAREN:
			drop();

			func.nest();

			if (!(initializer["expression-list"] = parse_expression_list()))
				return func.fail();

			if (!drop(OP_RPAREN))
				return func.fail();

			func.unnest();

			return initializer;

		default:
			return func.fail();
		}
	}

	AST parse_brace_or_equal_initializer()
	{
		DeclParse(brace_or_equal_initializer);

		switch (peek())
		{
		case OP_ASS:
			drop();
			if (!(brace_or_equal_initializer["initializer-clause"] = parse_initializer_clause()))
				return func.fail();

			return brace_or_equal_initializer;

		case OP_LBRACE:
			if (!(brace_or_equal_initializer["braced-init-list"] = parse_braced_init_list()))
				return func.fail();

			return brace_or_equal_initializer;

		default:
			return func.fail();
		}
	}

	AST parse_initializer_clause()
	{
		DeclParse(initializer_clause);

		if (peek(OP_LBRACE))
		{
			if (!(initializer_clause["braced-init-list"] = parse_braced_init_list()))
				return func.fail();

			return initializer_clause;
		}

		if (initializer_clause["assignment-expression"] = parse_assignment_expression())
			return initializer_clause;
		else
			return func.fail();
	}

	AST parse_initializer_list()
	{
		DeclParse(initializer_list);

		if (!(initializer_list[0] = parse_initializer_clause_dots()))
			return func.fail();

		for (size_t i = 1; true; i++)
		{
			size_t save = pos;

			if (!drop(OP_COMMA))
				return initializer_list;

			if (!(initializer_list[i] = parse_initializer_clause_dots()))
			{
				jump(save);
				return initializer_list;
			}
		}
	}

	AST parse_initializer_clause_dots()
	{
		DeclParse(initializer_clause_dots);

		if (initializer_clause_dots["initializer-clause"] = parse_initializer_clause())
		{
			initializer_clause_dots["OP_DOTS"] = maybe(OP_DOTS);

			return initializer_clause_dots;
		}
		else
			return func.fail();
	}

	AST parse_braced_init_list()
	{
		DeclParse(braced_init_list);

		if (!drop(OP_LBRACE))
			return func.fail();

		func.nest();

		auto initializer_list = parse_initializer_list();

		if (initializer_list)
			drop(OP_COMMA);
		else
			initializer_list = astempty;


		if (!drop(OP_RBRACE))
			return func.fail();

		func.unnest();

		braced_init_list["initializer-list"] = initializer_list;

		return braced_init_list;
	}

	AST parse_class_specifier()
	{
		DeclParse(class_specifier);

		if (!(class_specifier["class-head"] = parse_class_head()))
			return func.fail();

		if (!drop(OP_LBRACE))
			return func.fail();

		func.nest();

		class_specifier["member-specification-seq"] = parse_star(&CppParser::parse_member_specification);

		if (!drop(OP_RBRACE))
			return func.fail();

		func.unnest();

		return class_specifier;
	}

	AST parse_class_head()
	{
		DeclParse(class_head);

		if (!(class_head["class-key"] = parse_class_key()))
			return func.fail();

		class_head["attribute-specifier-seq"] = parse_attribute_specifier_seq();

		if (class_head["class-head-name"] = parse_class_head_name())
			class_head["class-virt-specifier"] = parse_class_virt_specifier();

		class_head["base-clause"] = parse_base_clause();

		return class_head;
	}

	AST parse_class_head_name()
	{
		DeclParse(class_head_name);

		class_head_name["nested-name-specifier"] = parse_nested_name_specifier();

		if (class_head_name["class-name"] = parse_class_name())
			return class_head_name;
		else
			return func.fail();
	}

	AST parse_class_virt_specifier()
	{
		DeclParse(class_virt_specifier);

		if (peekt()->src.spelling == "final")
		{
			class_virt_specifier["KW_FINAL"] = pop();

			return class_virt_specifier;
		}
		else
			return func.fail();
	}

	AST parse_class_key()
	{
		DeclParse(class_key);

		switch (peek())
		{
		case KW_CLASS:
		case KW_STRUCT:
		case KW_UNION:
			return pop();

		default:
			return func.fail();
		}
	}

	AST parse_member_specification()
	{
		DeclParse(member_specification);

		if (member_specification["access-specifier"] = parse_access_specifier())
		{
			if (!drop(OP_COLON))
				return func.fail();

			return member_specification;
		}
		else if (member_specification["member-declaration"] = parse_member_declaration())
			return member_specification;
		else
			return func.fail();
	}


	AST parse_member_declaration()
	{
		DeclParse(member_declaration);

		if (drop(OP_SEMICOLON))
			return astempty;
		else if (member_declaration["alias-declaration"] = parse_alias_declaration())
			return member_declaration;
		else if (member_declaration["template-declaration"] = parse_template_declaration())
			return member_declaration;
		else if (member_declaration["static_assert-declaration"] = parse_static_assert_declaration())
			return member_declaration;
		else if (member_declaration["using-declaration"] = parse_using_declaration())
			return member_declaration;
		else if (member_declaration["function-definition"] = parse_function_definition())
			return member_declaration;

		member_declaration["attribute-specifier-seq"] = parse_attribute_specifier_seq();
		if (!(member_declaration["decl-specifier-seq"] = parse_decl_specifier_seq()))
			return func.fail();

		member_declaration["member-declarator-list"] = parse_member_declarator_list();

		if (!drop(OP_SEMICOLON))
			return func.fail();

		return member_declaration;
	}

	AST parse_member_declarator_list()
	{
		DeclParse(member_declarator_list);

		if (!(member_declarator_list[0] = parse_member_declarator()))
			return func.fail();

		for (size_t i = 1; true; i++)
		{
			size_t save = pos;

			if (!drop(OP_COMMA))
				return member_declarator_list;

			if (!(member_declarator_list[i] = parse_member_declarator()))
			{
				jump(save);

				return member_declarator_list;
			}
		}
	}

	AST parse_member_declarator()
	{
		DeclParse(member_declarator);

		size_t start = pos;

		auto id = maybe(TT_IDENTIFIER);
		auto attrs = parse_attribute_specifier_seq();

		if (peek(OP_COLON))
		{
			drop();

			member_declarator["TT_IDENTIFIER"] = id;
			member_declarator["attribute-specifier-seq"] = attrs;

			if (member_declarator["constant-expression"] = parse_constant_expression())
				return member_declarator;
			else
				return func.fail();
		}

		jump(start);

		if (member_declarator["declarator"] = parse_declarator())
		{
			member_declarator["virt-specifier-seq"] = parse_plus(&CppParser::parse_virt_specifier);

			member_declarator["pure-specifier"] = parse_pure_specifier();

			if (member_declarator["virt-specifier-seq"] || member_declarator["pure-specifier"])
				return member_declarator;

			if (member_declarator["brace-or-equal-initializer"] = parse_brace_or_equal_initializer())
				return member_declarator;


			return member_declarator;
		}

		return func.fail();
	}

	AST parse_virt_specifier()
	{
		DeclParse(virt_specifier);

		if (peekt()->src.spelling == "override" || peekt()->src.spelling == "final")
			return pop();
		else
			return func.fail();
	}

	AST parse_pure_specifier()
	{
		DeclParse(pure_specifier);

		if (peek(0, OP_ASS) && peekt(1)->src.spelling == "0")
		{
			drop(2);
			return astempty;
		}
		else
			return func.fail();
	}

	AST parse_base_clause()
	{
		DeclParse(base_clause);

		if (!drop(OP_COLON))
			return func.fail();

		if (base_clause["base-specifier-list"] = parse_base_specifier_list())
			return base_clause;
		else
			return func.fail();
	}

	AST parse_base_specifier_list()
	{
		DeclParse(base_specifier_list);

		if (!(base_specifier_list[0] = parse_base_specifier_dots()))
			return func.fail();

		for (size_t i = 1; true; i++)
		{
			size_t save = pos;
			if (!drop(OP_COMMA))
				return base_specifier_list;

			if (!(base_specifier_list[i] = parse_base_specifier_dots()))
			{
				jump(save);
				return base_specifier_list;
			}
		}
	}

	AST parse_base_specifier_dots()
	{
		DeclParse(base_specifier_dots);

		if (!(base_specifier_dots["base-specifier"] = parse_base_specifier()))
			return func.fail();

		base_specifier_dots["OP_DOTS"] = maybe(OP_DOTS);

		return base_specifier_dots;
	}

	AST parse_base_specifier()
	{
		DeclParse(base_specifier);

		base_specifier["attribute-specifier-seq"] = parse_attribute_specifier_seq();

		switch (peek())
		{
		case KW_VIRTUAL:
			base_specifier["KW_VIRTUAL"] = pop();
			base_specifier["access-specifier"] = parse_access_specifier();
			break;

		case KW_PRIVATE:
		case KW_PROTECTED:
		case KW_PUBLIC:
			base_specifier["access-specifier"] = pop();
			base_specifier["KW_VIRTUAL"] = maybe(KW_VIRTUAL);
			break;
		}

		if (base_specifier["base-type-specifier"] = parse_base_type_specifier())
			return base_specifier;
		else
			return func.fail();

	}

	AST parse_class_or_decltype()
	{
		DeclParse(class_or_decltype);

		if (peek(KW_DECLTYPE))
		{
			if (class_or_decltype["decltype-specifier"] = parse_decltype_specifier())
				return class_or_decltype;
			else
				return func.fail();
		}

		class_or_decltype["nested-name-specifier"] = parse_nested_name_specifier();

		if (class_or_decltype["class-name"] = parse_class_name())
			return class_or_decltype;
		else
			return func.fail();
	}

	AST parse_base_type_specifier()
	{
		return parse_class_or_decltype();
	}

	AST parse_access_specifier()
	{
		DeclParse(access_specifier);

		switch (peek())
		{
		case KW_PRIVATE:
		case KW_PROTECTED:
		case KW_PUBLIC:
			return pop();

		default:
			return func.fail();
		}
	}

	AST parse_conversion_function_id()
	{
		DeclParse(conversion_function_id);

		if (!drop(KW_OPERATOR))
			return func.fail();

		if (conversion_function_id["conversion-type-id"] = parse_conversion_type_id())
			return conversion_function_id;
		else
			return func.fail();
	}

	AST parse_conversion_type_id()
	{
		DeclParse(conversion_type_id);

		if (!(conversion_type_id["type-specifier-seq"] = parse_type_specifier_seq()))
			return func.fail();

		conversion_type_id["conversion-declarator"] = parse_star(&CppParser::parse_ptr_operator);

		return conversion_type_id;
	}

	AST parse_ctor_initializer()
	{
		DeclParse(ctor_initializer);

		if (!drop(OP_COLON))
			return func.fail();

		if (ctor_initializer["mem-initializer-list"] = parse_mem_initializer_list())
			return ctor_initializer;
		else
			return func.fail();
	}

	AST parse_mem_initializer_list()
	{
		DeclParse(mem_initializer_list);

		if (!(mem_initializer_list[0] = parse_mem_initializer_dots()))
			return func.fail();

		for (size_t i = 1; true; i++)
		{
			size_t save = pos;

			if (!drop(OP_COMMA))
				return mem_initializer_list;

			if (!(mem_initializer_list[i] = parse_mem_initializer_dots()))
			{
				jump(save);

				return mem_initializer_list;
			}
		}
	}

	AST parse_mem_initializer_dots()
	{
		DeclParse(mem_initializer_dots);

		if (!(mem_initializer_dots["mem-initializer"] = parse_mem_initializer()))
			return func.fail();

		mem_initializer_dots["OP_DOTS"] = maybe(OP_DOTS);

		return mem_initializer_dots;
	}

	AST parse_mem_initializer()
	{
		DeclParse(mem_initializer);

		if (!(mem_initializer["mem-initializer-id"] = parse_mem_initializer_id()))
			return func.fail();

		if (drop(OP_LPAREN))
		{
			func.nest();

			mem_initializer["expression-list"] = parse_expression_list();

			if (!drop(OP_RPAREN))
				return func.fail();

			func.unnest();

			return mem_initializer;
		}

		if (!(mem_initializer["braced-init-list"] = parse_braced_init_list()))
			return func.fail();

		return mem_initializer;
	}

	AST parse_mem_initializer_id()
	{
		DeclParse(mem_initializer_id);

		if (mem_initializer_id["class-or-decltype"] = parse_class_or_decltype())
			return mem_initializer_id;
		else if (mem_initializer_id["TT_IDENTIFIER"] = maybe(TT_IDENTIFIER))
			return mem_initializer_id;
		else
			return func.fail();
	}

	AST parse_operator_function_id()
	{
		DeclParse(operator_function_id);

		if (!(operator_function_id["KW_OPERATOR"] = maybe(KW_OPERATOR)))
			return func.fail();

		switch (peek())
		{
		case KW_NEW:
		case KW_DELETE:
			operator_function_id["op"] = pop();

			if (peek(0, OP_LSQUARE) && peek(1, OP_RSQUARE))
			{
				operator_function_id["OP_LSQUARE"] = pop();
				operator_function_id["OP_RSQUARE"] = pop();
			}

			return operator_function_id;

		case OP_LPAREN:
			operator_function_id["op"] = pop();

			func.nest();

			if (operator_function_id["OP_RPAREN"] = maybe(OP_RPAREN))
				return operator_function_id;
			else
				return func.fail();

		case OP_LSQUARE:
			operator_function_id["op"] = pop();
			if (operator_function_id["OP_RSQUARE"] = maybe(OP_RSQUARE))
				return operator_function_id;
			else
				return func.fail();

		case ST_RSHIFT_1:
			operator_function_id["op"] = pop();
			if (operator_function_id["ST_RSHIFT_2"] = maybe(ST_RSHIFT_2))
				return operator_function_id;
			else
				return func.fail();

		case OP_PLUS:
		case OP_MINUS:
		case OP_STAR:
		case OP_DIV:
		case OP_MOD:
		case OP_XOR:
		case OP_AMP:
		case OP_BOR:
		case OP_COMPL:
		case OP_LNOT:
		case OP_ASS:
		case OP_LT:
		case OP_GT:
		case OP_PLUSASS:
		case OP_MINUSASS:
		case OP_STARASS:
		case OP_DIVASS:
		case OP_MODASS:
		case OP_XORASS:
		case OP_BANDASS:
		case OP_BORASS:
		case OP_LSHIFT:
		case OP_RSHIFTASS:
		case OP_LSHIFTASS:
		case OP_EQ:
		case OP_NE:
		case OP_LE:
		case OP_GE:
		case OP_LAND:
		case OP_LOR:
		case OP_INC:
		case OP_DEC:
		case OP_COMMA:
		case OP_ARROWSTAR:
		case OP_ARROW:
			operator_function_id["op"] = pop();
			return operator_function_id;

		default:
			return func.fail();
		}
	}

	AST parse_literal_operator_id()
	{
		DeclParse(literal_operator_id);

		if (peek(0, KW_OPERATOR) && peekt(1)->src.spelling == "\"\"" && peek(2, TT_IDENTIFIER))
		{
			drop(2);
			literal_operator_id["identifier"] = pop();
			return literal_operator_id;
		}
		else
			return func.fail();
	}

	AST parse_template_declaration()
	{
		DeclParse(template_declaration);

		if (!drop(KW_TEMPLATE))
			return func.fail();

		if (!drop(OP_LT))
			return func.fail();

		func.nest(true);

		if (!(template_declaration["template-parameter-list"] = parse_template_parameter_list()))
			return func.fail();

		if (!parse_close_angle_bracket())
			return func.fail();

		func.unnest();

		if (!(template_declaration["declaration"] = parse_declaration()))
			return func.fail();

		return template_declaration;
	}

	AST parse_template_parameter_list()
	{
		DeclParse(template_parameter_list);

		template_parameter_list[0] = parse_template_parameter();

		for (size_t i = 1; true; i++)
		{
			size_t save = pos;

			if (!drop(OP_COMMA))
				return template_parameter_list;

			if (!(template_parameter_list[i] = parse_template_parameter()))
			{
				jump(save);
				return template_parameter_list;
			}
		}
	}

	AST parse_template_parameter()
	{
		DeclParse(template_parameter);

		if (template_parameter["type-parameter"] = parse_type_parameter())
			return template_parameter;
		else if (template_parameter["parameter-declaration"] = parse_parameter_declaration())
			return template_parameter;
		else
			return func.fail();
	}

	AST parse_type_parameter()
	{
		DeclParse(type_parameter);

		switch (peek())
		{
		case KW_TEMPLATE:
			type_parameter["kw"] = pop();
			if (!drop(OP_LT))
				return func.fail();

			func.nest(true);

			if (!(type_parameter["template-parameter-list"] = parse_template_parameter_list()))
				return func.fail();

			if (!parse_close_angle_bracket())
				return func.fail();

			func.unnest();

			if (!drop(KW_CLASS))
				return func.fail();

			type_parameter["OP_DOTS"] = maybe(OP_DOTS);

			type_parameter["identifier"] = maybe(TT_IDENTIFIER);

			if (!type_parameter["OP_DOTS"] && peek(OP_ASS))
			{
				type_parameter["OP_ASS"] = pop();

				if (type_parameter["id-expression"] = parse_id_expression())
					return type_parameter;
				else
					return func.fail();
			}
			else
				return type_parameter;

		case KW_CLASS:
		case KW_TYPENAME:
			type_parameter["kw"] = pop();

			type_parameter["OP_DOTS"] = maybe(OP_DOTS);

			type_parameter["identifier"] = maybe(TT_IDENTIFIER);

			if (!type_parameter["OP_DOTS"] && peek(OP_ASS))
			{
				type_parameter["OP_ASS"] = pop();

				if (type_parameter["type-id"] = parse_type_id())
					return type_parameter;
				else
					return func.fail();
			}
			else
				return type_parameter;

		default:
			return func.fail();
		}
	}

	AST parse_close_angle_bracket()
	{
		DeclParse(close_angle_bracket);

		switch (peek())
		{
		case OP_GT:
		case ST_RSHIFT_1:
		case ST_RSHIFT_2:
			return pop();

		default:
			return func.fail();
		}
	}

	AST parse_template_id()
	{
		DeclParse(template_id);

		if (template_id["simple-template-id"] = parse_simple_template_id())
			return template_id;

		if ((template_id["operator-function-id"] = parse_operator_function_id()) ||
			(template_id["literal-operator-id"] = parse_literal_operator_id()))
		{
			if (!drop(OP_LT))
				return func.fail();

			func.nest(true);

			template_id["template-argument-list"] = parse_template_argument_list();

			if (!parse_close_angle_bracket())
				return func.fail();

			func.unnest();

			return template_id;
		}
		else
			return func.fail();
	}

	AST parse_template_argument_list()
	{
		DeclParse(template_argument_list);

		if (!(template_argument_list[0] = parse_template_argument_dots()))
			return func.fail();

		for (size_t i = 1; true; i++)
		{
			size_t save = pos;

			if (!drop(OP_COMMA))
				return template_argument_list;

			if (!(template_argument_list[i] = parse_template_argument_dots()))
			{
				jump(save);

				return template_argument_list;
			}
		}
	}

	AST parse_template_argument_dots()
	{
		DeclParse(template_argument_dots);

		if (template_argument_dots["template-argument"] = parse_template_argument())
		{
			template_argument_dots["OP_DOTS"] = maybe(OP_DOTS);

			return template_argument_dots;
		}
		else
			return func.fail();
	}

	AST parse_template_argument()
	{
		DeclParse(template_argument);

		size_t start = pos;

		auto type_id = parse_type_id();

		size_t end_type_id = pos;

		jump(start);

		auto id_expression = parse_id_expression();

		size_t end_id_expression = pos;

		jump(start);

		auto constant_expression = parse_constant_expression();

		size_t end_constant_expression = pos;

		size_t longest_end = max({end_type_id, end_id_expression, end_constant_expression});

		jump(longest_end);

		if (end_type_id == longest_end)
		{
			if (template_argument["type-id"] = type_id)
				return template_argument;
			else
				return func.fail();
		}

		if (end_id_expression == longest_end)
		{
			if (template_argument["id-expression"] = id_expression)
				return template_argument;
			else
				return func.fail();
		}

		if (end_constant_expression == longest_end)
		{
			if (template_argument["constant-expression"] = constant_expression)
				return template_argument;
			else
				return func.fail();
		}

		throw logic_error("internal error #1 parse_template_argument");
	}

	AST parse_typename_specifier()
	{
		DeclParse(typename_specifier);

		if (!drop(KW_TYPENAME))
			return func.fail();

		if (!(typename_specifier["nested-name-specifier"] = parse_nested_name_specifier()))
			return func.fail();

		typename_specifier["KW_TEMPLATE"] = maybe(KW_TEMPLATE);

		if (typename_specifier["simple-template-id"] = parse_simple_template_id())
			return typename_specifier;

		if (typename_specifier["KW_TEMPLATE"])
			return func.fail();

		if (typename_specifier["identifier"] = maybe(TT_IDENTIFIER))
			return typename_specifier;
		else
			return func.fail();
	}

	AST parse_explicit_instantiation()
	{
		DeclParse(explicit_instantiation);

		explicit_instantiation["KW_EXTERN"] = maybe(KW_EXTERN);
		if (!drop(KW_TEMPLATE))
			return func.fail();

		if (explicit_instantiation["declaration"] = parse_declaration())
			return explicit_instantiation;
		else
			return func.fail();
	}

	AST parse_explicit_specialization()
	{
		DeclParse(explicit_specialization);

		if (!drop(KW_TEMPLATE))
			return func.fail();

		if (!drop(OP_LT))
			return func.fail();

		if (!parse_close_angle_bracket())
			return func.fail();

		if (!(explicit_specialization["declaration"] = parse_declaration()))
			return func.fail();

		return explicit_specialization;
	}

	AST parse_try_block()
	{
		DeclParse(try_block);

		try_block["attribute-specifier-seq"] = parse_attribute_specifier_seq();

		if (!drop(KW_TRY))
			return func.fail();

		if (!(try_block["compound-statement"] = parse_compound_statement()))
			return func.fail();

		if (!(try_block["handler-seq"] = parse_plus(&CppParser::parse_handler)))
			return func.fail();

		return try_block;
	}

	AST parse_function_try_block()
	{
		DeclParse(function_try_block);

		if (!drop(KW_TRY))
			return func.fail();

		function_try_block["ctor-initializer"] = parse_ctor_initializer();

		if (!(function_try_block["compound-statement"] = parse_compound_statement()))
			return func.fail();

		if (!(function_try_block["handler-seq"] = parse_plus(&CppParser::parse_handler)))
			return func.fail();

		return function_try_block;
	}

	AST parse_handler()
	{
		DeclParse(handler);

		if (!drop(KW_CATCH))
			return func.fail();

		if (!drop(OP_LPAREN))
			return func.fail();

		func.nest();

		if (!(handler["exception-declaration"] = parse_exception_declaration()))
			return func.fail();

		if (!drop(OP_RPAREN))
			return func.fail();

		func.unnest();

		if (!(handler["compound-statement"] = parse_compound_statement()))
			return func.fail();

		return handler;
	}

	AST parse_exception_declaration()
	{
		DeclParse(exception_declaration);

		if (peek(0, OP_DOTS) && peek(1, OP_RPAREN))
		{
			exception_declaration["dots"] = pop();
			return exception_declaration;
		}

		exception_declaration["attribute-specifier-seq"] = parse_attribute_specifier_seq();

		if (!(exception_declaration["type-specifier-seq"] = parse_type_specifier_seq()))
			return func.fail();

		if (exception_declaration["declarator"] = parse_declarator())
			return exception_declaration;
		else if (exception_declaration["abstract-declarator"] = parse_abstract_declarator())
			return exception_declaration;
		else
			return exception_declaration;
	}

	AST parse_throw_expression()
	{
		DeclParse(throw_expression);

		if (throw_expression["KW_THROW"] = maybe(KW_THROW))
		{
			throw_expression["assignment-expression"] = parse_assignment_expression();

			return throw_expression;
		}
		else
			return func.fail();
	}

	AST parse_exception_specification()
	{
		DeclParse(exception_specification);

		auto dynamic_exception_specification = parse_dynamic_exception_specification();

		if (dynamic_exception_specification)
		{
			exception_specification["dynamic-exception-specification"] = dynamic_exception_specification;
			return exception_specification;
		}

		auto noexcept_specification = parse_noexcept_specification();

		if (noexcept_specification)
		{
			exception_specification["noexcept-specification"] = noexcept_specification;
			return exception_specification;
		}

		return func.fail();
	}


	AST parse_dynamic_exception_specification()
	{
		DeclParse(dynamic_exception_specification);

		if (!(dynamic_exception_specification["KW_THROW"] = maybe(KW_THROW)))
			return func.fail();

		if (!drop(OP_LPAREN))
			return func.fail();

		func.nest();

		dynamic_exception_specification["type-id-list"] = parse_type_id_list();

		if (!drop(OP_RPAREN))
			return func.fail();

		func.unnest();

		return dynamic_exception_specification;
	}

	AST parse_type_id_list()
	{
		DeclParse(type_id_list);

		if (!(type_id_list[0] = parse_type_id_dots()))
			return func.fail();

		for (size_t i = 1; true; i++)
		{
			size_t save = pos;

			if (!drop(OP_COMMA))
				return type_id_list;

			if (!(type_id_list[i] = parse_type_id_dots()))
			{
				jump(save);

				return type_id_list;
			}
		}
	}

	AST parse_type_id_dots()
	{
		DeclParse(type_id_dots);

		if (type_id_dots["type-id"] = parse_type_id())
		{
			type_id_dots["OP_DOTS"] = maybe(OP_DOTS);

			return type_id_dots;
		}
		else
			return func.fail();
	}

	AST parse_noexcept_specification()
	{
		DeclParse(noexcept_specification);

		if (!(noexcept_specification["KW_NOEXCEPT"] = maybe(KW_NOEXCEPT)))
			return func.fail();

		if (peek(OP_LPAREN))
		{
			size_t start_paren = pos;
			drop();

			func.nest();

			AST constant_expression;
			if (!(constant_expression = parse_constant_expression()))
			{
				jump(start_paren);
				return noexcept_specification;
			}

			if (!drop(OP_RPAREN))
			{
				jump(start_paren);
				return noexcept_specification;
			}

			func.unnest();

			noexcept_specification["constant-expression"] = parse_constant_expression();

			return noexcept_specification;
		}

		return noexcept_specification;
	}
};

void CppParse(vector<PToken>&& tokens, bool trace)
{
	vector<PToken> tokens2;

	for (const PToken& token : tokens)
	{
		if (token->token_type != OP_RSHIFT)
			tokens2.push_back(token);
		else
		{
			tokens2.push_back(make_shared<Token>(token->src, ST_RSHIFT_1));
			tokens2.push_back(make_shared<Token>(token->src, ST_RSHIFT_2));
		}
	}
	CppParser parser(move(tokens2), trace);

	if (trace)
		parser.dump_tokens();

	AST test = parser.parse_translation_unit();

	if (!test)
	{
		cout << "parsing failed" << endl;
		parser.dump_deepest();

		throw logic_error("parsing failed");
	}
	else
		cout << test;
}
