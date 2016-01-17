#include "std.pch"

#include "SymbolTable.h"

#include "ToString.h"
#include "AnnotatedExpressions.h"

void SymbolTable::enter_translation_unit(const string& filename)
{
	if (current_translation_unit)
		throw logic_error("internal error, enter_translation_unit");

	TranslationUnit* translation_unit = new TranslationUnit(filename);

	translation_units.push_back(translation_unit);

	current_translation_unit = translation_unit;

	current_namespace = translation_unit->global_namespace;
}

void SymbolTable::exit_translation_unit()
{
	current_translation_unit = nullptr;
	current_namespace = nullptr;
}

void SymbolTable::enter_namespace_definition(const string& namespace_name, bool inline_namespace)
{
	Namespace* existing_namespace = current_namespace->get_member_namespace(namespace_name);

	if (!existing_namespace)
	{
		ELinkage linkage = current_namespace->linkage;

		if (namespace_name.empty())
			linkage = LK_INTERNAL;

		Namespace* new_namespace = new Namespace(current_translation_unit, linkage, namespace_name, inline_namespace, current_namespace);

		current_namespace->add_namespace(new_namespace);

		current_namespace = new_namespace;
	}
	else
	{
		if (!existing_namespace->inline_namespace && inline_namespace)
			throw logic_error("extension namespace cannot be inline");

		current_namespace = existing_namespace;
	}
}

void SymbolTable::exit_namespace_definition()
{
	current_namespace = current_namespace->enclosing_namespace;
}

void SymbolTable::process_declaration(EmptyDeclaration*)
{

}

Type* SymbolTable::analyze_decl_specifier_seq(DeclSpecifierSeq* decl_specifier_seq)
{
	SpecifiersBitField specifiers = decl_specifier_seq->specifiers;

	Type* base_type = nullptr;

	if (decl_specifier_seq->typedecl_specifier)
	{
		base_type = decl_specifier_seq->typedecl_specifier->type;

		if (!base_type)
			throw logic_error("internal error, typedecl_specifier with no type");

		if (specifiers & SP_CONST)
			base_type = base_type->const_qualify();

		if (specifiers & SP_VOLATILE)
			base_type = base_type->volatile_qualify();
	}

	if (decl_specifier_seq->decltype_specifier)
	{
		if (base_type)
			throw logic_error("both decltype and type specifier in decl-specifier-seq");

		base_type = deduce_type(decl_specifier_seq->decltype_specifier);

		if (!base_type)
			throw logic_error("unable to deduce type");

		if (specifiers & SP_CONST)
			base_type = base_type->const_qualify();

		if (specifiers & SP_VOLATILE)
			base_type = base_type->volatile_qualify();

	}

	SpecifiersBitField simple_type_specifiers = specifiers & SP_SIMPLE_TYPE_SPECIFIERS;

	if (simple_type_specifiers)
	{
		if (base_type)
			throw logic_error("simple type specifiers and type-specifeir in decl-specifier-seq");

		EFundamentalType fundamental_type = SimpleTypeSpecifiersToFundamentalType(simple_type_specifiers);

		base_type = new FundamentalType(fundamental_type, specifiers & SP_CONST, specifiers & SP_VOLATILE);
	}

	if (!base_type)
		throw logic_error("no type found in decl-specifier-seq");

	return base_type;
}

size_t SymbolTable::evaluate_array_bound(RawExpression* rawexpr)
{
	Expression* expr = rawexpr->annotate(*this);

//	cerr << "DEBUG: " << expr << endl;

	static Type* size_t_type = new FundamentalType(FT_UNSIGNED_LONG_INT, true, false);

	Expression* expr2 = size_t_type->convert(expr);

//	cerr << "DEBUG: " << expr2 << endl;

	if (!expr2->is_constant_expression())
		throw logic_error("array bound not constant expression");

	vector<byte> array_bound_data = expr2->get_constant_data();

	if (array_bound_data.size() != 8)
		throw logic_error("internal error: array bound unexpected size");

	size_t array_bound;

	memcpy(&array_bound, &array_bound_data[0], 8);

	return array_bound;
}

static Entity* select_entity_by_type(const set<Entity*>& entities, Type* type)
{
	for (Entity* entity : entities)
	{
		if (entity->declaration_type_match(type))
			return entity;
	}
	return nullptr;
}

