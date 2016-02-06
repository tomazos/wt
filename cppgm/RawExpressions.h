#pragma once

#include "Names.h"
#include "Tokens.h"

struct SymbolTable;
struct Expression;

enum EBinaryOperation {
  BO_SUBSCRIPT,    // x[y]
  BO_MULT,         // x * y
  BO_DIV,          // x / y
  BO_MOD,          // x % y
  BO_ADD,          // x + y
  BO_SUBTRACT,     // x - y
  BO_LSHIFT,       // x << y
  BO_RSHIFT,       // x >> y
  BO_GT,           // x > y
  BO_LT,           // x < y
  BO_LE,           // x <= y
  BO_GE,           // x >= y
  BO_EQ,           // x == y
  BO_NE,           // x != y
  BO_BAND,         // x & y
  BO_XOR,          // x ^ y
  BO_BOR,          // x | y
  BO_LAND,         // x && y
  BO_LOR,          // x || y
  BO_ASS,          // x = y
  BO_MULTASS,      // x *= y
  BO_DIVASS,       // x /= y
  BO_MODASS,       // x %= y
  BO_ADDASS,       // x += y
  BO_SUBTRACTASS,  // x -= y
  BO_RSHIFTASS,    // x >>= y
  BO_LSHIFTASS,    // x <<= y
  BO_BANDASS,      // x &= y
  BO_XORASS,       // x ^= y
  BO_BORASS,       // x |= y
  BO_COMMA,        // x , y
};

inline const char* EBinaryOperationToString(EBinaryOperation e) {
  switch (e) {
    case BO_SUBSCRIPT:
      return "BO_SUBSCRIPT";
    case BO_MULT:
      return "BO_MULT";
    case BO_DIV:
      return "BO_DIV";
    case BO_MOD:
      return "BO_MOD";
    case BO_ADD:
      return "BO_ADD";
    case BO_SUBTRACT:
      return "BO_SUBTRACT";
    case BO_LSHIFT:
      return "BO_LSHIFT";
    case BO_RSHIFT:
      return "BO_RSHIFT";
    case BO_GT:
      return "BO_GT";
    case BO_LT:
      return "BO_LT";
    case BO_LE:
      return "BO_LE";
    case BO_GE:
      return "BO_GE";
    case BO_EQ:
      return "BO_EQ";
    case BO_NE:
      return "BO_NE";
    case BO_BAND:
      return "BO_BAND";
    case BO_XOR:
      return "BO_XOR";
    case BO_BOR:
      return "BO_BOR";
    case BO_LAND:
      return "BO_LAND";
    case BO_LOR:
      return "BO_LOR";
    case BO_ASS:
      return "BO_ASS";
    case BO_MULTASS:
      return "BO_MULTASS";
    case BO_DIVASS:
      return "BO_DIVASS";
    case BO_MODASS:
      return "BO_MODASS";
    case BO_ADDASS:
      return "BO_ADDASS";
    case BO_SUBTRACTASS:
      return "BO_SUBTRACTASS";
    case BO_RSHIFTASS:
      return "BO_RSHIFTASS";
    case BO_LSHIFTASS:
      return "BO_LSHIFTASS";
    case BO_BANDASS:
      return "BO_BANDASS";
    case BO_XORASS:
      return "BO_XORASS";
    case BO_BORASS:
      return "BO_BORASS";
    case BO_COMMA:
      return "BO_COMMA";
    default:
      throw logic_error("unknown binary operation");
  }
}

enum EUnaryOperation {
  UO_POSTINC,  // x++
  UO_POSTDEC,  // x--
  UO_PREINC,   // ++x
  UO_PREDEC,   // --x
  UO_INDIR,    // *x
  UO_ADDROF,   // &x
  UO_POS,      // +x
  UO_NEG,      // -x
  UO_LNOT,     // !x
  UO_COMPL,    // ~x
};

