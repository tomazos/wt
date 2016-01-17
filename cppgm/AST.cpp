#include "std.pch"

#include "AST.h"

struct ASTBase
{
	virtual void write(size_t depth, ostream&) const = 0;
	virtual shared_ptr<ASTBase> clone() const = 0;
	virtual bool has(const string&) const { return false; }
	virtual AST& lookup(const string& key) = 0;
	virtual const AST& lookup(const string& key) const = 0;
	virtual bool test_bool() const = 0;
	virtual bool empty() const { return !test_bool(); }
	virtual ~ASTBase() {}
};

struct ASTTable : ASTBase
{
	virtual shared_ptr<ASTBase> clone() const
	{
		return make_shared<ASTTable>(*this);
	}

	virtual void write(size_t depth, ostream& o) const
	{
		if (!test_bool())
		{
			o << "null" << endl;
			return;
		}

		o << endl;
		for (const auto& x : table)
			if (x.second)
			{
				o << string(4*(depth+1), ' ') << x.first << ": ";

				x.second.write(depth+1, o);
			}
	}

	map<string, AST> table;

	bool has(const string& key) const
	{
		return table.count(key);
	}

	virtual AST& lookup(const string& key)
	{
		return table[key];
	}

	virtual const AST& lookup(const string& key) const
	{
		auto it = table.find(key);

		if (it != table.end())
			return it->second;

		static AST null_ast;
		return null_ast;
	}

	virtual bool test_bool() const
	{
		for (const auto& x : table)
			if (x.second)
				return true;

		return false;
	}

};

struct ASTToken : ASTBase
{
	PToken token;

	ASTToken(const PToken& token)
		: token(token)
	{
		if (!token)
			throw logic_error("internal error: ASTToken from null token");
	}

	virtual shared_ptr<ASTBase> clone() const
	{
		return make_shared<ASTToken>(*this);
	}

	virtual void write(size_t, ostream& o) const
	{
		token->write(o);
		o << endl;
	}

	virtual AST& lookup(const string&)
	{
		throw logic_error("internal error: AST::lookup on token");
	}

	virtual const AST& lookup(const string&) const
	{
		throw logic_error("internal error: AST::lookup on token");
	}

	virtual bool test_bool() const
	{
		return true;
	}
};

struct ASTEmpty : ASTBase
{
	virtual shared_ptr<ASTBase> clone() const
	{
		return make_shared<ASTEmpty>();
	}

	virtual void write(size_t, ostream& o) const
	{
		o << "empty";
		o << endl;
	}

	virtual AST& lookup(const string&)
	{
		throw logic_error("internal error: AST::lookup on empty");
	}

	virtual const AST& lookup(const string&) const
	{
		throw logic_error("internal error: AST::lookup on empty");
	}

	virtual bool test_bool() const
	{
		return true;
	}

	virtual bool empty() const
	{
		return true;
	}
};

AST::AST(astempty_t)
	: m(make_shared<ASTEmpty>())
{
}

AST::AST(const PToken& token)
	: m(make_shared<ASTToken>(token))
{
}

AST::AST(const AST& that)
	: m(that.m ? that.m->clone() : nullptr)
{

}

AST::AST(AST&& that)
	: m(move(that.m))
{}

AST& AST::operator=(const AST& that)
{
	if (that.m)
		m = that.m->clone();
	else
		m = nullptr;

	return (*this);
}

AST& AST::operator=(AST&& that)
{
	m = move(that.m);

	return (*this);
}

AST::operator bool() const
{
	if (!m)
		return false;

	return m->test_bool();
}

bool AST::empty() const
{
	if (!m)
		return true;

	return m->empty();
}

bool AST::has(const string& key) const
{
	if (!m)
		return false;

	return m->has(key);
}

const AST& AST::operator[](const string& key) const
{
	if (!m)
	{
		static AST null_ast;
		return null_ast;
	}

	return m->lookup(key);
}

const AST& AST::operator[](size_t idx) const
{
	string s = to_string(idx);
	if (s.size() < 2)
		s = "0" + s;

	return (*this)[s];
}

AST& AST::operator[](const string& key)
{
	if (!m)
		m = make_shared<ASTTable>();
	return m->lookup(key);
}

AST& AST::operator[](size_t idx)
{
	string s = to_string(idx);
	if (s.size() < 2)
		s = "0" + s;

	return (*this)[s];
}

void AST::write(size_t depth, ostream& o) const
{
	if (!m)
		o << "null" << endl;
	else
		m->write(depth, o);
}

ostream& operator<<(ostream& o, const AST& ast)
{
	ast.write(0, o);

	return o;
}

astempty_t astempty;