SymbolTable::DeclaratorInfo SymbolTable::analyze_declarator(Type* base_type, Declarator* declarator, bool last_ref_in)
{
	bool last_ref = last_ref_in;

	for (PtrOperator* ptr_operator : declarator->ptr_operators)
	{
		switch (ptr_operator->po)
		{
		case PO_CONST_VOLATILE_POINTER:
			base_type = new PointerType(base_type, true, true);
			last_ref = false;
			break;

		case PO_CONST_POINTER:
			base_type = new PointerType(base_type, true, false);
			last_ref = false;
			break;

		case PO_VOLATILE_POINTER:
			base_type = new PointerType(base_type, false, true);
			last_ref = false;
			break;

		case PO_POINTER:
			base_type = new PointerType(base_type, false, false);
			last_ref = false;
			break;

		case PO_LVALUE_REFERENCE:
			if (last_ref)
				throw logic_error("reference to reference in declarator");
			base_type = ReferenceType::make_lvalue_reference(base_type);
			last_ref = true;
			break;

		case PO_RVALUE_REFERENCE:
			if (last_ref)
				throw logic_error("reference to reference in declarator");
			base_type = ReferenceType::make_rvalue_reference(base_type);
			last_ref = true;
			break;

		default:
			throw logic_error("internal error, unknown ptr operator");
		}
	}

	for (auto it = declarator->suffixes.rbegin(); it != declarator->suffixes.rend(); it++)
	{
		DeclaratorSuffix* declarator_suffix = *it;

		if (auto array_suffix = dynamic_cast<ArrayDeclaratorSuffix*>(declarator_suffix))
		{
			size_t array_bound = 0;

			if (array_suffix->expression)
			{
				array_bound = evaluate_array_bound(array_suffix->expression);

				if (array_bound < 1)
					throw logic_error("invalid array bound");

			}

			base_type = new ArrayType(base_type, array_bound);
			last_ref = false;
		}
		else if (auto function_suffix = dynamic_cast<FunctionDeclaratorSuffix*>(declarator_suffix))
		{
			auto parameter_declaration_clause = function_suffix->parameter_declarartion_clause;

			vector<Type*> parameter_type_list;

			for (auto parameter_declaration : parameter_declaration_clause->parameters)
			{
				Type* parameter_base_type = analyze_decl_specifier_seq(parameter_declaration->decl_specifier_seq);

				auto parameter_analysis = analyze_declarator(parameter_base_type, parameter_declaration->declarator);
				Type* parameter_type = parameter_analysis.type;
//				QualifiedName* parameter_name = parameter_analysis.second;

				parameter_type_list.push_back(parameter_type->to_function_parameter_type()->unqualify());
			}

			base_type = new FunctionType(base_type, parameter_type_list, parameter_declaration_clause->has_ellipse);
			last_ref = false;
		}
	}

	if (declarator->declarator)
		return analyze_declarator(base_type, declarator->declarator, last_ref);
	else
	{
		SymbolTable::DeclaratorInfo result;
		result.type = base_type;
		result.name = declarator->name;
		result.entities = declarator->entities;
		return result;
	}
}

Type* SymbolTable::make_type_id(DeclSpecifierSeq* decl_specifier_seq, Declarator* declarator)
{
	Type* base_type = analyze_decl_specifier_seq(decl_specifier_seq);

	auto declarator_analysis = analyze_declarator(base_type, declarator);

	return declarator_analysis.type;
}

Entity* SymbolTable::declare_name(SpecifiersBitField specifiers, QualifiedName* name, Type* type, Entity* entity)
{
	if (name->unqualified())
	{
		string id = name->get_unqualified_id();

		if (id.empty())
			throw logic_error("internal error, declare_name with empty id");

		entity = current_namespace->declare_name(specifiers, id, type);

		return entity;
	}
	else
	{
		if (!entity)
			throw logic_error("unable to find " + ToString(*name));

		entity->redeclare(specifiers, type, entity->enclosing_namespace);

		return entity;
	}
}

Entity* SymbolTable::process_simple_declarator(DeclSpecifierSeq* decl_specifier_seq, Declarator* declarator)
{
	Type* base_type = analyze_decl_specifier_seq(decl_specifier_seq);

	auto declarator_analysis = analyze_declarator(base_type, declarator);

	return declare_name(decl_specifier_seq->specifiers, declarator_analysis.name, declarator_analysis.type,
		select_entity_by_type(declarator_analysis.entities, declarator_analysis.type));
}

void SymbolTable::process_declaration(FunctionDefinition*)
{
	cout << "function-definition" << endl;
}

void SymbolTable::process_declaration(AliasDeclaration* alias_declaration)
{
	current_namespace->declare_name(SP_TYPEDEF, alias_declaration->name, alias_declaration->type);
}

void SymbolTable::process_declaration(StaticAssertDeclaration* static_assert_declaration)
{
	static auto bool_type = new FundamentalType(FT_BOOL, false, false);

	RawExpression* rawexpr = static_assert_declaration->expression;

//	cerr << "DEBUG: rawexpr = " << rawexpr << endl;

	Expression* expr1 = rawexpr->annotate(*this);

//	cerr << "DEBUG: expr1 = " << expr1 << endl;

	Expression* expr2 = bool_type->convert(expr1);

//	cerr << "DEBUG: expr2 = " << expr2 << endl;

	if (!expr2->is_constant_expression())
		throw logic_error("static_assert on non-constant expression");

	if (expr2->get_constant_data() != MakeImmediate(true)->get_constant_data())
		throw logic_error("static_assert failed: " + static_assert_declaration->message);
}

