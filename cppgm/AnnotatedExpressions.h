#pragma once

#include "Pooled.h"
#include "EFundamentalType.h"
#include "Types.h"

struct Variable;
struct Function;

enum EValueCategory
{
	VC_LVALUE,
	VC_XVALUE,
	VC_PRVALUE
};

inline const char* EValueCategoryToString(EValueCategory e)
{
	switch (e)
	{
	case VC_LVALUE: return "VC_LVALUE";
	case VC_XVALUE: return "VC_XVALUE";
	case VC_PRVALUE: return "VC_PRVALUE";
	default: throw logic_error("internal error: unknown value category");
	}
}

struct Expression : Pooled
{
	EValueCategory value_category;
	Type* type;

	Expression(EValueCategory value_category, Type* type)
		: value_category(value_category)
		, type(type)
	{}

	virtual Expression* to_prvalue()
	{
		if (value_category == VC_PRVALUE)
			return this;
		else
			throw logic_error("internal error: to_prvalue");
	}

	virtual Expression* pointer_to_bool()
	{
		throw logic_error("pointer to bool on invalid expression");
	}

	virtual void set_type(Type* new_type)
	{
		type = new_type;
	}

	virtual Expression* to_fundamental_type(EFundamentalType source, EFundamentalType destination);

	virtual Expression* dereference()
	{
		if (type->is_reference())
			throw logic_error("invalid expression to dereference");

		return this;
	}

	virtual bool is_constant_expression() { return false; }

	virtual bool is_null_pointer_constant() { return false; }

	virtual vector<byte> get_constant_data() { throw logic_error("get_constant_data on non-constant expression"); }

	virtual vector<byte> get_reference_data() { throw logic_error("get_reference_data on invalid expression"); }

	virtual Expression* relocate() { return this; }

	void subwrite(ostream& o) const
	{
		o << EValueCategoryToString(value_category) << " " << *type;
	}
};

struct FundamentalTypeConversion : Expression
{
	Expression* operand;

	FundamentalTypeConversion(Type* type, Expression* operand)
		: Expression(VC_PRVALUE, type)
		, operand(operand)
	{}

	virtual Expression* relocate()
	{
		operand = operand->relocate();

		return operand->to_fundamental_type(
			dynamic_cast<FundamentalType*>(operand->type)->type,
			dynamic_cast<FundamentalType*>(type)->type);
	}

	void write(ostream& out) const;
};

struct StringLiteral : Expression
{
	vector<byte> data;
	size_t image_offset = 0;

	StringLiteral(Type* type, const vector<byte>& data)
		: Expression(VC_LVALUE, type)
		, data(data)
	{}

	Expression* to_prvalue();

	vector<byte> get_reference_data() { return to_byte_vector(image_offset); }

	void write(ostream& o) const;
};

vector<byte> ConvertFundamentalValue(EFundamentalType source, EFundamentalType destination, const vector<byte>& data_in);

struct Immediate : Expression
{
	vector<byte> data;

	Immediate(Type* type, const vector<byte>& data)
		: Expression(VC_PRVALUE, type)
		, data(data)
	{}

	virtual bool is_constant_expression() { return true; }

	virtual bool is_null_pointer_constant()
	{
		if (type->is_nullptr_t())
			return true;

		if (!type->is_integral_type())
			return false;

		for (auto c : data)
			if (c)
				return false;

		return true;
	}

	virtual Expression* to_fundamental_type(EFundamentalType source, EFundamentalType destination);

	vector<byte> get_constant_data() { return data; }

	void write(ostream& o) const;
};

template<typename T>
Expression* MakeImmediate(T t)
{
	Type* type = new FundamentalType(FundamentalTypeOf<T>(), false, false);

	return new Immediate(type, to_byte_vector(t));
}

struct StringLiteralAddress : Expression
{
	StringLiteral* string_literal;

	StringLiteralAddress(StringLiteral* string_literal)
		: Expression(VC_PRVALUE, string_literal->type->array_to_pointer())
		, string_literal(string_literal)
	{}

	StringLiteralAddress(StringLiteral* string_literal, Type* type)
		: Expression(VC_PRVALUE, type)
		, string_literal(string_literal)
	{}

	Expression* pointer_to_bool() { return MakeImmediate(true); }

	Expression* relocate()
	{
		return new Immediate(type, to_byte_vector(string_literal->image_offset));
	}

	void write(ostream& o) const;
};

struct PointerToBoolConversion : Expression
{
	Expression* operand;

	PointerToBoolConversion(Expression* operand)
		: Expression(VC_PRVALUE, new FundamentalType(FT_BOOL, false, false))
		, operand(operand)
	{}

	Expression* relocate()
	{
		return operand->relocate()->pointer_to_bool();
	}

	void write(ostream& o) const;
};

struct LValueToRValueConversion : Expression
{
	Variable* variable;

	LValueToRValueConversion(Type* type, Variable* variable)
		: Expression(VC_PRVALUE, type)
		, variable(variable)
	{}

	Expression* pointer_to_bool()
	{
		if (dynamic_cast<PointerType*>(type))
			return new PointerToBoolConversion(this);
		else
			throw logic_error("pointer to bool on non-pointer");
	}

	void write(ostream& o) const;
};

struct ArrayVariablePointer : Expression
{
	Variable* array;

	ArrayVariablePointer(Type* type, Variable* array)
		: Expression(VC_PRVALUE, type)
		, array(array)
	{}

	Expression* pointer_to_bool() { return MakeImmediate(true); }

	bool is_constant_expression() { return true; }

	Expression* relocate();

	void write(ostream& o) const;
};

struct VariableExpression : Expression
{
	Variable* variable;

	VariableExpression(Type* type, Variable* variable)
		: Expression(VC_LVALUE, type)
		, variable(variable)
	{}

	Expression* to_prvalue();

	bool is_constant_expression();

	vector<byte> get_constant_data();

	vector<byte> get_reference_data();

	void write(ostream& o) const;

};

struct FunctionAddress : Expression
{
	Function* function;

	FunctionAddress(Type* type, Function* function)
		: Expression(VC_PRVALUE, type)
		, function(function)
	{}

	Expression* pointer_to_bool() { return MakeImmediate(true); }

	Expression* relocate();

	void write(ostream& o) const;
};

struct FunctionExpression : Expression
{
	Function* function;

	FunctionExpression(Type* type, Function* function)
		: Expression(VC_LVALUE, type)
		, function(function)
	{}

	Expression* to_prvalue();

	vector<byte> get_reference_data();

	void write(ostream& o) const;
};