inline const char* EUnaryOperationToString(EUnaryOperation e) {
  switch (e) {
    case UO_POSTINC:
      return "UO_POSTINC";
    case UO_POSTDEC:
      return "UO_POSTDEC";
    case UO_PREINC:
      return "UO_PREINC";
    case UO_PREDEC:
      return "UO_PREDEC";
    case UO_INDIR:
      return "UO_INDIR";
    case UO_ADDROF:
      return "UO_ADDROF";
    case UO_POS:
      return "UO_POS";
    case UO_NEG:
      return "UO_NEG";
    case UO_LNOT:
      return "UO_LNOT";
    case UO_COMPL:
      return "UO_COMPL";
    default:
      throw logic_error("unknown unary operation");
  }
}

enum ECastCategory {
  CT_EXPLICIT,
  CT_DYNAMIC,
  CT_STATIC,
  CT_REINTERPRET,
  CT_CONST,
};

inline const char* ECastCategoryToString(ECastCategory e) {
  switch (e) {
    case CT_EXPLICIT:
      return "CT_EXPLICIT";
    case CT_DYNAMIC:
      return "CT_DYNAMIC";
    case CT_STATIC:
      return "CT_STATIC";
    case CT_REINTERPRET:
      return "CT_REINTERPRET";
    case CT_CONST:
      return "CT_CONST";
    default:
      throw logic_error("unknown cast category");
  }
}

struct RawExpression : Pooled {
  virtual Expression* annotate(SymbolTable& symtab) = 0;
};

struct ExpressionList : Pooled {
  const vector<RawExpression*> expressions;

  ExpressionList(const vector<RawExpression*>& expressions)
      : expressions(expressions) {}

  void write(ostream& out) const {
    out << "ExpressionList (";
    for (auto expr : expressions) out << expr << ", ";
    out << ")";
  }
};

struct LiteralExpression : RawExpression {
  PToken literal_token;

  LiteralExpression(PToken literal_token) : literal_token(literal_token) {}

  virtual Expression* annotate(SymbolTable& symtab);

  template <class T>
  size_t convert_array_bound(T t) {
    if (t < T(0)) throw logic_error("negative array bound");

    return size_t(t);
  }

  size_t evaluate_array_bound() {
    if (literal_token->token_type != TT_LITERAL)
      throw logic_error("expected literal");

    LiteralToken& tok = dynamic_cast<LiteralToken&>(*literal_token);

    switch (tok.fundamental_type) {
      case FT_UNSIGNED_CHAR:
        return tok.get<unsigned char>();
      case FT_UNSIGNED_SHORT_INT:
        return tok.get<unsigned short int>();
      case FT_UNSIGNED_INT:
        return tok.get<unsigned int>();
      case FT_UNSIGNED_LONG_INT:
        return tok.get<unsigned long int>();
      case FT_UNSIGNED_LONG_LONG_INT:
        return tok.get<unsigned long long int>();

      case FT_SIGNED_CHAR:
        return convert_array_bound(tok.get<signed char>());
      case FT_SHORT_INT:
        return convert_array_bound(tok.get<short int>());
      case FT_INT:
        return convert_array_bound(tok.get<int>());
      case FT_LONG_INT:
        return convert_array_bound(tok.get<long int>());
      case FT_LONG_LONG_INT:
        return convert_array_bound(tok.get<long long int>());

      case FT_WCHAR_T:
        return convert_array_bound(tok.get<wchar_t>());
      case FT_CHAR:
        return convert_array_bound(tok.get<char>());
      case FT_CHAR16_T:
        return tok.get<char16_t>();
      case FT_CHAR32_T:
        return tok.get<char32_t>();

      case FT_BOOL:
        return tok.get<bool>();

      case FT_VOID:
      case FT_NULLPTR_T:
        throw logic_error("unexpected literal type as array bound");

      case FT_FLOAT:
      case FT_DOUBLE:
      case FT_LONG_DOUBLE:
        throw logic_error("floating literal as array bound");

      default:
        throw logic_error("unknown fundamental type (#1)");
    }
  }