void SymbolTable::process_declaration(NamespaceAliasDefinition* namespace_alias_definition)
{
	current_namespace->set_namespace_alias(namespace_alias_definition->name, namespace_alias_definition->ns);
}

void SymbolTable::process_declaration(UsingDirective* using_directive)
{
	current_namespace->add_using_directive(using_directive->ns);
}

set<Entity*> SymbolTable::current_lookup_entity(QualifiedName* name, bool direct_only, EntityCategory entity_category)
{
	if (name->global)
	{
		Namespace* ns = current_translation_unit->global_namespace;

		for (size_t i = 0; i+1 < name->identifiers.size(); i++)
			ns = ns->qualified_lookup_namespace(name->identifiers[i]);

		return ns->qualified_lookup_entity(name->get_unqualified_id(), direct_only, entity_category);
	}
	else if (name->identifiers.size() == 1)
		return current_lookup_entity(name->get_unqualified_id(), entity_category);
	else
	{
		auto namespaces = current_lookup_entity(name->identifiers[0], EC_NAMESPACE);

		if (namespaces.empty())
			throw logic_error("unabled to find namespace " + name->identifiers[0]);

		if (namespaces.size() > 1)
			throw logic_error(name->identifiers[0] + " ambiguous");

		Namespace* ns = dynamic_cast<Namespace*>(*namespaces.begin());

		for (size_t i = 1; i+1 < name->identifiers.size(); i++)
			ns = ns->qualified_lookup_namespace(name->identifiers[i]);

		return ns->qualified_lookup_entity(name->get_unqualified_id(), direct_only, entity_category);
	}
}

Namespace* SymbolTable::lookup_namespace(QualifiedName* name)
{
	set<Entity*> entities = current_lookup_entity(name, false, EC_NAMESPACE);

	if (entities.size() > 1)
		throw logic_error("ambiguous lookup");

	return dynamic_cast<Namespace*>(*entities.begin());
}

Type* SymbolTable::lookup_type(QualifiedName* name)
{
	set<Entity*> entities = current_lookup_entity(name, false, EC_ANY);

	if (entities.empty())
		return nullptr;

	Type* type = nullptr;

	for (Entity* entity : entities)
		if (TypeDef* tdef = dynamic_cast<TypeDef*>(entity))
		{
			if (type && !tdef->type->equal(type))
				throw logic_error("ambiguous lookup");

			type = tdef->type;
		}
		else
			return nullptr;

	return type;
}

set<Entity*> SymbolTable::current_lookup_entity(const string& name, EntityCategory entity_category)
{
	map<Namespace*, set<Namespace*>> search_namespaces;
	return current_namespace->unqualified_lookup_entity(name, entity_category, search_namespaces);
}

Type* SymbolTable::deduce_type(DeclTypeSpecifier*)
{
	throw logic_error("deduce type not yet implemented");
}

void SymbolTable::process_declaration(UsingDeclaration* using_declaration)
{
	set<Entity*> entities = current_lookup_entity(using_declaration->name, false, EC_ANY & ~EC_NAMESPACE);

	if (entities.empty())
		throw logic_error("unable to find name");

	current_namespace->add_using_declaration(using_declaration->name->get_unqualified_id(), entities);
}

void SymbolTable::dump(ostream& out)
{
	out << translation_units.size() << " translation units" << endl;
	for (auto translation_unit : translation_units)
		translation_unit->dump(out);
}

void SymbolTable::mock_nsinit(ostream& out)
{
	vector<byte> program_image;
	link(program_image);
	out.write((char*) program_image.data(), program_image.size());
}

Variable* SymbolTable::create_temporary(EStorageDuration storage_duration, Type* type, Expression* initializer_expression)
{
	Variable* temp = new Variable(storage_duration, type, initializer_expression);

	temporary_variables.push_back(temp);

	return temp;
}

struct LinkedEntity : Pooled
{
	static LinkedEntity* construct(Entity*);

	size_t entity_size;
	size_t entity_alignment;
	size_t image_offset;

	virtual void add_entity(Entity* entity) = 0;

	virtual void merge() = 0;

	virtual void notify_offset() = 0;

	virtual vector<byte> get_data() = 0;
};


struct LinkedVariable : LinkedEntity
{
	vector<Variable*> variables;

	Variable* definition = nullptr;

	LinkedVariable(Variable* var) { add_entity(var); }

