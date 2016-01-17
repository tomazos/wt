#include "std.pch"

#include "AnnotatedExpressions.h"

#include "Entities.h"

Expression* Immediate::to_fundamental_type(EFundamentalType source, EFundamentalType destination)
{
	if (source == destination)
		return this;

	vector<byte> data_out = ConvertFundamentalValue(source, destination, data);

	return new Immediate(new FundamentalType(destination, false, false), data_out);
}

inline bool FundamentalTypeConversionAllowed(EFundamentalType /*source*/, EFundamentalType /*destination*/)
{
	return true; // TODO
}

Expression* Expression::to_fundamental_type(EFundamentalType source, EFundamentalType destination)
{
	if (source == destination)
		return this;

	if (FundamentalTypeConversionAllowed(source, destination))
		return new FundamentalTypeConversion(new FundamentalType(destination, false, false), this);
	else
		throw logic_error("conversion not allowed");
}

Expression* StringLiteral::to_prvalue()
{
	return new StringLiteralAddress(this);
}

Expression* VariableExpression::to_prvalue()
{
	if (variable->type->is_array())
	{
		return new ArrayVariablePointer(variable->type->array_to_pointer(), variable);
	}
	else
	{
		if (variable->is_constant())
			return variable->initializer_expression->to_prvalue();
		else
			return new LValueToRValueConversion(type, variable);
	}
}

bool VariableExpression::is_constant_expression()
{
	return variable->is_constant();
}

vector<byte> VariableExpression::get_constant_data()
{
	return variable->get_data();
}

Expression* ArrayVariablePointer::relocate()
{
	vector<byte> data(8, 0);

	memcpy(data.data(), &(array->image_offset), 8);

	return new Immediate(type, data);
}

Expression* FunctionExpression::to_prvalue()
{
	return new FunctionAddress(new PointerType(type, false, false), function);
}

Expression* FunctionAddress::relocate()
{
	vector<byte> data(8, 0);

	memcpy(data.data(), &(function->image_offset), 8);

	return new Immediate(type, data);
}

EFundamentalType CollpaseFundamentalUnderlying(EFundamentalType t)
{
	switch (t)
	{
	case FT_WCHAR_T: return FT_INT;
	case FT_CHAR: return FT_SIGNED_CHAR;
	case FT_CHAR16_T: return FT_UNSIGNED_SHORT_INT;
	case FT_CHAR32_T: return FT_UNSIGNED_INT;
	case FT_VOID: throw logic_error("underlying type of void?");
	default: return t;
	}
}

template<typename source_type, typename destination_type>
vector<byte> ConvertFundamental(EFundamentalType /*source*/, EFundamentalType /*destination*/, const vector<byte>& data_in)
{
	source_type s;
	if (sizeof(s) != data_in.size())
		throw logic_error("internal error: convert fundamental #1");

	memcpy(&s, data_in.data(), sizeof(s));

	destination_type d = s;

	return to_byte_vector(d);
}

vector<byte> ConvertFundamental_from_nullptr_t(EFundamentalType /* source */, EFundamentalType destination, const vector<byte>& data_in)
{
	switch (destination)
	{
	case FT_NULLPTR_T:
		return data_in;

	case FT_BOOL:
		return vector<byte>(1, 0);

	default:
		throw logic_error("convert floating type to nullptr_t");
	}
}

template<typename destination_type>
vector<byte> ConvertFundamental(EFundamentalType source, EFundamentalType destination, const vector<byte>& data_in)
{
	switch (source)
	{
	// 3.9.1.2
	case FT_SIGNED_CHAR: return ConvertFundamental<signed char, destination_type> (source, destination, data_in);
	case FT_SHORT_INT: return ConvertFundamental<short int, destination_type>(source, destination, data_in);
	case FT_INT: return ConvertFundamental<int, destination_type>(source, destination, data_in);
	case FT_LONG_INT: return ConvertFundamental<long int, destination_type>(source, destination, data_in);
	case FT_LONG_LONG_INT: return ConvertFundamental<long long int, destination_type>(source, destination, data_in);

	// 3.9.1.3
	case FT_UNSIGNED_CHAR: return ConvertFundamental<unsigned char, destination_type>(source, destination, data_in);
	case FT_UNSIGNED_SHORT_INT: return ConvertFundamental<unsigned short int, destination_type>(source, destination, data_in);
	case FT_UNSIGNED_INT: return ConvertFundamental<unsigned int, destination_type>(source, destination, data_in);
	case FT_UNSIGNED_LONG_INT: return ConvertFundamental<unsigned long int, destination_type>(source, destination, data_in);
	case FT_UNSIGNED_LONG_LONG_INT: return ConvertFundamental<unsigned long long int, destination_type>(source, destination, data_in);

	// 3.9.1.6
	case FT_BOOL: return ConvertFundamental<bool, destination_type>(source, destination, data_in);

	// 3.9.1.8
	case FT_FLOAT: return ConvertFundamental<float, destination_type>(source, destination, data_in);
	case FT_DOUBLE: return ConvertFundamental<double, destination_type>(source, destination, data_in);
	case FT_LONG_DOUBLE: return ConvertFundamental<long double, destination_type>(source, destination, data_in);

	// 3.9.1.10
	case FT_NULLPTR_T: return ConvertFundamental_from_nullptr_t(source, destination, data_in);

	default:
		throw logic_error("internal error: convert fundamental - unexpected source type");
	}
}


