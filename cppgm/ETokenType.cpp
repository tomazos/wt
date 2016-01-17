#include "std.pch"

#include "ETokenType.h"

ETokenType PPTokenToSimpleTokenType(const string& s)
{
	static unordered_map<string, ETokenType> m =
	{
		{"alignas", KW_ALIGNAS},
		{"alignof", KW_ALIGNOF},
		{"asm", KW_ASM},
		{"auto", KW_AUTO},
		{"bool", KW_BOOL},
		{"break", KW_BREAK},
		{"case", KW_CASE},
		{"catch", KW_CATCH},
		{"char", KW_CHAR},
		{"char16_t", KW_CHAR16_T},
		{"char32_t", KW_CHAR32_T},
		{"class", KW_CLASS},
		{"const", KW_CONST},
		{"constexpr", KW_CONSTEXPR},
		{"const_cast", KW_CONST_CAST},
		{"continue", KW_CONTINUE},
		{"decltype", KW_DECLTYPE},
		{"default", KW_DEFAULT},
		{"delete", KW_DELETE},
		{"do", KW_DO},
		{"double", KW_DOUBLE},
		{"dynamic_cast", KW_DYNAMIC_CAST},
		{"else", KW_ELSE},
		{"enum", KW_ENUM},
		{"explicit", KW_EXPLICIT},
		{"export", KW_EXPORT},
		{"extern", KW_EXTERN},
		{"false", KW_FALSE},
		{"float", KW_FLOAT},
		{"for", KW_FOR},
		{"friend", KW_FRIEND},
		{"goto", KW_GOTO},
		{"if", KW_IF},
		{"inline", KW_INLINE},
		{"int", KW_INT},
		{"long", KW_LONG},
		{"mutable", KW_MUTABLE},
		{"namespace", KW_NAMESPACE},
		{"new", KW_NEW},
		{"noexcept", KW_NOEXCEPT},
		{"nullptr", KW_NULLPTR},
		{"operator", KW_OPERATOR},
		{"private", KW_PRIVATE},
		{"protected", KW_PROTECTED},
		{"public", KW_PUBLIC},
		{"register", KW_REGISTER},
		{"reinterpret_cast", KW_REINTERPET_CAST},
		{"return", KW_RETURN},
		{"short", KW_SHORT},
		{"signed", KW_SIGNED},
		{"sizeof", KW_SIZEOF},
		{"static", KW_STATIC},
		{"static_assert", KW_STATIC_ASSERT},
		{"static_cast", KW_STATIC_CAST},
		{"struct", KW_STRUCT},
		{"switch", KW_SWITCH},
		{"template", KW_TEMPLATE},
		{"this", KW_THIS},
		{"thread_local", KW_THREAD_LOCAL},
		{"throw", KW_THROW},
		{"true", KW_TRUE},
		{"try", KW_TRY},
		{"typedef", KW_TYPEDEF},
		{"typeid", KW_TYPEID},
		{"typename", KW_TYPENAME},
		{"union", KW_UNION},
		{"unsigned", KW_UNSIGNED},
		{"using", KW_USING},
		{"virtual", KW_VIRTUAL},
		{"void", KW_VOID},
		{"volatile", KW_VOLATILE},
		{"wchar_t", KW_WCHAR_T},
		{"while", KW_WHILE},
		{"{", OP_LBRACE},
		{"<%", OP_LBRACE},
		{"}", OP_RBRACE},
		{"%>", OP_RBRACE},
		{"[", OP_LSQUARE},
		{"<:", OP_LSQUARE},
		{"]", OP_RSQUARE},
		{":>", OP_RSQUARE},
		{"(", OP_LPAREN},
		{")", OP_RPAREN},
		{"|", OP_BOR},
		{"bitor", OP_BOR},
		{"^", OP_XOR},
		{"xor", OP_XOR},
		{"~", OP_COMPL},
		{"compl", OP_COMPL},
		{"&", OP_AMP},
		{"bitand", OP_AMP},
		{"!", OP_LNOT},
		{"not", OP_LNOT},
		{";", OP_SEMICOLON},
		{":", OP_COLON},
		{"...", OP_DOTS},
		{"?", OP_QMARK},
		{"::", OP_COLON2},
		{".", OP_DOT},
		{".*", OP_DOTSTAR},
		{"+", OP_PLUS},
		{"-", OP_MINUS},
		{"*", OP_STAR},
		{"/", OP_DIV},
		{"%", OP_MOD},
		{"=", OP_ASS},
		{"<", OP_LT},
		{">", OP_GT},
		{"+=", OP_PLUSASS},
		{"-=", OP_MINUSASS},
		{"*=", OP_STARASS},
		{"/=", OP_DIVASS},
		{"%=", OP_MODASS},
		{"^=", OP_XORASS},
		{"xor_eq", OP_XORASS},
		{"&=", OP_BANDASS},
		{"and_eq", OP_BANDASS},
		{"|=", OP_BORASS},
		{"or_eq", OP_BORASS},
		{"<<", OP_LSHIFT},
		{">>", OP_RSHIFT},
		{">>=", OP_RSHIFTASS},
		{"<<=", OP_LSHIFTASS},
		{"==", OP_EQ},
		{"!=", OP_NE},
		{"not_eq", OP_NE},
		{"<=", OP_LE},
		{">=", OP_GE},
		{"&&", OP_LAND},
		{"and", OP_LAND},
		{"||", OP_LOR},
		{"or", OP_LOR},
		{"++", OP_INC},
		{"--", OP_DEC},
		{",", OP_COMMA},
		{"->*", OP_ARROWSTAR},
		{"->", OP_ARROW}
	};

	auto it = m.find(s);

	if (it == m.end())
		return TT_INVALID;
	else
		return it->second;
}

