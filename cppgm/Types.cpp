#include "std.pch"

#include "Types.h"

#include "AnnotatedExpressions.h"

bool Type::operator<(Type& that)
{
	if (cv.const_qualified < that.cv.const_qualified)
		return true;
	else if (that.cv.const_qualified < cv.const_qualified)
		return false;
	else if (cv.volatile_qualified < that.cv.volatile_qualified)
		return true;
	else if (that.cv.volatile_qualified < cv.volatile_qualified)
		return false;
	else if (category < that.category)
		return true;
	else if (that.category < category)
		return false;
	else switch (category)
	{
	case TC_FUNDAMENTAL:
		return dynamic_cast<FundamentalType*>(this)->less(dynamic_cast<FundamentalType*>(&that));

	case TC_POINTER:
		return dynamic_cast<PointerType*>(this)->less(dynamic_cast<PointerType*>(&that));

	case TC_LVALUE_REFERENCE:
		return dynamic_cast<LValueReferenceType*>(this)->less(dynamic_cast<LValueReferenceType*>(&that));

	case TC_RVALUE_REFERENCE:
		return dynamic_cast<RValueReferenceType*>(this)->less(dynamic_cast<RValueReferenceType*>(&that));

	case TC_ARRAY:
		return dynamic_cast<ArrayType*>(this)->less(dynamic_cast<ArrayType*>(&that));

	case TC_FUNCTION:
		return dynamic_cast<FunctionType*>(this)->less(dynamic_cast<FunctionType*>(&that));

	default:
		throw logic_error("internal error: wrong type category");
	}
}

Type* ReferenceType::make_lvalue_reference(Type* base)
{
	if (auto ref = dynamic_cast<ReferenceType*>(base))
		return new LValueReferenceType(ref->T);
	else
		return new LValueReferenceType(base);
}

Type* ReferenceType::make_rvalue_reference(Type* base)
{
	if (base->is_reference())
		return base;
	else
		return new RValueReferenceType(base);
}

Expression* FundamentalType::convert(Expression* expr)
{
	expr = expr->to_prvalue();

	if (type == FT_BOOL)
	{
		if (dynamic_cast<PointerType*>(expr->type))
			return expr->pointer_to_bool();
	}

	auto type_in = dynamic_cast<FundamentalType*>(expr->type);

	if (!type_in)
		throw logic_error("unable to convert expr to fundamental type");

	return expr->to_fundamental_type(type_in->type, type);
}

vector<pair<CvQualifiers, CvQualifiers>> PointerType::get_cv_qualification_signature(PointerType* source, PointerType* destination)
{
	if (source->unqualify()->equal(destination->unqualify()))
		return {};

	vector<PointerType*> sources;
	vector<PointerType*> destinations;

	while (true)
	{
		sources.push_back(source);
		destinations.push_back(destination);

		if (source->T->unqualify()->equal(destination->T->unqualify()))
			break;

		if (!(source = dynamic_cast<PointerType*>(source->T)))
			throw logic_error("invalid pointer conversion");

		if (!(destination = dynamic_cast<PointerType*>(destination->T)))
			throw logic_error("invalid pointer conversion");
	}

	vector<pair<CvQualifiers, CvQualifiers>> output;

	for (size_t i = 0; i < sources.size(); i++)
		output.push_back(make_pair(sources[i]->T->cv, destinations[i]->T->cv));

	return output;
}

