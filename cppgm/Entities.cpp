#include "std.pch"

#include "Entities.h"

#include "AnnotatedExpressions.h"

#include "SymbolTable.h"

Function::Function(const string& name, SpecifiersBitField specifiers, FunctionType* type, Namespace* ns)
	: Entity(name, ns)
	, specifiers(specifiers)
	, type(type)
{
	ns->functions.push_back(this);
	ns->tu->entities.push_back(this);
	ns->add_member(this);

	if (ns->linker_symbol)
		linker_symbol = ns->linker_symbol->with_function_name(name, type);

	if (specifiers & SP_STATIC)
		linkage = LK_INTERNAL;
	else
		linkage = ns->linkage;
}

//FunctionOverloadSet::FunctionOverloadSet(const string& name, Namespace* ns)
//	: Entity(name, ns)
//{
//	ns->add_member(this);
//}

//Entity* FunctionOverloadSet::redeclare(SpecifiersBitField specifiers, Type* type_in, Namespace* ns)
//{
//	if (!type_in->is_function())
//		throw logic_error("entity declaration mismatch");
//
//	for (auto function : functions)
//	{
//		if (function->type->signature_equivilant(type_in))
//			return function->redeclare(specifiers, type_in, ns);
//	}
//
//	Function* function = new Function(name, specifiers, dynamic_cast<FunctionType*>(type_in), ns);
//
//	functions.push_back(function);
//
//	return function;
//}

Variable::Variable(const string& name, SpecifiersBitField specifiers, Type* type, Namespace* ns)
	: Entity(name, ns)
	, specifiers(specifiers)
	, type(type)
{
	check_specifiers(specifiers);

	if (specifiers & SP_THREAD_LOCAL)
		storage_duration = SD_THREAD_LOCAL;

	if (specifiers & SP_CONSTEXPR)
		this->type = type->const_qualify();

	ns->variables.push_back(this);
	ns->tu->entities.push_back(this);

	ns->add_member(this);

	if (ns->linker_symbol)
		linker_symbol = ns->linker_symbol->with_name(name);

	if (specifiers & SP_STATIC)
		linkage = LK_INTERNAL;
	else if (type->cv.const_qualified && !(specifiers & SP_EXTERN))
		linkage = LK_INTERNAL;
	else
		linkage = ns->linkage;
}

Variable::Variable(EStorageDuration storage_duration, Type* type, Expression* initializer_expression)
	: Entity("<temp>", nullptr)
	, type(type)
	, initializer_expression(initializer_expression)
	, storage_duration(storage_duration)
{

}

void Variable::relocate()
{
	if (initializer_expression)
		initializer_expression = initializer_expression->relocate();
}

void Variable::check_specifiers(SpecifiersBitField specifiers)
{
	size_t n_storage_class_specifiers = 0;

	if (specifiers & SP_REGISTER) n_storage_class_specifiers++;
	if (specifiers & SP_STATIC) n_storage_class_specifiers++;
	if (specifiers & SP_THREAD_LOCAL) n_storage_class_specifiers++;
	if (specifiers & SP_EXTERN) n_storage_class_specifiers++;
	if (specifiers & SP_MUTABLE) n_storage_class_specifiers++;

	if (n_storage_class_specifiers > 2)
		throw logic_error("more than 2 storage class specifiers");

	if (n_storage_class_specifiers == 2 && !(specifiers & SP_THREAD_LOCAL))
		throw logic_error("2 storage class specifiers and neither thread_local");
}

Entity* Variable::redeclare(SpecifiersBitField specifiers_in, Type* type_in, Namespace*)
{
	check_specifiers(specifiers_in);

	if ((specifiers & SP_CONSTEXPR) != (specifiers_in & SP_CONSTEXPR))
		throw logic_error("constexpr must appear on all declarations of same variable");

	if ((specifiers_in & SP_THREAD_LOCAL) ^ (specifiers & SP_THREAD_LOCAL))
		throw logic_error(name + " not declared thread_local in all declarations");

	if (specifiers & SP_CONSTEXPR)
		type_in = type_in->const_qualify();

	if (specifiers_in & SP_TYPEDEF)
		throw logic_error(name + " redeclared typedef");

	if (!type->equal(type_in))
	{
		if (auto type_arr = dynamic_cast<ArrayType*>(type))
			if (auto type_in_arr = dynamic_cast<ArrayType*>(type_in))
			{
				if (!type_arr->element_type->equal(type_in_arr->element_type))
					throw logic_error(name + " redeclared different type");

				if (type_arr->bound == 0)
					type = type_in;
				else if (type_in_arr->bound != 0)
					throw logic_error(name + " redeclared different type");
			}
			else
			{
				throw logic_error(name + " redeclared different type");
			}
		else
		{
			throw logic_error(name + " redeclared different type");
		}
	}

	if ((specifiers & SP_EXTERN) && !(specifiers_in & SP_EXTERN))
		specifiers &= ~SP_EXTERN;

	if ((specifiers_in & SP_STATIC) && (!specifiers & SP_STATIC))
		throw logic_error("static on external variable");

	return this;
}

TypeDef::TypeDef(SpecifiersBitField specifiers, const string& name, Type* type, Namespace* ns)
	: Entity(name, ns)
	, type(type)
{
	if (specifiers & SP_STORAGE_CLASS_SPECIFIERS)
		throw logic_error("storage class specifier in typedef");

	ns->typedefs.push_back(this);

	ns->add_member(this);
}

