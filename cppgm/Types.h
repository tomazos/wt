#pragma once

#include "Pooled.h"
#include "Tokens.h"

struct Expression;

enum ETypeCategory
{
	TC_FUNDAMENTAL,
	TC_POINTER,
	TC_LVALUE_REFERENCE,
	TC_RVALUE_REFERENCE,
	TC_ARRAY,
	TC_FUNCTION,
};

struct CvQualifiers
{
	const bool const_qualified;
	const bool volatile_qualified;

	CvQualifiers(bool const_qualified, bool volatile_qualified)
		: const_qualified(const_qualified)
		, volatile_qualified(volatile_qualified)
	{}

	void write(ostream& o) const
	{
		if (const_qualified && volatile_qualified)
			o << "const volatile ";
		else if (const_qualified)
			o << "const ";
		else if (volatile_qualified)
			o << "volatile ";
	}

	bool operator ==(const CvQualifiers& cv2) const
	{
		return const_qualified == cv2.const_qualified && volatile_qualified == cv2.volatile_qualified;
	}

	bool operator !=(const CvQualifiers& cv2) const { return !(*this == cv2); }

	bool operator <(const CvQualifiers& that) const
	{
		if (!const_qualified && !volatile_qualified && (that.const_qualified || that.volatile_qualified))
			return true;

		if (that.const_qualified && that.volatile_qualified && !(const_qualified && volatile_qualified))
			return true;

		return false;
	}

	bool operator >=(const CvQualifiers& cv2) const { return !(*this < cv2); }
	bool operator >(const CvQualifiers& cv2) const { return cv2 < *this; }
	bool operator <=(const CvQualifiers& cv2) const { return cv2 >= *this; }
};

struct Type : Pooled
{
	const ETypeCategory category;
	const CvQualifiers cv;

	Type(ETypeCategory category, bool const_qualified, bool volatile_qualified)
		: category(category)
		, cv(const_qualified, volatile_qualified)
	{}

	virtual Type* const_qualify() = 0;
	virtual Type* volatile_qualify() = 0;
	virtual Type* unqualify() = 0;

	virtual ~Type() = default;

	virtual void write(ostream&) const = 0;

	virtual bool allowed_reference_to() const { return true; }
	virtual bool allowed_array_element_type() const { return false; }
	virtual bool allowed_function_parameter_type() const { return false; }
	virtual bool allowed_function_return_type() const { return false; }
	virtual bool allowed_pointer_to() { return true; }
	virtual Type* to_function_parameter_type() { return this; }
	virtual Type* strip_ref() { return this; }

	virtual bool is_function() const { return false; }
	virtual bool is_array() const { return false; }
	virtual bool is_array_unknown_bound() const { return false; }
	virtual bool is_reference() const { return false; }
	virtual bool equal(Type* that) const = 0;
	virtual bool reference_related(Type* that) const { return equal(that); }
	virtual bool reference_compatible(Type* that) const { return equal(that); }

	virtual bool is_integral_type() const { return false; }
	virtual bool is_floating_type() const { return false; }
	virtual bool is_arithmetic_type() const { return false; }
	virtual bool is_scalar_type() const { return false; }
	virtual bool is_literal_type() const { return false; }
	virtual bool is_nullptr_t() const { return false; }

	virtual bool is_complete() const { return true; }

	virtual bool is_default_initializable() const { return !cv.const_qualified; }

	virtual size_t size() = 0;
	virtual size_t alignment() { return size(); }

	virtual Expression* convert(Expression* expr) = 0;

	virtual Type* array_to_pointer() { throw logic_error("invalid type for array to pointer"); }

	bool operator<(Type& that);
};

struct FundamentalType : Type
{
	const EFundamentalType type;

	FundamentalType(EFundamentalType type, bool const_qualified, bool volatile_qualified)
		: Type(TC_FUNDAMENTAL, const_qualified, volatile_qualified)
		, type(type)
	{}

	FundamentalType(ETokenType simple_type_keyword, bool const_qualified, bool volatile_qualified)
		: Type(TC_FUNDAMENTAL, const_qualified, volatile_qualified)
		, type(FundamentalTypeFromKeyword(simple_type_keyword))
	{}

	virtual Type* const_qualify()
	{
		return new FundamentalType(type, true, cv.volatile_qualified);
	}

	virtual Type* volatile_qualify()
	{
		return new FundamentalType(type, cv.const_qualified, true);
	}

	virtual Type* unqualify()
	{
		return new FundamentalType(type, false, false);
	}

	void write(ostream& o) const
	{
		cv.write(o);

		o << FundamentalTypeToString(type);
	}