	virtual void add_entity(Entity* entity)
	{
		if (auto var = dynamic_cast<Variable*>(entity))
		{
			variables.push_back(var);
			var->linked_variable = this;
		}
		else
			throw logic_error("different entity category across translation unit");
	}

	virtual void merge()
	{
		for (auto var : variables)
			if (var->is_defined())
			{
				definition = var;

				entity_size = var->image_size();
				entity_alignment = var->image_alignment();

				break;
			}

		if (!definition)
			throw logic_error("variable undefined");
	}

	virtual void notify_offset()
	{
		for (auto var : variables)
			var->notify_offset(image_offset);
	}

	virtual vector<byte> get_data()
	{
		return definition->get_data();
	}

	virtual void write(ostream& out) const
	{
		out << "LinkedVariable " << variables.size() << " vars, def=(" << definition << ") size=" << entity_size << " align=" << entity_alignment << " offset=" << image_offset;
	}

};

struct LinkedFunction : LinkedEntity
{
	vector<Function*> functions;

	Function* definition = nullptr;

	LinkedFunction(Function* func) { add_entity(func); }

	virtual void add_entity(Entity* entity)
	{
		if (auto func = dynamic_cast<Function*>(entity))
		{
			functions.push_back(func);
			func->linked_function = this;
		}
		else
			throw logic_error("different entity category across translation unit");
	}

	virtual void merge()
	{
		entity_size = 4;
		entity_alignment = 4;

		for (auto func : functions)
			if (func->is_defined())
			{
				definition = func;

				entity_size = func->image_size();
				entity_alignment = func->image_alignment();
			}

		if (!definition)
			throw logic_error("function undefined");
	}

	virtual void notify_offset()
	{
		for (auto func : functions)
			func->notify_offset(image_offset);
	}

	vector<byte> get_data()
	{
		static const char* fun_magic = "fun";
		return vector<byte>(fun_magic, fun_magic+4);
	}

	virtual void dump()
	{
		cerr << "LinkedFunction" << endl;
	}
};

LinkedEntity* LinkedEntity::construct(Entity* entity)
{
	if (auto func = dynamic_cast<Function*>(entity))
		return new LinkedFunction(func);
	else if (auto var = dynamic_cast<Variable*>(entity))
		return new LinkedVariable(var);
	else
		throw logic_error("unexpected entity to link");
}

void SymbolTable::link(vector<byte>& program_image)
{
	vector<LinkedEntity*> linker_objects;
	map<const LinkerSymbol*, LinkedEntity*, LinkerSymbolLess> linker_symbols;

	for (auto translation_unit : translation_units)
	{
		for (auto entity : translation_unit->entities)
		{
			cerr << "LINKING: " << entity << endl;

			if (entity->linkage == LK_INTERNAL)
				linker_objects.push_back(LinkedEntity::construct(entity));
			else if (entity->linkage == LK_EXTERNAL)
			{
				const LinkerSymbol* linker_symbol = entity->linker_symbol;

				auto it = linker_symbols.find(linker_symbol);

				if (it == linker_symbols.end())
				{
					auto linker_object = LinkedEntity::construct(entity);

					linker_objects.push_back(linker_object);

					linker_symbols[linker_symbol] = linker_object;
				}
				else
				{
					it->second->add_entity(entity);
				}
			}
		}
	}

	for (auto temporary_variable : temporary_variables)
		linker_objects.push_back(LinkedEntity::construct(temporary_variable));

	for (auto linker_object : linker_objects)
		linker_object->merge();

	static const char* image_magic = "PA8";
	program_image = vector<byte>(image_magic, image_magic+4);

	auto insert_object = [&program_image](size_t alignment, size_t size) -> size_t
	{
		size_t padding = (alignment - (program_image.size() % alignment)) % alignment;

		program_image.insert(program_image.end(), padding, 0);

		size_t image_offset = program_image.size();

		program_image.insert(program_image.end(), size, 0);

		return image_offset;
	};

	for (auto linker_object : linker_objects)
		linker_object->image_offset =
			insert_object(linker_object->entity_alignment, linker_object->entity_size);

	for (auto string_literal : string_literals)
		string_literal->image_offset =
			insert_object(string_literal->type->alignment(), string_literal->type->size());

	for (auto linker_object : linker_objects)
		linker_object->notify_offset();

	for (auto translation_unit : translation_units)
		for (auto entity : translation_unit->entities)
			entity->relocate();

	for (auto linker_object : linker_objects)
	{
		auto data = linker_object->get_data();

		if (data.size() != linker_object->entity_size)
			throw logic_error("internal error: entity size mismatch");

		copy(data.begin(), data.end(), program_image.begin() + linker_object->image_offset);
	}

	for (auto string_literal : string_literals)
		copy(string_literal->data.begin(), string_literal->data.end(), program_image.begin() + string_literal->image_offset);
}