Entity* TypeDef::redeclare(SpecifiersBitField specifiers_in, Type* type_in, Namespace*)
{
	if (specifiers_in & SP_STORAGE_CLASS_SPECIFIERS)
		throw logic_error("storage class specifier in typedef");

	if (!type->equal(type_in))
		throw logic_error(name + " redeclared different type");

	if (!(specifiers_in & SP_TYPEDEF))
		throw logic_error(name + " redeclared non-typedef");

	return this;
}

Entity* Namespace::declare_name(SpecifiersBitField specifiers, const string& name, Type* type)
{
	auto range = scope.equal_range(name);

	set<Entity*> existing_entities;

	for (auto it = range.first; it != range.second; ++it)
		existing_entities.insert(it->second);

	Entity* entity_out = nullptr;

	for (Entity* existing_entity : existing_entities)
	{
		if (existing_entity->can_coexist(type))
			continue;

		if (!is_member(existing_entity))
			throw logic_error("attempt to redeclare non-member");

		if (entity_out)
			throw logic_error("internal error: multiple namespace members that cant coexist");

		entity_out = existing_entity->redeclare(specifiers, type, this);
	}

	if (entity_out)
		return entity_out;

	// create new member
	if (specifiers & SP_TYPEDEF)
		return new TypeDef(specifiers, name, type, this);
	else if (type->is_function())
		return new Function(name, specifiers, dynamic_cast<FunctionType*>(type), this);
	else
		return new Variable(name, specifiers, type, this);
}

void Namespace::set_namespace_alias(const string& name, Namespace* ns)
{
	auto range = scope.equal_range(name);

	if (range.first == range.second)
		scope.insert(make_pair(name, ns));
	else
	{
		if (range.first->second != ns)
			throw logic_error(name + " already exists");

		if (get_member_namespace(name))
			throw logic_error(name + " is an original-namespace-name");
	}
}

Namespace* Namespace::find_common_ancestor(Namespace* ns)
{
	for (Namespace* ns2 = this; ns2 != nullptr; ns2 = ns2->enclosing_namespace)
		if (ns->ancestor_namespaces.count(ns2))
			return ns2;

	throw logic_error("internal error: namespaces have no common ancestor");
}

set<Entity*> Namespace::unqualified_lookup_entity(const string& name, EntityCategory entity_category,
                                             map<Namespace*, set<Namespace*>>& search_namespaces)
{
	for (Namespace* ns : using_directives_closure)
		search_namespaces[find_common_ancestor(ns)].insert(ns);

	const set<Namespace*>& search_set = search_namespaces[this];

	set<Entity*> result;

	for (Namespace* ns : search_set)
	{
		set<Entity*> entities = ns->simple_lookup_entity(name, false, entity_category);

		result.insert(entities.begin(), entities.end());
	}

	if (!result.empty())
		return result;

	if (enclosing_namespace)
		return enclosing_namespace->unqualified_lookup_entity(name, entity_category, search_namespaces);

	return {};
}

bool Variable::is_constant() const
{
	if (!initializer_expression)
		return false;

	if (!initializer_expression->is_constant_expression())
		return false;

	Type* t = type;

	if (t->is_reference())
		t = dynamic_cast<ReferenceType*>(t)->T;

	if (t->is_integral_type() && t->cv.const_qualified && !t->cv.volatile_qualified)
		return true;

	if (t->is_literal_type() && !t->cv.volatile_qualified && (specifiers & SP_CONSTEXPR))
		return true;

	return false;
}

vector<byte> Variable::get_data()
{
	if (type->is_reference())
	{
		return initializer_expression->get_reference_data();
	}
	else
	{
		if (!initializer_expression || !initializer_expression->is_constant_expression())
			return vector<byte>(image_size(), 0);

		return initializer_expression->get_constant_data();
	}
}

void Variable::write(ostream& out) const
{
	out << "variable " << name << " " << SpecifiersToString(specifiers) << " " <<
		ELinkageToString(linkage) << " " << type <<
		" initializer=(" << initializer << ") initializer_expression=(" << initializer_expression << ") "
		<< "is_constant = " << is_constant();
}

void Variable::set_initializer(SymbolTable& symtab, Initializer* initializer_in)
{
	if ((specifiers & SP_CONSTEXPR) && !initializer_in)
		throw logic_error("constexpr variable requires initializer");

	if (initializer && initializer_in)
		throw logic_error("variable " + name + " already has an initializer");

	if (initializer_in)
	{
		initializer = initializer_in;
		initializer_expression = initializer->create_initializer_expression(symtab, type);

		if (type->is_reference() && initializer_expression->value_category == VC_PRVALUE)
		{
			Variable* temp = symtab.create_temporary(storage_duration, initializer_expression->type, initializer_expression);

			initializer_expression = new VariableExpression(temp->type, temp);
		}

		if (auto array_type = dynamic_cast<ArrayType*>(type))
		{
			if (array_type->bound == 0)
			{
				if (auto array_type_in = dynamic_cast<ArrayType*>(initializer_expression->type))
					type = new ArrayType(array_type->element_type, array_type_in->bound);
			}
		}
	}
	else if (!(specifiers & SP_EXTERN))
	{
		if (!type->is_default_initializable())
			throw logic_error("type cannot be default initialized");

		initializer = new DefaultInitializer();
		initializer_expression = initializer->create_initializer_expression(symtab, type);
	}

	if (is_defined() && !type->is_complete())
		throw logic_error("variable defined with incomplete type");

//	cerr << "DEBUG: variable = " << this << endl;

}