	virtual bool is_integral_type() const
	{
		switch (type)
		{
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
		case FT_WCHAR_T:
		case FT_CHAR:
		case FT_CHAR16_T:
		case FT_CHAR32_T:
		case FT_BOOL:
			return true;

		case FT_FLOAT:
		case FT_DOUBLE:
		case FT_LONG_DOUBLE:
		case FT_VOID:
		case FT_NULLPTR_T:
			return false;

		default:
			throw logic_error("unknown fundamental type");
		}
	}

	virtual bool is_floating_type() const
	{
		switch (type)
		{
		case FT_FLOAT:
		case FT_DOUBLE:
		case FT_LONG_DOUBLE:
			return true;

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
		case FT_WCHAR_T:
		case FT_CHAR:
		case FT_CHAR16_T:
		case FT_CHAR32_T:
		case FT_BOOL:
		case FT_VOID:
		case FT_NULLPTR_T:
			return false;

		default:
			throw logic_error("unknown fundamental type");
		}
	}

	virtual bool is_arithmetic_type() const
	{
		switch (type)
		{
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
		case FT_WCHAR_T:
		case FT_CHAR:
		case FT_CHAR16_T:
		case FT_CHAR32_T:
		case FT_BOOL:
		case FT_FLOAT:
		case FT_DOUBLE:
		case FT_LONG_DOUBLE:
			return true;

		case FT_VOID:
		case FT_NULLPTR_T:
			return false;

		default:
			throw logic_error("unknown fundamental type");
		}
	}

	virtual bool is_scalar_type() const
	{
		switch (type)
		{
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
		case FT_WCHAR_T:
		case FT_CHAR:
		case FT_CHAR16_T:
		case FT_CHAR32_T:
		case FT_BOOL:
		case FT_FLOAT:
		case FT_DOUBLE:
		case FT_LONG_DOUBLE:
		case FT_NULLPTR_T:
			return true;

		case FT_VOID:
			return false;

		default:
			throw logic_error("unknown fundamental type");
		}
	}

	virtual bool is_nullptr_t() const
	{
		return type == FT_NULLPTR_T;
	}

	virtual bool is_literal_type() const
	{
		return is_scalar_type();
	}

	virtual bool allowed_reference_to() const { return type != FT_VOID; }
	virtual bool allowed_array_element_type() const { return type != FT_VOID; }
	virtual bool allowed_function_parameter_type() const { return type != FT_VOID; }
	virtual bool allowed_function_return_type() const { return true; }

	virtual bool equal(Type* that_in) const
	{
		if (auto that = dynamic_cast<FundamentalType*>(that_in))
			return this->type == that->type && this->cv == that->cv;
		else
			return false;
	}

	virtual bool reference_related(Type* that_in) const
	{
		if (auto that = dynamic_cast<FundamentalType*>(that_in))
			return this->type == that->type;
		else
			return false;
	}

	virtual bool reference_compatible(Type* that_in) const
	{
		return reference_related(that_in) && this->cv >= that_in->cv;
	}

	virtual bool is_complete() const { return type != FT_VOID; }

	virtual size_t size() { return SizeOfFundamentalType(type); }

	virtual Expression* convert(Expression* expr);

	bool less(FundamentalType* that) { return this->type < that->type; }
};

struct PointerType : Type
{
	Type* const T;

	PointerType(Type* T, bool const_qualified, bool volatile_qualified)
		: Type(TC_POINTER, const_qualified, volatile_qualified)
		, T(T)
	{
		if (!T->allowed_pointer_to())
			throw logic_error("pointer to that type not allowed");
	};

	virtual Type* const_qualify()
	{
		return new PointerType(T, true, cv.volatile_qualified);
	}

	virtual Type* volatile_qualify()
	{
		return new PointerType(T, cv.const_qualified, true);
	}

	virtual Type* unqualify()
	{
		return new PointerType(T, false, false);
	}

	virtual Type* to_void_pointer()
	{
		return new PointerType(new FundamentalType(FT_VOID, T->cv.const_qualified, T->cv.volatile_qualified), cv.const_qualified, cv.volatile_qualified);
	}

	void write(ostream& o) const
	{
		cv.write(o);

		o << "pointer to ";

		T->write(o);
	}

	virtual bool is_scalar_type() const { return true; }
	virtual bool is_literal_type() const { return true; }

	virtual bool allowed_array_element_type() const { return true; }
	virtual bool allowed_function_parameter_type() const { return !T->is_array_unknown_bound(); }
	virtual bool allowed_function_return_type() const { return true; }

	virtual bool equal(Type* that_in) const
	{
		if (auto that = dynamic_cast<PointerType*>(that_in))
			return this->T->equal(that->T) && this->cv == that->cv;
		else
			return false;
	}

