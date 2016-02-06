#pragma once

#include "RawExpressions.h"

struct ParameterDeclarationClause;
struct Statement;
struct Namespace;
struct Entity;

typedef unsigned int SpecifiersBitField;

enum ESpecifier {
  // [dcl.stc]
  SP_REGISTER = 1 << 0,
  SP_STATIC = 1 << 1,
  SP_THREAD_LOCAL = 1 << 2,
  SP_EXTERN = 1 << 3,
  SP_MUTABLE = 1 << 4,

  // [dcl.fct.spec]
  SP_INLINE = 1 << 5,
  SP_VIRTUAL = 1 << 6,
  SP_EXPLICIT = 1 << 7,

  // [dcl.typedef]
  SP_TYPEDEF = 1 << 8,

  // [dcl.friend]
  SP_FRIEND = 1 << 9,

  // [dcl.constexpr]
  SP_CONSTEXPR = 1 << 10,

  // [dcl.type.cv]
  SP_CONST = 1 << 11,
  SP_VOLATILE = 1 << 12,

  // [dcl.type.simple]
  SP_CHAR = 1 << 13,
  SP_CHAR16_T = 1 << 14,
  SP_CHAR32_T = 1 << 15,
  SP_WCHAR_T = 1 << 16,
  SP_BOOL = 1 << 17,
  SP_SHORT = 1 << 18,
  SP_LONG_1 = 1 << 19,
  SP_LONG_2 = 1 << 20,  // two bits for long
  SP_INT = 1 << 21,
  SP_SIGNED = 1 << 22,
  SP_UNSIGNED = 1 << 23,
  SP_FLOAT = 1 << 24,
  SP_DOUBLE = 1 << 25,
  SP_VOID = 1 << 26,

  // [dcl.spec.auto]
  SP_AUTO = 1 << 27,
};

constexpr SpecifiersBitField MAX_SPECIFIERS_BIT_FIELD = 27;

constexpr SpecifiersBitField SP_SIMPLE_TYPE_SPECIFIERS =
    SP_CHAR | SP_CHAR16_T | SP_CHAR32_T | SP_WCHAR_T | SP_BOOL | SP_SHORT |
    SP_LONG_1 | SP_LONG_2 | SP_INT | SP_SIGNED | SP_UNSIGNED | SP_FLOAT |
    SP_DOUBLE | SP_VOID;

constexpr SpecifiersBitField SP_STORAGE_CLASS_SPECIFIERS =
    SP_REGISTER | SP_STATIC | SP_THREAD_LOCAL | SP_EXTERN | SP_MUTABLE;

