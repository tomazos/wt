#pragma once

#include "Pooled.h"
#include "Types.h"
#include "Entities.h"

struct StringLiteral;

struct TranslationUnit : Pooled {
  string filename;
  Namespace* global_namespace;

  vector<Entity*> entities;

  TranslationUnit(const string& filename)
      : filename(filename),
        global_namespace(new Namespace(this, LK_EXTERNAL, "", false, nullptr)) {
    global_namespace->set_global();
  }

  void dump(ostream& out) {
    out << "start translation unit " << filename << endl;

    global_namespace->dump(out);

    out << "end translation unit" << endl;
  }
};

struct SymbolTable {
  vector<TranslationUnit*> translation_units;
  vector<StringLiteral*> string_literals;
  vector<Variable*> temporary_variables;

  Variable* create_temporary(EStorageDuration, Type*,
                             Expression* initializer_expression);

  TranslationUnit* current_translation_unit = nullptr;
  Namespace* current_namespace = nullptr;

  void enter_translation_unit(const string& filename);
  void exit_translation_unit();

  void enter_namespace_definition(const string& namespace_name,
                                  bool inline_namespace);
  void exit_namespace_definition();

  Entity* process_simple_declarator(DeclSpecifierSeq*, Declarator*);

  void process_declaration(EmptyDeclaration*);
  void process_declaration(FunctionDefinition*);
  void process_declaration(AliasDeclaration*);
  void process_declaration(StaticAssertDeclaration*);
  void process_declaration(NamespaceAliasDefinition*);
  void process_declaration(UsingDirective*);
  void process_declaration(UsingDeclaration*);

  Namespace* lookup_namespace(QualifiedName*);
  Type* lookup_type(QualifiedName*);

  set<Entity*> current_lookup_entity(QualifiedName* name, bool direct_only,
                                     EntityCategory entity_category);
  set<Entity*> current_lookup_entity(const string& name,
                                     EntityCategory entity_category);

  Type* make_type_id(DeclSpecifierSeq*, Declarator*);

  Type* deduce_type(DeclTypeSpecifier*);

  struct DeclaratorInfo {
    Type* type = nullptr;
    QualifiedName* name = nullptr;
    set<Entity*> entities;
  };

  Type* analyze_decl_specifier_seq(DeclSpecifierSeq*);
  DeclaratorInfo analyze_declarator(Type* base_type, Declarator*,
                                    bool last_ref_in = false);

  size_t evaluate_array_bound(RawExpression* expr);

  Entity* declare_name(SpecifiersBitField specifiers, QualifiedName* name,
                       Type* type, Entity* entity);

  void link(vector<byte>& program_image);

  void dump(ostream& out);
  void mock_nsinit(ostream& out);
};

struct ScopeChanger {
  SymbolTable& symtab;
  Namespace* old_namespace;

  ScopeChanger(SymbolTable& symtab) : symtab(symtab) {
    old_namespace = symtab.current_namespace;
  }

  void reset() { symtab.current_namespace = old_namespace; }

  set<Entity*> enter_qualified_declarator_id(QualifiedName* name) {
    set<Entity*> entities = symtab.current_lookup_entity(name, true, EC_ANY);

    if (entities.empty()) throw logic_error("qualified name not found");

    Namespace* enclosing_namespace = (*entities.begin())->enclosing_namespace;

    if (!symtab.current_namespace->encloses(enclosing_namespace))
      throw logic_error("qualified name not from enclosed namespace");

    symtab.current_namespace = enclosing_namespace;

    return entities;
  }

  ~ScopeChanger() { symtab.current_namespace = old_namespace; }
};
