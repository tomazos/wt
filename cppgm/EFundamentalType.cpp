#include "std.pch"

#include "EFundamentalType.h"

string FundamentalTypeToString(EFundamentalType e)
{
	switch (e)
	{
	case FT_SIGNED_CHAR: return "signed char";
	case FT_SHORT_INT: return "short int";
	case FT_INT: return "int";
	case FT_LONG_INT: return "long int";
	case FT_LONG_LONG_INT: return "long long int";
	case FT_UNSIGNED_CHAR: return "unsigned char";
	case FT_UNSIGNED_SHORT_INT: return "unsigned short int";
	case FT_UNSIGNED_INT: return "unsigned int";
	case FT_UNSIGNED_LONG_INT: return "unsigned long int";
	case FT_UNSIGNED_LONG_LONG_INT: return "unsigned long long int";
	case FT_WCHAR_T: return "wchar_t";
	case FT_CHAR: return "char";
	case FT_CHAR16_T: return "char16_t";
	case FT_CHAR32_T: return "char32_t";
	case FT_BOOL: return "bool";
	case FT_FLOAT: return "float";
	case FT_DOUBLE: return "double";
	case FT_LONG_DOUBLE: return "long double";
	case FT_VOID: return "void";
	case FT_NULLPTR_T: return "nullptr_t";
	default: throw logic_error("unknown fundamental type: " + to_string(e));
	}
}

EFundamentalType FundamentalTypeFromKeyword(ETokenType simple_type_keyword)
{
	switch (simple_type_keyword)
	{
	case KW_CHAR: return FT_CHAR;
	case KW_CHAR16_T: return FT_CHAR16_T;
	case KW_CHAR32_T: return FT_CHAR32_T;
	case KW_WCHAR_T: return FT_WCHAR_T;
	case KW_BOOL: return FT_BOOL;
	case KW_SHORT: return FT_SHORT_INT;
	case KW_INT: return FT_INT;
	case KW_LONG: return FT_LONG_INT;
	case KW_SIGNED: return FT_INT;
	case KW_UNSIGNED: return FT_UNSIGNED_INT;
	case KW_FLOAT: return FT_FLOAT;
	case KW_DOUBLE: return FT_DOUBLE;
	case KW_VOID: return FT_VOID;
	default: throw logic_error("internal error, invalid simple_type_keyword");
	}
}

size_t SizeOfFundamentalType(EFundamentalType e)
{
	switch (e)
	{
	case FT_SIGNED_CHAR: return 1;
	case FT_SHORT_INT: return 2;
	case FT_INT: return 4;
	case FT_LONG_INT: return 8;
	case FT_LONG_LONG_INT: return 8;
	case FT_UNSIGNED_CHAR: return 1;
	case FT_UNSIGNED_SHORT_INT: return 2;
	case FT_UNSIGNED_INT: return 4;
	case FT_UNSIGNED_LONG_INT: return 8;
	case FT_UNSIGNED_LONG_LONG_INT: return 8;
	case FT_WCHAR_T: return 4;
	case FT_CHAR: return 1;
	case FT_CHAR16_T: return 2;
	case FT_CHAR32_T: return 4;
	case FT_BOOL: return 1;
	case FT_FLOAT: return 4;
	case FT_DOUBLE: return 8;
	case FT_LONG_DOUBLE: return 16;
	case FT_VOID: throw logic_error("sizeof(void)");
	case FT_NULLPTR_T: return 8;
	default: throw logic_error("unknown fundamental type: " + to_string(e));
	}
}