Expression* PointerType::convert(Expression* expr)
{
	expr = expr->to_prvalue();

	if (equal(expr->type))
		return expr;

	if (expr->is_null_pointer_constant())
		return new Immediate(this, vector<byte>(8,0));

	if (auto ft = dynamic_cast<FundamentalType*>(T))
		if (ft->type == FT_VOID)
			if (auto source_ptr_type = dynamic_cast<PointerType*>(expr->type))
				expr->set_type(source_ptr_type->to_void_pointer());

	if (equal(expr->type))
		return expr;

	if (auto source_ptr = dynamic_cast<PointerType*>(expr->type))
	{
		vector<pair<CvQualifiers, CvQualifiers>> signature = get_cv_qualification_signature(source_ptr, this);

		for (auto p : signature)
			if (p.first > p.second)
				throw logic_error("invalid qualification conversion");

		for (size_t j = 0; j < signature.size(); j++)
			if (signature[j].first != signature[j].second)
				for (size_t k = 1; k < j; k++)
					if (!signature[k].second.const_qualified)
						throw logic_error("invalid qualification conversion");

		expr->set_type(this);

		return expr;
	}
	else
		throw logic_error("unable to convert to pointer");
}

Expression* ReferenceType::convert(Expression* expr)
{
	if (is_lvalue_reference() && expr->value_category == VC_LVALUE && T->reference_compatible(expr->type))
		return expr;

	if (!(is_lvalue_reference() && T->cv.const_qualified && !T->cv.volatile_qualified)
		&& !is_rvalue_reference())
		throw logic_error("ill-formed reference binding");

	if ((expr->value_category == VC_XVALUE ||
			(expr->value_category == VC_PRVALUE && expr->type->is_array()) ||
			(expr->value_category == VC_LVALUE && expr->type->is_function()))
			&& T->reference_compatible(expr->type))
		return expr;

	auto tmp = T->convert(expr);

	if (T->reference_related(expr->type))
	{
		if (T->cv < expr->type->cv)
			throw logic_error("ill-formed reference binding: reference-related cv qualification mismatch");

		if (is_rvalue_reference() && expr->value_category == VC_LVALUE)
			throw logic_error("binding rvalue-reference to reference-related lvalue");
	}

	return tmp;
}

Expression* ArrayType::convert(Expression* expr)
{
	if (auto string_literal = dynamic_cast<StringLiteral*>(expr))
	{
		if (auto string_literal_type = dynamic_cast<ArrayType*>(string_literal->type))
		{
			FundamentalType* destination_element_type = dynamic_cast<FundamentalType*>(element_type->unqualify());

			if (!destination_element_type)
				throw logic_error("invalid destination element type");

			FundamentalType* source_element_type = dynamic_cast<FundamentalType*>(string_literal_type->element_type->unqualify());

			size_t char_size;

			switch (source_element_type->type)
			{
			case FT_CHAR:
				char_size = 1;
				if (destination_element_type->type != FT_CHAR
					&& destination_element_type->type != FT_UNSIGNED_CHAR
					&& destination_element_type->type != FT_SIGNED_CHAR)
					throw logic_error("invalid destination for narrow string literal");
			    break;
			case FT_CHAR16_T:
				char_size = 2;
				if (destination_element_type->type != FT_CHAR16_T)
					throw logic_error("invalid destination for char16_t character literal");
				break;
			case FT_CHAR32_T:
				char_size = 4;
				if (destination_element_type->type != FT_CHAR32_T)
					throw logic_error("invalid destination for char32_t character literal");
				break;
			case FT_WCHAR_T:
				char_size = 4;
				if (destination_element_type->type != FT_WCHAR_T)
					throw logic_error("invalid destination for char32_t character literal");
				break;
			default:
				throw logic_error("invalid destination for string literal");
			}

			size_t new_bound = bound;

			if (bound == 0)
				new_bound = string_literal_type->bound;

			if (new_bound < string_literal_type->bound)
				throw logic_error("desination array can't fit string literal");

			vector<byte> new_data(new_bound*char_size, 0);

			copy(string_literal->data.begin(), string_literal->data.end(), new_data.begin());

			return new Immediate(new ArrayType(element_type, new_bound), new_data);
		}
		else
			throw logic_error("internal error: string literal not of array type");
	}
	else
	{
		throw logic_error("invalid initialization of array");
	}
}

Expression* FunctionType::convert(Expression*)
{
	throw logic_error("attempt to initialize a function with an expression");
}