vector<byte> ConvertFundamental_to_nullptr_t(EFundamentalType source, EFundamentalType /*destination*/, const vector<byte>& data_in)
{
	switch (source)
	{
	// 3.9.1.2
	case FT_SIGNED_CHAR:
	case FT_SHORT_INT:
	case FT_INT:
	case FT_LONG_INT:
	case FT_LONG_LONG_INT:
	case FT_UNSIGNED_CHAR:
	case FT_UNSIGNED_SHORT_INT:
	case FT_UNSIGNED_INT:
	case FT_UNSIGNED_LONG_INT:
	case FT_UNSIGNED_LONG_LONG_INT:
	case FT_BOOL:
		for (auto c : data_in)
			if (c)
				throw logic_error("nullptr_t initialized with non null pointer constant");
		return vector<byte>(8, 0);

	// 3.9.1.8
	case FT_FLOAT:
	case FT_DOUBLE:
	case FT_LONG_DOUBLE:
		throw logic_error("convert floating type to nullptr_t");

	// 3.9.1.10
	case FT_NULLPTR_T: return data_in;

	default:
		throw logic_error("internal error: unexpected fundamental type");
	}
}

vector<byte> ConvertFundamentalValue(EFundamentalType source, EFundamentalType destination, const vector<byte>& data_in)
{
	source = CollpaseFundamentalUnderlying(source);
	destination = CollpaseFundamentalUnderlying(destination);

	switch (destination)
	{
	// 3.9.1.2
	case FT_SIGNED_CHAR: return ConvertFundamental<signed char>(source, destination, data_in);
	case FT_SHORT_INT: return ConvertFundamental<short int>(source, destination, data_in);
	case FT_INT: return ConvertFundamental<int>(source, destination, data_in);
	case FT_LONG_INT: return ConvertFundamental<long int>(source, destination, data_in);
	case FT_LONG_LONG_INT: return ConvertFundamental<long long int>(source, destination, data_in);

	// 3.9.1.3
	case FT_UNSIGNED_CHAR: return ConvertFundamental<unsigned char>(source, destination, data_in);
	case FT_UNSIGNED_SHORT_INT: return ConvertFundamental<unsigned short int>(source, destination, data_in);
	case FT_UNSIGNED_INT: return ConvertFundamental<unsigned int>(source, destination, data_in);
	case FT_UNSIGNED_LONG_INT: return ConvertFundamental<unsigned long int>(source, destination, data_in);
	case FT_UNSIGNED_LONG_LONG_INT: return ConvertFundamental<unsigned long long int>(source, destination, data_in);

	// 3.9.1.6
	case FT_BOOL: return ConvertFundamental<bool>(source, destination, data_in);

	// 3.9.1.8
	case FT_FLOAT: return ConvertFundamental<float>(source, destination, data_in);
	case FT_DOUBLE: return ConvertFundamental<double>(source, destination, data_in);
	case FT_LONG_DOUBLE: return ConvertFundamental<long double>(source, destination, data_in);

	// 3.9.1.10
	case FT_NULLPTR_T: return ConvertFundamental_to_nullptr_t(source, destination, data_in);

	default:
		throw logic_error("internal error: unexpected fundamental type");
	}
}

void StringLiteral::write(ostream& o) const
{
	o << "StringLiteral (";
	subwrite(o);
	o << " " << HexDump(data) << ")";
}

void Immediate::write(ostream& o) const
{
	o << "Immediate (";
	subwrite(o);
	o << " " << HexDump(data) << ")";
}

void StringLiteralAddress::write(ostream& o) const
{
	o << "StringLiteralAddress (";
	subwrite(o);
	o << " " << *string_literal << ")";
}

void PointerToBoolConversion::write(ostream& o) const
{
	o << "PointerToBoolConversion (";
	subwrite(o);
	o << " " << *operand << ")";
}

void LValueToRValueConversion::write(ostream& o) const
{
	o << "LValueToRValueConversion (";
	subwrite(o);
	o << " " << variable->name << ")";
}

void ArrayVariablePointer::write(ostream& o) const
{
	o << "ArrayVariablePointer (";
	subwrite(o);
	o << " " << array->name << ")";
}


void VariableExpression::write(ostream& o) const
{
	o << "VariableExpression (";
	subwrite(o);
	o << " " << variable->name << ")";
}

void FunctionAddress::write(ostream& o) const
{
	o << "FunctionAddress (";
	subwrite(o);
	o << " " << function->name << ")";
}

void FunctionExpression::write(ostream& o) const
{
	o << "FunctionExpression (";
	subwrite(o);
	o << " " << function->name << ")";
}

void FundamentalTypeConversion::write(ostream& o) const
{
	o << "FundamentalTypeConversion (";
	subwrite(o);
	o << " " << operand << ")";
}

vector<byte> VariableExpression::get_reference_data()
{
	if (variable->has_category(EC_REFERENCE))
		return variable->get_data();
	else
		return to_byte_vector(variable->image_offset);
}

vector<byte> FunctionExpression::get_reference_data() { return to_byte_vector(function->image_offset); }