  void write(ostream& out) const {
    out << "LiteralExpression (";
    literal_token->write(out);
    out << ")";
  }
};

struct IdExpression : RawExpression {
  QualifiedName* const name;

  IdExpression(QualifiedName* name) : name(name){};

  virtual Expression* annotate(SymbolTable& symtab);

  void write(ostream& out) const { out << "IdExpression (" << name << ")"; }
};

struct CastExpression : RawExpression {
  const ECastCategory cast_category;
  ExpressionList* const expression_list;
  Type* const destination_type;

  CastExpression(ECastCategory cast_category, Type* type,
                 RawExpression* expression)
      : cast_category(cast_category),
        expression_list(new ExpressionList(vector<RawExpression*>{expression})),
        destination_type(type) {}

  CastExpression(Type* type, RawExpression* expression)
      : cast_category(CT_EXPLICIT),
        expression_list(new ExpressionList(vector<RawExpression*>{expression})),
        destination_type(type) {}

  CastExpression(Type* type, ExpressionList* expression_list)
      : cast_category(CT_EXPLICIT),
        expression_list(expression_list),
        destination_type(type) {}

  virtual Expression* annotate(SymbolTable&) {
    throw logic_error("not yet implemented");
  }

  void write(ostream& out) const {
    out << "CastExpression (" << ECastCategoryToString(cast_category) << " "
        << destination_type << " " << expression_list << ")";
  }
};

struct FunctionCallExpression : RawExpression {
  RawExpression* const function;
  ExpressionList* const arguments;
  FunctionCallExpression(RawExpression* function, ExpressionList* arguments)
      : function(function), arguments(arguments) {}

  virtual Expression* annotate(SymbolTable&) {
    throw logic_error("not yet implemented");
  }

  void write(ostream& out) const {
    out << "FunctionCallExpression (" << function << " : " << arguments << ")";
  }
};

struct UnaryExpression : RawExpression {
  const EUnaryOperation operation;
  RawExpression* const operand;

  UnaryExpression(EUnaryOperation operation, RawExpression* operand)
      : operation(operation), operand(operand) {}

  virtual Expression* annotate(SymbolTable&) {
    throw logic_error("not yet implemented");
  }

  void write(ostream& out) const {
    out << "UnaryExpression (" << EUnaryOperationToString(operation) << " "
        << operand << ")";
  }
};

struct BinaryExpression : RawExpression {
  const EBinaryOperation operation;
  RawExpression* const lhs;
  RawExpression* const rhs;

  BinaryExpression(EBinaryOperation operation, RawExpression* lhs,
                   RawExpression* rhs)
      : operation(operation), lhs(lhs), rhs(rhs) {}

  virtual Expression* annotate(SymbolTable&) {
    throw logic_error("not yet implemented");
  }

  void write(ostream& out) const {
    out << "UnaryExpression (" << EBinaryOperationToString(operation) << " "
        << lhs << " : " << rhs << ")";
  }
};

struct SizeOfExpression : RawExpression {
  Type* const type;
  RawExpression* const expression;

  SizeOfExpression(Type* type) : type(type), expression(nullptr){};
  SizeOfExpression(RawExpression* expression)
      : type(nullptr), expression(expression){};

  virtual Expression* annotate(SymbolTable&) {
    throw logic_error("not yet implemented");
  }

  void write(ostream& out) const {
    out << "SizeOfExpression (" << type << " " << expression << ")";
  }
};

struct ConditionalExpression : RawExpression {
  RawExpression* const condition;
  RawExpression* const ontrue;
  RawExpression* const onfalse;

  ConditionalExpression(RawExpression* condition, RawExpression* ontrue,
                        RawExpression* onfalse)
      : condition(condition), ontrue(ontrue), onfalse(onfalse) {}

  virtual Expression* annotate(SymbolTable&) {
    throw logic_error("not yet implemented");
  }

  void write(ostream& out) const {
    out << "ConditionalExpression (" << condition << " ? " << ontrue << " : "
        << onfalse << ")";
  }
};