string SimpleTokenTypeToString(ETokenType e)
{
	switch (e)
	{
	case KW_ALIGNAS: return "KW_ALIGNAS";
	case KW_ALIGNOF: return "KW_ALIGNOF";
	case KW_ASM: return "KW_ASM";
	case KW_AUTO: return "KW_AUTO";
	case KW_BOOL: return "KW_BOOL";
	case KW_BREAK: return "KW_BREAK";
	case KW_CASE: return "KW_CASE";
	case KW_CATCH: return "KW_CATCH";
	case KW_CHAR: return "KW_CHAR";
	case KW_CHAR16_T: return "KW_CHAR16_T";
	case KW_CHAR32_T: return "KW_CHAR32_T";
	case KW_CLASS: return "KW_CLASS";
	case KW_CONST: return "KW_CONST";
	case KW_CONSTEXPR: return "KW_CONSTEXPR";
	case KW_CONST_CAST: return "KW_CONST_CAST";
	case KW_CONTINUE: return "KW_CONTINUE";
	case KW_DECLTYPE: return "KW_DECLTYPE";
	case KW_DEFAULT: return "KW_DEFAULT";
	case KW_DELETE: return "KW_DELETE";
	case KW_DO: return "KW_DO";
	case KW_DOUBLE: return "KW_DOUBLE";
	case KW_DYNAMIC_CAST: return "KW_DYNAMIC_CAST";
	case KW_ELSE: return "KW_ELSE";
	case KW_ENUM: return "KW_ENUM";
	case KW_EXPLICIT: return "KW_EXPLICIT";
	case KW_EXPORT: return "KW_EXPORT";
	case KW_EXTERN: return "KW_EXTERN";
	case KW_FALSE: return "KW_FALSE";
	case KW_FLOAT: return "KW_FLOAT";
	case KW_FOR: return "KW_FOR";
	case KW_FRIEND: return "KW_FRIEND";
	case KW_GOTO: return "KW_GOTO";
	case KW_IF: return "KW_IF";
	case KW_INLINE: return "KW_INLINE";
	case KW_INT: return "KW_INT";
	case KW_LONG: return "KW_LONG";
	case KW_MUTABLE: return "KW_MUTABLE";
	case KW_NAMESPACE: return "KW_NAMESPACE";
	case KW_NEW: return "KW_NEW";
	case KW_NOEXCEPT: return "KW_NOEXCEPT";
	case KW_NULLPTR: return "KW_NULLPTR";
	case KW_OPERATOR: return "KW_OPERATOR";
	case KW_PRIVATE: return "KW_PRIVATE";
	case KW_PROTECTED: return "KW_PROTECTED";
	case KW_PUBLIC: return "KW_PUBLIC";
	case KW_REGISTER: return "KW_REGISTER";
	case KW_REINTERPET_CAST: return "KW_REINTERPET_CAST";
	case KW_RETURN: return "KW_RETURN";
	case KW_SHORT: return "KW_SHORT";
	case KW_SIGNED: return "KW_SIGNED";
	case KW_SIZEOF: return "KW_SIZEOF";
	case KW_STATIC: return "KW_STATIC";
	case KW_STATIC_ASSERT: return "KW_STATIC_ASSERT";
	case KW_STATIC_CAST: return "KW_STATIC_CAST";
	case KW_STRUCT: return "KW_STRUCT";
	case KW_SWITCH: return "KW_SWITCH";
	case KW_TEMPLATE: return "KW_TEMPLATE";
	case KW_THIS: return "KW_THIS";
	case KW_THREAD_LOCAL: return "KW_THREAD_LOCAL";
	case KW_THROW: return "KW_THROW";
	case KW_TRUE: return "KW_TRUE";
	case KW_TRY: return "KW_TRY";
	case KW_TYPEDEF: return "KW_TYPEDEF";
	case KW_TYPEID: return "KW_TYPEID";
	case KW_TYPENAME: return "KW_TYPENAME";
	case KW_UNION: return "KW_UNION";
	case KW_UNSIGNED: return "KW_UNSIGNED";
	case KW_USING: return "KW_USING";
	case KW_VIRTUAL: return "KW_VIRTUAL";
	case KW_VOID: return "KW_VOID";
	case KW_VOLATILE: return "KW_VOLATILE";
	case KW_WCHAR_T: return "KW_WCHAR_T";
	case KW_WHILE: return "KW_WHILE";
	case OP_LBRACE: return "OP_LBRACE";
	case OP_RBRACE: return "OP_RBRACE";
	case OP_LSQUARE: return "OP_LSQUARE";
	case OP_RSQUARE: return "OP_RSQUARE";
	case OP_LPAREN: return "OP_LPAREN";
	case OP_RPAREN: return "OP_RPAREN";
	case OP_BOR: return "OP_BOR";
	case OP_XOR: return "OP_XOR";
	case OP_COMPL: return "OP_COMPL";
	case OP_AMP: return "OP_AMP";
	case OP_LNOT: return "OP_LNOT";
	case OP_SEMICOLON: return "OP_SEMICOLON";
	case OP_COLON: return "OP_COLON";
	case OP_DOTS: return "OP_DOTS";
	case OP_QMARK: return "OP_QMARK";
	case OP_COLON2: return "OP_COLON2";
	case OP_DOT: return "OP_DOT";
	case OP_DOTSTAR: return "OP_DOTSTAR";
	case OP_PLUS: return "OP_PLUS";
	case OP_MINUS: return "OP_MINUS";
	case OP_STAR: return "OP_STAR";
	case OP_DIV: return "OP_DIV";
	case OP_MOD: return "OP_MOD";
	case OP_ASS: return "OP_ASS";
	case OP_LT: return "OP_LT";
	case OP_GT: return "OP_GT";
	case OP_PLUSASS: return "OP_PLUSASS";
	case OP_MINUSASS: return "OP_MINUSASS";
	case OP_STARASS: return "OP_STARASS";
	case OP_DIVASS: return "OP_DIVASS";
	case OP_MODASS: return "OP_MODASS";
	case OP_XORASS: return "OP_XORASS";
	case OP_BANDASS: return "OP_BANDASS";
	case OP_BORASS: return "OP_BORASS";
	case OP_LSHIFT: return "OP_LSHIFT";
	case OP_RSHIFT: return "OP_RSHIFT";
	case OP_RSHIFTASS: return "OP_RSHIFTASS";
	case OP_LSHIFTASS: return "OP_LSHIFTASS";
	case OP_EQ: return "OP_EQ";
	case OP_NE: return "OP_NE";
	case OP_LE: return "OP_LE";
	case OP_GE: return "OP_GE";
	case OP_LAND: return "OP_LAND";
	case OP_LOR: return "OP_LOR";
	case OP_INC: return "OP_INC";
	case OP_DEC: return "OP_DEC";
	case OP_COMMA: return "OP_COMMA";
	case OP_ARROWSTAR: return "OP_ARROWSTAR";
	case OP_ARROW: return "OP_ARROW";
	case TT_INVALID: return "TT_INVALID";
	case TT_EOF: return "TT_EOF";
	case TT_IDENTIFIER: return "TT_IDENTIFIER";
	case TT_LITERAL: return "TT_LITERAL";
	case TT_UDSTRING: return "TT_UDSTRING";
	case TT_UDCHARACTER: return "TT_UDCHARACTER";
	case TT_UDINTEGER: return "TT_UDINTEGER";
	case TT_UDFLOATING: return "TT_UDFLOATING";
	case ST_RSHIFT_1: return "ST_RSHIFT_1";
	case ST_RSHIFT_2: return "ST_RSHIFT_2";

	default: return "??? unknown token type";
	};
}
