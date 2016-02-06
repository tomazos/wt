#pragma once

// ETokenType PPTokenToSimpleTokenType(const string& s);
// string SimpleTokenTypeToString(ETokenType e);

enum ETokenType {
  TT_INVALID,  // error: unknown terminal type
  TT_EOF,      // eof

  // complex
  TT_IDENTIFIER,
  TT_LITERAL,
  TT_UDSTRING,
  TT_UDCHARACTER,
  TT_UDINTEGER,
  TT_UDFLOATING,

  // special tokens
  ST_RSHIFT_1,
  ST_RSHIFT_2,

  // keywords
  KW_ALIGNAS,
  KW_ALIGNOF,
  KW_ASM,
  KW_AUTO,
  KW_BOOL,
  KW_BREAK,
  KW_CASE,
  KW_CATCH,
  KW_CHAR,
  KW_CHAR16_T,
  KW_CHAR32_T,
  KW_CLASS,
  KW_CONST,
  KW_CONSTEXPR,
  KW_CONST_CAST,
  KW_CONTINUE,
  KW_DECLTYPE,
  KW_DEFAULT,
  KW_DELETE,
  KW_DO,
  KW_DOUBLE,
  KW_DYNAMIC_CAST,
  KW_ELSE,
  KW_ENUM,
  KW_EXPLICIT,
  KW_EXPORT,
  KW_EXTERN,
  KW_FALSE,
  KW_FLOAT,
  KW_FOR,
  KW_FRIEND,
  KW_GOTO,
  KW_IF,
  KW_INLINE,
  KW_INT,
  KW_LONG,
  KW_MUTABLE,
  KW_NAMESPACE,
  KW_NEW,
  KW_NOEXCEPT,
  KW_NULLPTR,
  KW_OPERATOR,
  KW_PRIVATE,
  KW_PROTECTED,
  KW_PUBLIC,
  KW_REGISTER,
  KW_REINTERPET_CAST,
  KW_RETURN,
  KW_SHORT,
  KW_SIGNED,
  KW_SIZEOF,
  KW_STATIC,
  KW_STATIC_ASSERT,
  KW_STATIC_CAST,
  KW_STRUCT,
  KW_SWITCH,
  KW_TEMPLATE,
  KW_THIS,
  KW_THREAD_LOCAL,
  KW_THROW,
  KW_TRUE,
  KW_TRY,
  KW_TYPEDEF,
  KW_TYPEID,
  KW_TYPENAME,
  KW_UNION,
  KW_UNSIGNED,
  KW_USING,
  KW_VIRTUAL,
  KW_VOID,
  KW_VOLATILE,
  KW_WCHAR_T,
  KW_WHILE,

  // operators/punctuation
  OP_LBRACE,
  OP_RBRACE,
  OP_LSQUARE,
  OP_RSQUARE,
  OP_LPAREN,
  OP_RPAREN,
  OP_BOR,
  OP_XOR,
  OP_COMPL,
  OP_AMP,
  OP_LNOT,
  OP_SEMICOLON,
  OP_COLON,
  OP_DOTS,
  OP_QMARK,
  OP_COLON2,
  OP_DOT,
  OP_DOTSTAR,
  OP_PLUS,
  OP_MINUS,
  OP_STAR,
  OP_DIV,
  OP_MOD,
  OP_ASS,
  OP_LT,
  OP_GT,
  OP_PLUSASS,
  OP_MINUSASS,
  OP_STARASS,
  OP_DIVASS,
  OP_MODASS,
  OP_XORASS,
  OP_BANDASS,
  OP_BORASS,
  OP_LSHIFT,
  OP_RSHIFT,
  OP_RSHIFTASS,
  OP_LSHIFTASS,
  OP_EQ,
  OP_NE,
  OP_LE,
  OP_GE,
  OP_LAND,
  OP_LOR,
  OP_INC,
  OP_DEC,
  OP_COMMA,
  OP_ARROWSTAR,
  OP_ARROW,
};

ETokenType PPTokenToSimpleTokenType(const string& s);
// given s: a keyword, operator or punctuation preprocessing-token
// return: the ETokenType or TT_INVALID if not found
// eg PPTokenToSimpleTokenType(">>=") returns OP_RSHIFTASS

string SimpleTokenTypeToString(ETokenType e);
// given e: an ETerminalType
// return: the string version (#e) of its name
// eg SimpleTokenTypeToString(OP_RSHIFTASS) returns "OP_RSHIFTASS"