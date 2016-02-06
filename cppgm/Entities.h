#pragma once

#include "Declarations.h"

#include "Types.h"

struct TranslationUnit;
struct LinkedFunction;
struct LinkedVariable;

enum EStorageDuration { SD_STATIC, SD_THREAD_LOCAL, SD_AUTOMATIC, SD_DYNAMIC };

enum ELinkage { LK_NONE, LK_EXTERNAL, LK_INTERNAL };

inline const char* ELinkageToString(ELinkage e) {
  switch (e) {
    case LK_NONE:
      return "LK_NONE";
    case LK_EXTERNAL:
      return "LK_EXTERNAL";
    case LK_INTERNAL:
      return "LK_INTERNAL";
    default:
      throw logic_error("internal error: unknown linkage");
  }
}

typedef unsigned int EntityCategory;
constexpr EntityCategory EC_FUNCTION = 1 << 0;
constexpr EntityCategory EC_OBJECT = 1 << 1;
constexpr EntityCategory EC_REFERENCE = 1 << 2;
constexpr EntityCategory EC_TYPEDEF = 1 << 3;
constexpr EntityCategory EC_NAMESPACE = 1 << 4;

constexpr EntityCategory EC_ANY =
    EC_FUNCTION | EC_OBJECT | EC_REFERENCE | EC_TYPEDEF | EC_NAMESPACE;

struct Entity : Pooled {
  string name;
  Namespace* enclosing_namespace = nullptr;
  const LinkerSymbol* linker_symbol = nullptr;
  ELinkage linkage = LK_NONE;

  Entity(string name, Namespace* enclosing_namespace)
      : name(name), enclosing_namespace(enclosing_namespace) {}

  virtual Entity* redeclare(SpecifiersBitField specifiers, Type* type,
                            Namespace* ns) = 0;

  virtual void set_initializer(SymbolTable&, Initializer* initializer) {
    if (initializer) throw logic_error("invalid entity for initializer");
  }

  virtual bool has_category(EntityCategory entity_category) = 0;

  virtual bool is_defined() {
    throw logic_error("is_defined on invalid entity");
  }
  virtual size_t image_size() {
    throw logic_error("image_size on invalid entity");
  }
  virtual size_t image_alignment() {
    throw logic_error("image_size on invalid entity");
  }

  virtual void relocate() {}

  virtual void set_function_definition() {
    throw logic_error("invalid entity for function definition");
  }

  virtual bool equivalent(Entity* that) { return this == that; }
  virtual bool declaration_type_match(Type*) { return true; }
  virtual bool can_coexist(Entity* that) { return this == that; }
  virtual bool can_coexist(Type*) { return false; }

  virtual ~Entity() = default;
};

struct Function : Entity {
  SpecifiersBitField specifiers;
  FunctionType* type;
  LinkedFunction* linked_function = nullptr;
  size_t image_offset;
  bool defined = false;

  Function(const string& name, SpecifiersBitField specifiers,
           FunctionType* type, Namespace* ns);

  bool has_category(EntityCategory entity_category) {
    return entity_category & EC_FUNCTION;
  }

  virtual Entity* redeclare(SpecifiersBitField, Type* type_in, Namespace*) {
    if (!type->equal(type_in))
      throw logic_error(name + " redeclared different type");

    return this;
  }

  virtual void dump(ostream& out, bool = false) {
    out << "function " << name << " ";
    type->write(out);
    out << endl;
  }

  virtual void write(ostream& out) const {
    out << "function " << name << " " << SpecifiersToString(specifiers) << " "
        << type << " defined=" << defined;
  }

  virtual void notify_offset(size_t image_offset) {
    this->image_offset = image_offset;
  }

  virtual void set_function_definition() {
    if (defined) throw logic_error("function " + name + " already defined");
    defined = true;
  }

  virtual bool is_defined() { return defined; }

  virtual size_t image_size() { return 4; }
  virtual size_t image_alignment() { return 4; }

  virtual bool can_coexist(Entity* that) {
    if (Function* that_function = dynamic_cast<Function*>(that)) {
      return !type->signature_equivilant(that_function->type);
    } else
      return false;
  }

  virtual bool can_coexist(Type* that) {
    if (FunctionType* that_function_type = dynamic_cast<FunctionType*>(that)) {
      return !type->signature_equivilant(that_function_type);
    } else
      return false;
  }

  virtual bool declaration_type_match(Type* that) {
    return type->signature_equivilant(that);
  }
};

// struct FunctionOverloadSetDepr : Entity
//{
//	vector<Function*> functions;
//
//	FunctionOverloadSet(const string& name, Namespace* ns);
//
//	bool has_category(EntityCategory entity_category)
//	{
//		return entity_category & EC_FUNCTION;
//	}
//
//	Entity* redeclare(SpecifiersBitField specifiers, Type* type_in,
//Namespace* ns);
//};

