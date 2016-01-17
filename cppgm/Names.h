#pragma once

#include "Pooled.h"

struct FunctionType;

struct QualifiedName : Pooled
{
	bool global;
	vector<string> identifiers;

	QualifiedName(bool global)
		: global(global)
	{}

	void push_identifier(const string& identifier)
	{
		identifiers.push_back(identifier);
	}

	bool unqualified() const { return !global && identifiers.size() == 1; }

	string get_unqualified_id() const
	{
		return identifiers.back();
	}

	void write(ostream& o) const
	{
		if (global)
			o << "::";

		size_t n = identifiers.size();

		for (size_t i = 0; i < n; i++)
		{
			o << identifiers[i];

			if (i < n-1)
				o << "::";
		}
	}
};

struct LinkerName
{
	string name;
	FunctionType* extra = nullptr;

	LinkerName(const string& name)
		: name(name)
	{}

	LinkerName(const string& name, FunctionType* extra)
		: name(name)
		, extra(extra)
	{}

	bool operator<(const LinkerName& that) const;
};

struct LinkerSymbol : Pooled
{
	const vector<LinkerName> names;

	static const LinkerSymbol* global_linker_symbol()
	{
		return new LinkerSymbol;
	}

	const LinkerSymbol* with_name(const string& name) const
	{
		vector<LinkerName> new_names(names);

		new_names.push_back(name);

		return new LinkerSymbol(move(new_names));
	}

	const LinkerSymbol* with_function_name(const string& name, FunctionType* extra) const
	{
		vector<LinkerName> new_names(names);

		new_names.push_back(LinkerName(name,extra));

		return new LinkerSymbol(move(new_names));
	}

	bool operator<(const LinkerSymbol& that) const
	{
		return names < that.names;
	}

private:
	LinkerSymbol() = default;

	LinkerSymbol(vector<LinkerName>&& names_in)
		: names(move(names_in))
	{

	}
};

struct LinkerSymbolLess {
    bool operator() (const LinkerSymbol* const & lhs, const LinkerSymbol* const & rhs) const{
        return *lhs < *rhs;
    }
};