	virtual bool reference_related(Type* that_in) const
	{
		if (auto that = dynamic_cast<PointerType*>(that_in))
			return this->T->equal(that->T);
		else
			return false;
	}

	virtual bool reference_compatible(Type* that_in) const
	{
		return reference_related(that_in) && this->cv >= that_in->cv;
	}

	virtual size_t size() { return 8; }

	virtual Expression* convert(Expression* expr);

	static vector<pair<CvQualifiers, CvQualifiers>> get_cv_qualification_signature(PointerType* source, PointerType* destination);

	bool less(PointerType* that) { return *(this->T) < *(that->T); }
};

struct ReferenceType : Type
{
	Type* const T;

	static Type* make_lvalue_reference(Type* base);
	static Type* make_rvalue_reference(Type* base);

	ReferenceType(ETypeCategory category, Type* T) : Type(category, false, false), T(T)
	{
		if (!T->allowed_reference_to())
			throw logic_error("invalid type for reference to");
	};

	virtual Type* strip_ref() { return T; }

	virtual Type* const_qualify()
	{
		return this;
	}

	virtual Type* volatile_qualify()
	{
		return this;
	}

	virtual Type* unqualify()
	{
		return this;
	}

	virtual bool reference_compatible(Type*) const
	{
		throw logic_error("internal error: reference_compatible on reference?");
	}

	virtual bool is_reference() const { return true; }
	virtual bool is_lvalue_reference() const = 0;
	virtual bool is_rvalue_reference() const = 0;

	virtual bool is_literal_type() const { return true; }

	virtual bool allowed_array_element_type() const { return false; }
	virtual bool allowed_function_parameter_type() const { return !T->is_array_unknown_bound(); }
	virtual bool allowed_function_return_type() const { return true; }
	virtual bool allowed_pointer_to() { return false; }

	virtual size_t size() { throw logic_error("sizeof(reference)"); }
	virtual size_t alignment() { throw logic_error("sizeof(reference)"); }

	virtual bool is_default_initializable() const { return false; }

	virtual Expression* convert(Expression* expr);
};

struct LValueReferenceType : ReferenceType
{
	LValueReferenceType(Type* T) : ReferenceType(TC_LVALUE_REFERENCE, T) {};

	void write(ostream& o) const
	{
		cv.write(o);

		o << "lvalue-reference to ";

		T->write(o);
	}

	virtual bool equal(Type* that_in) const
	{
		if (auto that = dynamic_cast<LValueReferenceType*>(that_in))
			return this->T->equal(that->T);
		else
			return false;
	}

	virtual bool is_lvalue_reference() const { return true; }
	virtual bool is_rvalue_reference() const { return false; };

	bool less(LValueReferenceType* that) { return *(this->T) < *(that->T); }
};

struct RValueReferenceType : ReferenceType
{
	RValueReferenceType(Type* T) : ReferenceType(TC_RVALUE_REFERENCE, T) {};

	void write(ostream& o) const
	{
		cv.write(o);

		o << "rvalue-reference to ";

		T->write(o);
	}

	virtual bool equal(Type* that_in) const
	{
		if (auto that = dynamic_cast<RValueReferenceType*>(that_in))
			return this->T->equal(that->T);
		else
			return false;
	}

	virtual bool is_lvalue_reference() const { return false; }
	virtual bool is_rvalue_reference() const { return true; };

	bool less(RValueReferenceType* that) { return *(this->T) < *(that->T); }
};

struct ArrayType : Type
{
	Type* const element_type;
	const size_t bound; // 0 means unknown bound

	ArrayType(Type* element_type, size_t bound)
		: Type(TC_ARRAY, false, false)
		, element_type(element_type)
		, bound(bound)
	{
		if (!element_type->allowed_array_element_type())
			throw logic_error("invalid array element type");
	}

	void write(ostream& o) const
	{
		cv.write(o);

		o << "array of ";

		if (bound > 0)
			o << bound << " ";
		else
			o << "unknown bound of ";

		element_type->write(o);
	}

	virtual Type* const_qualify()
	{
		return new ArrayType(element_type->const_qualify(), bound);
	}

	virtual Type* volatile_qualify()
	{
		return new ArrayType(element_type->volatile_qualify(), bound);
	}

	virtual Type* unqualify()
	{
		return this;
	}

	virtual bool is_literal_type() const { return element_type->is_literal_type(); }

	virtual bool allowed_array_element_type() const { return bound != 0; }

	virtual Type* to_function_parameter_type() { return new PointerType(element_type, false, false); }

	virtual bool equal(Type* that_in) const
	{
		if (auto that = dynamic_cast<ArrayType*>(that_in))
			return this->element_type->equal(that->element_type) && this->bound == that->bound;
		else
			return false;
	}