struct Variable : Entity {
  SpecifiersBitField specifiers;
  Type* type;
  Initializer* initializer = nullptr;
  Expression* initializer_expression = nullptr;
  LinkedVariable* linked_variable = nullptr;
  size_t image_offset;

  virtual void write(ostream& out) const;

  EStorageDuration storage_duration = SD_STATIC;

  static void check_specifiers(SpecifiersBitField specifiers);

  Variable(const string& name, SpecifiersBitField specifiers, Type* type,
           Namespace* ns);

  Variable(EStorageDuration, Type* type, Expression* initializer_expression);

  bool has_category(EntityCategory entity_category) {
    if (type->is_reference())
      return entity_category & EC_REFERENCE;
    else
      return entity_category & EC_OBJECT;
  }

  bool is_constant() const;

  void set_initializer(SymbolTable& symtab, Initializer* initializer_in);

  Entity* redeclare(SpecifiersBitField specifiers_in, Type* type_in,
                    Namespace* ns);

  virtual void dump(ostream& out, bool = false) {
    out << "variable " << name << " ";
    type->write(out);
    out << endl;
  }

  virtual bool is_defined() { return initializer || initializer_expression; }

  virtual size_t image_size() {
    if (type->is_reference())
      return 8;
    else
      return type->size();
  }

  virtual size_t image_alignment() {
    if (type->is_reference())
      return 8;
    else
      return type->alignment();
  }

  virtual void notify_offset(size_t image_offset) {
    this->image_offset = image_offset;
  }

  virtual void relocate();

  virtual vector<byte> get_data();
};

struct TypeDef : Entity {
  Type* type;

  TypeDef(SpecifiersBitField specifiers, const string& name, Type* type,
          Namespace* ns);

  bool has_category(EntityCategory entity_category) {
    return entity_category & EC_TYPEDEF;
  }

  virtual bool equivalent(Entity* that) {
    if (TypeDef* that_tdef = dynamic_cast<TypeDef*>(that))
      return type->equal(that_tdef->type);
    else
      return false;
  }

  Entity* redeclare(SpecifiersBitField specifiers_in, Type* type_in,
                    Namespace* ns);
};

struct Namespace : Entity {
  bool inline_namespace;
  TranslationUnit* tu;

  vector<Namespace*> namespaces;
  vector<Function*> functions;
  vector<Variable*> variables;
  vector<TypeDef*> typedefs;

  multimap<string, Entity*> members;
  bool is_member(Entity* entity) {
    for (auto member : members)
      if (entity == member.second) return true;
    return false;
  }

  set<Namespace*> ancestor_namespaces;

  set<Namespace*> using_directives_direct;
  set<Namespace*> using_directives_closure;
  set<Namespace*> reverse_using_directives_closure;

  set<Namespace*> inline_namespace_set;

  multimap<string, Entity*> scope;

  Namespace(TranslationUnit* tu, ELinkage linkage, const string& name,
            bool inline_namespace, Namespace* enclosing_namespace)
      : Entity(name, enclosing_namespace),
        inline_namespace(inline_namespace),
        tu(tu) {
    this->linkage = linkage;

    inline_namespace_set.insert(this);
    using_directives_closure.insert(this);
    reverse_using_directives_closure.insert(this);
    ancestor_namespaces.insert(this);
  }

  void add_member(Entity* entity) {
    members.insert(make_pair(entity->name, entity));
    scope.insert(make_pair(entity->name, entity));
  }

  bool encloses(Namespace* that) {
    if (that == this) return true;

    if (!that->enclosing_namespace) return false;

    return encloses(that->enclosing_namespace);
  }

  void set_global() { linker_symbol = LinkerSymbol::global_linker_symbol(); }

  void add_namespace(Namespace* ns) {
    if (ns->name.size() > 0 && linkage == LK_EXTERNAL) {
      ns->linker_symbol = linker_symbol->with_name(ns->name);
    }

    namespaces.push_back(ns);

    if (!ns->name.empty()) {
      if (scope.count(ns->name))
        throw logic_error(ns->name + " already exists");

      add_member(ns);
    }

    if (ns->inline_namespace || ns->name.empty()) add_using_directive(ns);

    ns->add_inline_namespace(ns);

    for (Namespace* ns2 = ns; ns2 != nullptr; ns2 = ns2->enclosing_namespace)
      ns->ancestor_namespaces.insert(ns2);
  }

  void add_using_directive(Namespace* ns) {
    //		if (ns == this)
    //			throw logic_error("adding using directive to self?");

    using_directives_direct.insert(ns);

    set<Namespace*> r = reverse_using_directives_closure;
    set<Namespace*> a = ns->using_directives_closure;

    for (Namespace* behind : r)
      for (Namespace* ahead : a) {
        behind->using_directives_closure.insert(ahead);
        ahead->reverse_using_directives_closure.insert(behind);
      }

    //		cout << "DEBUG: added using directive" << endl;
    //
    //		Namespace* ns2 = this;
    //
    //		while (ns2->enclosing_namespace)
    //			ns2 = ns2->enclosing_namespace;
    //
    //		ns2->dump(cout, true);
  }