inline string SpecifiersToString(SpecifiersBitField specifiers) {
  ostringstream oss;
  bool first = true;

  auto output_specifier = [&](SpecifiersBitField specifier, const char* str) {
    if (specifiers & specifier) {
      if (first)
        first = false;
      else
        oss << "|";

      oss << str;
    }
  };

#define CASE_OUTPUT_SPECIFIER(x) \
  case x:                        \
    output_specifier(x, #x);     \
    break;
  for (int i = 0; i <= MAX_SPECIFIERS_BIT_FIELD; i++) {
    switch (1 << i) {
      CASE_OUTPUT_SPECIFIER(SP_REGISTER)
      CASE_OUTPUT_SPECIFIER(SP_STATIC)
      CASE_OUTPUT_SPECIFIER(SP_THREAD_LOCAL)
      CASE_OUTPUT_SPECIFIER(SP_EXTERN)
      CASE_OUTPUT_SPECIFIER(SP_MUTABLE)
      CASE_OUTPUT_SPECIFIER(SP_INLINE)
      CASE_OUTPUT_SPECIFIER(SP_VIRTUAL)
      CASE_OUTPUT_SPECIFIER(SP_EXPLICIT)
      CASE_OUTPUT_SPECIFIER(SP_TYPEDEF)
      CASE_OUTPUT_SPECIFIER(SP_FRIEND)
      CASE_OUTPUT_SPECIFIER(SP_CONSTEXPR)
      CASE_OUTPUT_SPECIFIER(SP_CONST)
      CASE_OUTPUT_SPECIFIER(SP_VOLATILE)
      CASE_OUTPUT_SPECIFIER(SP_CHAR)
      CASE_OUTPUT_SPECIFIER(SP_CHAR16_T)
      CASE_OUTPUT_SPECIFIER(SP_CHAR32_T)
      CASE_OUTPUT_SPECIFIER(SP_WCHAR_T)
      CASE_OUTPUT_SPECIFIER(SP_BOOL)
      CASE_OUTPUT_SPECIFIER(SP_SHORT)
      CASE_OUTPUT_SPECIFIER(SP_LONG_1)
      CASE_OUTPUT_SPECIFIER(SP_LONG_2)
      CASE_OUTPUT_SPECIFIER(SP_INT)
      CASE_OUTPUT_SPECIFIER(SP_SIGNED)
      CASE_OUTPUT_SPECIFIER(SP_UNSIGNED)
      CASE_OUTPUT_SPECIFIER(SP_FLOAT)
      CASE_OUTPUT_SPECIFIER(SP_DOUBLE)
      CASE_OUTPUT_SPECIFIER(SP_VOID)
      CASE_OUTPUT_SPECIFIER(SP_AUTO)
      default:
        throw logic_error("internal error: unknown specifier");
    }
  }
#undef CASE_OUTPUT_SPECIFIER

  return oss.str();
}

enum EPtrOperator {
  PO_CONST_VOLATILE_POINTER,
  PO_CONST_POINTER,
  PO_VOLATILE_POINTER,
  PO_POINTER,
  PO_LVALUE_REFERENCE,
  PO_RVALUE_REFERENCE,
};

struct PtrOperator : Pooled {
  EPtrOperator po;

  PtrOperator(EPtrOperator po) : po(po) {}
};

struct DeclaratorSuffix : Pooled {
  virtual ~DeclaratorSuffix() = default;

  virtual bool is_function_declarator() { return false; }
};

struct ArrayDeclaratorSuffix : DeclaratorSuffix {
  RawExpression* expression;

  ArrayDeclaratorSuffix(RawExpression* expression) : expression(expression) {}
};

struct FunctionDeclaratorSuffix : DeclaratorSuffix {
  ParameterDeclarationClause* parameter_declarartion_clause;

  FunctionDeclaratorSuffix(
      ParameterDeclarationClause* parameter_declarartion_clause)
      : parameter_declarartion_clause(parameter_declarartion_clause) {}

  virtual bool is_function_declarator() { return true; }
};

struct Declarator : Pooled {
  vector<PtrOperator*> ptr_operators;
  Declarator* declarator = nullptr;
  QualifiedName* name = nullptr;
  set<Entity*> entities;

  vector<DeclaratorSuffix*> suffixes;

  void push_ptr_operator(PtrOperator* ptr_operator) {
    ptr_operators.push_back(ptr_operator);
  }
  void push_inner_declarator(Declarator* declarator) {
    this->declarator = declarator;
  }
  void push_declarator_id(QualifiedName* name) { this->name = name; }
  void push_entities(set<Entity*> entities) { this->entities = entities; }
  void push_array(RawExpression* expression) {
    suffixes.push_back(new ArrayDeclaratorSuffix(expression));
  }
  void push_parameter_declaration_clause(
      ParameterDeclarationClause* parameter_declaration_clause) {
    suffixes.push_back(
        new FunctionDeclaratorSuffix(parameter_declaration_clause));
  }

  bool just_name() {
    return ptr_operators.empty() && suffixes.empty() &&
           (name || declarator->just_name());
  }

  bool viable_function_declarator() {
    return (name || declarator->just_name()) && suffixes.size() == 1 &&
           suffixes[0]->is_function_declarator();
  }
};

struct Declaration : Pooled {};

struct DeclSpecifier : Pooled {
  virtual bool is_non_cv_type_specifier() = 0;

  virtual ~DeclSpecifier() = default;
};

struct KeywordDeclSpecifier : DeclSpecifier {
  ETokenType keyword;

  KeywordDeclSpecifier(ETokenType keyword) : keyword(keyword){};

  virtual bool is_non_cv_type_specifier() {
    switch (keyword) {
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
        return true;

      default:
        return false;
    }
  }
};

struct TypeDeclSpecifier : DeclSpecifier {
  Type* type;

  TypeDeclSpecifier(Type* type) : type(type) {}

  virtual bool is_non_cv_type_specifier() { return true; }
};

struct DeclTypeSpecifier : DeclSpecifier {
  RawExpression* expression;

  DeclTypeSpecifier(RawExpression* expression) : expression(expression) {}

  virtual bool is_non_cv_type_specifier() { return true; }
};

inline SpecifiersBitField KeywordToSpecifier(ETokenType specifier_keyword) {
  switch (specifier_keyword) {
    case KW_LONG:
      throw logic_error("long needs special handling");
    case KW_REGISTER:
      return SP_REGISTER;
    case KW_STATIC:
      return SP_STATIC;
    case KW_THREAD_LOCAL:
      return SP_THREAD_LOCAL;
    case KW_EXTERN:
      return SP_EXTERN;
    case KW_INLINE:
      return SP_INLINE;
    case KW_TYPEDEF:
      return SP_TYPEDEF;
    case KW_CONSTEXPR:
      return SP_CONSTEXPR;
    case KW_CHAR:
      return SP_CHAR;
    case KW_CHAR16_T:
      return SP_CHAR16_T;
    case KW_CHAR32_T:
      return SP_CHAR32_T;
    case KW_WCHAR_T:
      return SP_WCHAR_T;
    case KW_BOOL:
      return SP_BOOL;
    case KW_SHORT:
      return SP_SHORT;
    case KW_INT:
      return SP_INT;
    case KW_SIGNED:
      return SP_SIGNED;
    case KW_UNSIGNED:
      return SP_UNSIGNED;
    case KW_FLOAT:
      return SP_FLOAT;
    case KW_DOUBLE:
      return SP_DOUBLE;
    case KW_VOID:
      return SP_VOID;
    case KW_CONST:
      return SP_CONST;
    case KW_VOLATILE:
      return SP_VOLATILE;
    default:
      throw logic_error("unknown specifier keyword");
  }
}

inline EFundamentalType SimpleTypeSpecifiersToFundamentalType(
    SpecifiersBitField simple_type_specifiers) {
  switch (simple_type_specifiers) {
    case SP_CHAR:
      return FT_CHAR;
    case SP_UNSIGNED | SP_CHAR:
      return FT_UNSIGNED_CHAR;
    case SP_SIGNED | SP_CHAR:
      return FT_SIGNED_CHAR;
    case SP_CHAR16_T:
      return FT_CHAR16_T;
    case SP_CHAR32_T:
      return FT_CHAR32_T;
    case SP_BOOL:
      return FT_BOOL;
    case SP_UNSIGNED:
      return FT_UNSIGNED_INT;
    case SP_UNSIGNED | SP_INT:
      return FT_UNSIGNED_INT;
    case SP_SIGNED:
      return FT_INT;
    case SP_SIGNED | SP_INT:
      return FT_INT;
    case SP_INT:
      return FT_INT;
    case SP_UNSIGNED | SP_SHORT | SP_INT:
      return FT_UNSIGNED_SHORT_INT;
    case SP_UNSIGNED | SP_SHORT:
      return FT_UNSIGNED_SHORT_INT;
    case SP_UNSIGNED | SP_LONG_1 | SP_INT:
      return FT_UNSIGNED_LONG_INT;
    case SP_UNSIGNED | SP_LONG_1:
      return FT_UNSIGNED_LONG_INT;
    case SP_UNSIGNED | SP_LONG_2 | SP_INT:
      return FT_UNSIGNED_LONG_LONG_INT;
    case SP_UNSIGNED | SP_LONG_2:
      return FT_UNSIGNED_LONG_LONG_INT;
    case SP_SIGNED | SP_LONG_1 | SP_INT:
      return FT_LONG_INT;
    case SP_SIGNED | SP_LONG_1:
      return FT_LONG_INT;
    case SP_SIGNED | SP_LONG_2 | SP_INT:
      return FT_LONG_LONG_INT;
    case SP_SIGNED | SP_LONG_2:
      return FT_LONG_LONG_INT;
    case SP_LONG_2 | SP_INT:
      return FT_LONG_LONG_INT;
    case SP_LONG_2:
      return FT_LONG_LONG_INT;
    case SP_LONG_1 | SP_INT:
      return FT_LONG_INT;
    case SP_LONG_1:
      return FT_LONG_INT;
    case SP_SIGNED | SP_SHORT | SP_INT:
      return FT_SHORT_INT;
    case SP_SIGNED | SP_SHORT:
      return FT_SHORT_INT;
    case SP_SHORT | SP_INT:
      return FT_SHORT_INT;
    case SP_SHORT:
      return FT_SHORT_INT;
    case SP_WCHAR_T:
      return FT_WCHAR_T;
    case SP_FLOAT:
      return FT_FLOAT;
    case SP_DOUBLE:
      return FT_DOUBLE;
    case SP_LONG_1 | SP_DOUBLE:
      return FT_LONG_DOUBLE;
    case SP_VOID:
      return FT_VOID;
    default:
      throw logic_error("invalid simple type specifier combination");
  }
}

struct DeclSpecifierSeq {
  DeclTypeSpecifier* decltype_specifier = nullptr;
  TypeDeclSpecifier* typedecl_specifier = nullptr;

  SpecifiersBitField specifiers = SpecifiersBitField(0);

  void push_decl_specifier(DeclSpecifier* specifier) {
    if (auto keyword_specifier =
            dynamic_cast<KeywordDeclSpecifier*>(specifier)) {
      ETokenType specifier_keyword = keyword_specifier->keyword;

      if (specifier_keyword == KW_LONG) {
        if (specifiers & SP_LONG_2)
          throw logic_error("too many long specifiers");

        if (specifiers & SP_LONG_1) {
          specifiers ^= SP_LONG_1;
          specifiers |= SP_LONG_2;
        } else
          specifiers |= SP_LONG_1;

      } else {
        auto specifier = KeywordToSpecifier(specifier_keyword);

        if (specifiers & specifier) throw logic_error("duplicate specifier");

        specifiers |= specifier;
      }
    } else if (auto typedecl_specifier_in =
                   dynamic_cast<TypeDeclSpecifier*>(specifier)) {
      if (typedecl_specifier)
        throw logic_error("two type-specifiers in decl-specifier-seq");

      typedecl_specifier = typedecl_specifier_in;
    } else if (auto decltype_specifier_in =
                   dynamic_cast<DeclTypeSpecifier*>(specifier)) {
      if (decltype_specifier)
        throw logic_error("two decltype-specifiers in decl-specifier-seq");

      decltype_specifier = decltype_specifier_in;
    } else
      throw logic_error("internal error, unknown decl-specifier");
  }

  bool empty() {
    return specifiers == 0 && !decltype_specifier && !typedecl_specifier;
  }
};

struct EmptyDeclaration : Declaration {};

// struct SimpleDeclaration : Declaration
//{
//	DeclSpecifierSeq* decl_specifier_seq;
//	InitDeclaratorList* init_declarator_list;
//
//	SimpleDeclaration(DeclSpecifierSeq* decl_specifier_seq,
//InitDeclaratorList* init_declarator_list)
//		: decl_specifier_seq(decl_specifier_seq)
//		, init_declarator_list(init_declarator_list)
//	{
//
//	};
//};

struct FunctionDefinition : Declaration {
  DeclSpecifierSeq* decl_specifier_seq;
  Declarator* declarator;
  Statement* statement;

  FunctionDefinition(DeclSpecifierSeq* decl_specifier_seq,
                     Declarator* declarator, Statement* statement)
      : decl_specifier_seq(decl_specifier_seq),
        declarator(declarator),
        statement(statement) {}
};

struct AliasDeclaration : Declaration {
  string name;
  Type* type;

  AliasDeclaration(const string& name, Type* type) : name(name), type(type) {}
};

struct StaticAssertDeclaration : Declaration {
  RawExpression* expression;
  string message;

  StaticAssertDeclaration(RawExpression* expression, const string& message)
      : expression(expression), message(message) {}
};

struct NamespaceAliasDefinition : Declaration {
  string name;
  Namespace* ns;

  NamespaceAliasDefinition(const string& name, Namespace* ns)
      : name(name), ns(ns) {}
};

struct UsingDeclaration : Declaration {
  QualifiedName* name;

  UsingDeclaration(QualifiedName* name) : name(name) {}
};

struct UsingDirective : Declaration {
  Namespace* ns;

  UsingDirective(Namespace* ns) : ns(ns) {}
};

struct Condition {};
struct ConditionDeclaration : Condition {
  DeclSpecifierSeq* decl_specifier_seq;
  Declarator* declarator;
  RawExpression* expression;

  ConditionDeclaration(DeclSpecifierSeq* decl_specifier_seq,
                       Declarator* declarator, RawExpression* expression)
      : decl_specifier_seq(decl_specifier_seq),
        declarator(declarator),
        expression(expression) {}
};

struct ExpressionCondition : Condition {
  RawExpression* expression;

  ExpressionCondition(RawExpression* expression) : expression(expression) {}
};

struct Initializer : Pooled {
  virtual Expression* create_initializer_expression(SymbolTable& /*symtab*/,
                                                    Type* /*destination*/) {
    throw logic_error("not yet implemented");
  }
};

struct DirectInitializer : Initializer {
  ExpressionList* expression_list;

  DirectInitializer(ExpressionList* expression_list)
      : expression_list(expression_list) {}

  void write(ostream& out) const {
    out << "DirectInitializer (" << expression_list << ")";
  }
};

struct CopyInitializer : Initializer {
  RawExpression* expression;

  Expression* create_initializer_expression(SymbolTable& symtab,
                                            Type* destination);

  CopyInitializer(RawExpression* expression) : expression(expression) {}

  void write(ostream& out) const {
    out << "CopyInitializer (" << expression << ")";
  }
};

struct DefaultInitializer : Initializer {
  Expression* create_initializer_expression(SymbolTable&, Type*) {
    return nullptr;
  }

  void write(ostream& out) const { out << "DefaultInitializer"; }
};

struct InitDeclarator {
  Declarator* declarator;
  Initializer* initializer;

  InitDeclarator(Declarator* declarator, Initializer* initializer)
      : declarator(declarator), initializer(initializer) {}

  Declarator* viable_function_declarator() {
    if (initializer == nullptr)
      return declarator;
    else
      return nullptr;
  }
};

struct InitDeclaratorList {
  vector<InitDeclarator*> init_declarators;

  void push_init_declarator(InitDeclarator* init_declarator) {
    init_declarators.push_back(init_declarator);
  }

  Declarator* viable_function_declarator() {
    if (init_declarators.size() != 1) return nullptr;

    return init_declarators[0]->viable_function_declarator();
  }
};

struct ParameterDeclaration {
  DeclSpecifierSeq* decl_specifier_seq;
  Declarator* declarator;
  RawExpression* default_argument = nullptr;

  ParameterDeclaration(DeclSpecifierSeq* decl_specifier_seq,
                       Declarator* declarator)
      : decl_specifier_seq(decl_specifier_seq), declarator(declarator) {}

  ParameterDeclaration(DeclSpecifierSeq* decl_specifier_seq,
                       Declarator* declarator, RawExpression* expression)
      : decl_specifier_seq(decl_specifier_seq),
        declarator(declarator),
        default_argument(expression) {}
};

struct ParameterDeclarationClause {
  vector<ParameterDeclaration*> parameters;
  bool has_ellipse = false;

  void set_ellipse() { has_ellipse = true; }

  void push_parameter_declaration(ParameterDeclaration* parameter_declaration) {
    parameters.push_back(parameter_declaration);
  }
};