	virtual bool is_array() const { return true; }
	virtual bool is_array_unknown_bound() const { return bound == 0; }

	virtual size_t size()
	{
		if (!bound)
			throw logic_error("sizeof(unknown bound)");

		return element_type->size() * bound;
	}

	virtual size_t alignment()
	{
		return element_type->alignment();
	}

	virtual Expression* convert(Expression* expr);

	Type* array_to_pointer()
	{
		return new PointerType(element_type, false, false);
	}

	virtual bool is_complete() const { return bound > 0 && element_type->is_complete(); }

	virtual bool is_default_initializable() const { return element_type->is_default_initializable(); }

	bool less(ArrayType* that)
	{
		if (this->bound < that->bound)
			return true;
		else if (that->bound < this->bound)
			return false;
		else
			return *(this->element_type) < *(that->element_type);
	}
};

struct FunctionType : Type
{
	Type* const return_type;
	const vector<Type*> parameter_type_list;
	const bool varargs;

	FunctionType(Type* return_type, vector<Type*> parameter_type_list, bool varargs)
		: Type(TC_FUNCTION, false, false)
		, return_type(return_type)
		, parameter_type_list(parameter_type_list)
		, varargs(varargs)
	{
		for (Type* parameter_type : parameter_type_list)
			if (!parameter_type->allowed_function_parameter_type())
				throw logic_error("function parameter type not allowed");

		if (!return_type->allowed_function_return_type())
			throw logic_error("function return type not allowed");
	}

	virtual Type* to_function_parameter_type() { return new PointerType(this, false, false); }

	void write(ostream& o) const
	{
		cv.write(o);

		o << "function of (";

		size_t n = parameter_type_list.size();

		for (size_t i = 0; i < n; i++)
		{
			parameter_type_list[i]->write(o);

			if (i < n-1 || varargs)
				o << ", ";
		}

		if (varargs)
			o << "...) returning ";
		else
			o << ") returning ";

		return_type->write(o);
	}

	virtual Type* const_qualify()
	{
		return this;
	}

	virtual Type* volatile_qualify()
	{
		return this;
	}

	virtual Type* unqualify()
	{
		return this;
	}

	bool is_function() const { return true; }

	virtual bool equal(Type* that_in) const
	{
		if (auto that = dynamic_cast<FunctionType*>(that_in))
		{
			if (!(this->return_type->equal(that->return_type)))
				return false;

			if (this->parameter_type_list.size() != that->parameter_type_list.size())
				return false;

			for (size_t i = 0; i < parameter_type_list.size(); i++)
				if (!(this->parameter_type_list[i]->equal(that->parameter_type_list[i])))
					return false;

			if (this->varargs != that->varargs)
				return false;

			return true;
		}
		else
			return false;
	}

	bool signature_equivilant(Type* that_in)
	{
		if (auto that = dynamic_cast<FunctionType*>(that_in))
		{
			if (this->parameter_type_list.size() != that->parameter_type_list.size())
				return false;

			for (size_t i = 0; i < parameter_type_list.size(); i++)
				if (!(this->parameter_type_list[i]->equal(that->parameter_type_list[i])))
					return false;

			if (this->varargs != that->varargs)
				return false;

			return true;
		}
		else
			return false;
	}

	bool less_signature(FunctionType* that)
	{
		if (parameter_type_list.size() < that->parameter_type_list.size())
			return true;
		else if (parameter_type_list.size() > that->parameter_type_list.size())
			return false;
		else
			for (size_t i = 0; i < parameter_type_list.size(); i++)
			{
				if (*(parameter_type_list[i]) < *(that->parameter_type_list[i]))
					return true;
				else if (*(that->parameter_type_list[i]) < *(parameter_type_list[i]))
					return false;
			}

		return this->varargs < that->varargs;
	}

	bool less(FunctionType* that)
	{
		if (parameter_type_list.size() < that->parameter_type_list.size())
			return true;
		else if (parameter_type_list.size() > that->parameter_type_list.size())
			return false;
		else
			for (size_t i = 0; i < parameter_type_list.size(); i++)
			{
				if (*(parameter_type_list[i]) < *(that->parameter_type_list[i]))
					return true;
				else if (*(that->parameter_type_list[i]) < *(parameter_type_list[i]))
					return false;
			}

		if (this->varargs < that->varargs)
			return true;
		else if (that->varargs < this->varargs)
			return false;

		return *(this->return_type) < *(that->return_type);
	}

	virtual size_t size()
	{
		throw logic_error("sizeof(function)");
	}

	virtual size_t alignment()
	{
		throw logic_error("alignof(function)");
	}

	virtual bool is_default_initializable() const { throw logic_error("internal error: functions are not initializable"); }

	virtual Expression* convert(Expression* expr);
};
