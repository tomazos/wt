#include "std.pch"

#include "RawExpressions.h"

#include "AnnotatedExpressions.h"
#include "SymbolTable.h"

Expression* LiteralExpression::annotate(SymbolTable& symtab)
{
	switch (literal_token->token_type)
	{
	case KW_TRUE:
		return new Immediate(new FundamentalType(FT_BOOL, false, false), vector<byte>(1,1));

	case KW_FALSE:
		return new Immediate(new FundamentalType(FT_BOOL, false, false), vector<byte>(1,0));

	case KW_NULLPTR:
		return new Immediate(new FundamentalType(FT_NULLPTR_T, false, false), vector<byte>(8, 0));

	case TT_LITERAL:
	{
		auto tok = dynamic_cast<LiteralToken*>(literal_token.get());

		if (tok->is_array)
		{
			Type* type = new ArrayType(new FundamentalType(tok->fundamental_type, true, false), tok->num_elements);

			auto p = new StringLiteral(type, tok->data);
			symtab.string_literals.push_back(p);
			return p;
		}
		else
		{
			return new Immediate(new FundamentalType(tok->fundamental_type, false, false), tok->data);
		}
	}

	case TT_UDSTRING:
	case TT_UDCHARACTER:
	case TT_UDINTEGER:
	case TT_UDFLOATING:
	default:
		throw logic_error("user-defined-literals not yet implemented");
	}
}

Expression* IdExpression::annotate(SymbolTable& symtab)
{
	set<Entity*> entities = symtab.current_lookup_entity(name, false, EC_ANY);

	if (entities.empty())
		throw logic_error("Could not find " + name->get_unqualified_id());

	if (entities.size() > 1)
		throw logic_error("ambiguous id-expression");

	Entity* entity = *entities.begin();

	if (auto variable = dynamic_cast<Variable*>(entity))
		return new VariableExpression(variable->type->strip_ref(), variable);

	if (auto function = dynamic_cast<Function*>(entity))
		return new FunctionExpression(function->type, function);

	throw logic_error("invalid entity for id-expression: " + name->get_unqualified_id());
}