  bool has_category(EntityCategory entity_category) {
    return entity_category & EC_NAMESPACE;
  }

  void set_namespace_alias(const string& name, Namespace* ns);

  Entity* redeclare(SpecifiersBitField, Type*, Namespace*) {
    throw logic_error(name + " already declared as namespace");
  }

  void add_inline_namespace(Namespace* ns) {
    inline_namespace_set.insert(ns);

    if (inline_namespace) enclosing_namespace->add_inline_namespace(ns);
  }

  Namespace* get_member_namespace(const string& namespace_name) {
    for (Namespace* ns : namespaces)
      if (ns->name == namespace_name) return ns;

    return nullptr;
  }

  set<Entity*> simple_lookup_entity(const string& entity_name, bool direct_only,
                                    EntityCategory entity_category) {
    set<Entity*> result;

    auto& to_search = (direct_only ? members : scope);

    auto range = to_search.equal_range(entity_name);

    for (auto it = range.first; it != range.second; ++it) {
      Entity* entity = it->second;

      if (entity->has_category(entity_category)) result.insert(entity);
    }

    return result;
  }

  set<Entity*> direct_lookup_set(const string& name, bool direct_only,
                                 EntityCategory entity_category) {
    set<Entity*> result;

    for (Namespace* ns : inline_namespace_set) {
      auto entities =
          ns->simple_lookup_entity(name, direct_only, entity_category);

      result.insert(entities.begin(), entities.end());
    }

    return result;
  }

  set<Entity*> namespace_qualified_lookup_set(const string& name,
                                              bool direct_only,
                                              EntityCategory entity_category,
                                              set<Namespace*>& done) {
    if (done.count(this))
      return {};
    else
      done.insert(this);

    auto results = direct_lookup_set(name, direct_only, entity_category);

    if (!results.empty() || direct_only) return results;

    for (Namespace* ns : inline_namespace_set)
      for (Namespace* ns2 : ns->using_directives_direct) {
        auto entities = ns2->namespace_qualified_lookup_set(
            name, direct_only, entity_category, done);

        results.insert(entities.begin(), entities.end());
      }

    return results;
  }

  set<Entity*> qualified_lookup_entity(
      const string& name, bool direct_only,
      EntityCategory entity_category = EC_ANY) {
    set<Namespace*> done;

    auto results = namespace_qualified_lookup_set(name, direct_only,
                                                  entity_category, done);

    if (results.empty()) throw logic_error(name + " not found");

    return results;
  }

  Namespace* qualified_lookup_namespace(const string& name) {
    auto results = qualified_lookup_entity(name, false, EC_NAMESPACE);

    if (results.empty()) throw logic_error(name + " not found");

    if (results.size() > 1) throw logic_error(name + " ambiguous");

    return dynamic_cast<Namespace*>(*results.begin());
  }

  Namespace* find_common_ancestor(Namespace* ns);

  set<Entity*> unqualified_lookup_entity(
      const string& name, EntityCategory entity_category,
      map<Namespace*, set<Namespace*>>& search_namespaces);

  Entity* declare_name(SpecifiersBitField specifiers, const string& name,
                       Type* type);

  void add_using_declaration(const string& name, set<Entity*> entities) {
    auto existing_entities = scope.equal_range(name);

    for (Entity* entity : entities) {
      bool save = true;
      for (auto it = existing_entities.first; it != existing_entities.second;
           ++it) {
        Entity* existing_entity = it->second;

        if (entity->equivalent(existing_entity))
          save = false;
        else if (!entity->can_coexist(existing_entity))
          throw logic_error(name + " can't coexist in scope");
      }

      if (save) scope.insert(make_pair(name, entity));
    }
  }

  void dump_namespace_set(const string& name, ostream& out,
                          const set<Namespace*>& s) {
    out << name << " = { ";
    for (Namespace* n : s)
      if (n->name.empty())
        out << (void*)(n) << ", ";
      else
        out << n->name << ", ";

    out << "}" << endl;
  }

  void dump(ostream& out, bool extra = false) {
    if (name.empty())
      out << "start unnamed namespace" << endl;
    else
      out << "start namespace " << name << endl;

    if (inline_namespace) out << "inline namespace" << endl;

    if (extra) {
      dump_namespace_set("ancestor_namespaces", out, ancestor_namespaces);
      dump_namespace_set("using_directives_direct", out,
                         using_directives_direct);
      dump_namespace_set("using_directives_closure", out,
                         using_directives_closure);
      dump_namespace_set("reverse_using_directives_closure", out,
                         reverse_using_directives_closure);
      dump_namespace_set("inline_namespace_set", out, inline_namespace_set);
    }

    for (auto var : variables) var->dump(out);

    for (auto func : functions) func->dump(out);

    for (auto enclosed_namespace : namespaces)
      enclosed_namespace->dump(out, extra);

    out << "end namespace" << endl;
  }
};
