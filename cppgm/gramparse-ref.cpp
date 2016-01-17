#include "std.pch"

enum TokenType
{
	ident,
	qmark,
	star,
	plussign,
	colon,
	newline,
	lparen,
	rparen,
	eof
};

struct Token
{
	Token(TokenType type, size_t linenum)
		: type(type)
		, linenum(linenum)
	{}

	Token(TokenType type, string src, size_t linenum)
		: type(type)
		, src(src)
		, linenum(linenum)
	{}

	TokenType type;
	string src;
	size_t linenum;
};

vector<Token> tokens;
set<string> terminals;
unordered_set<string> epsilons;
struct Rule;
unordered_map<string, Rule*> rulemap;
unordered_map<string,unordered_set<string>> firsts;
unordered_map<string,set<string>> follows;

bool isidchar(char c) { return (c == '_' || c == '-' || isalpha(c) || isdigit(c)); }

size_t pos = 0;

// grammar:
//     grammar-part* eof
//
// grammar-part:
//     production
//     newline
//
// production:
//     head body+ newline
//
// head:
//     ident colon newline
//
// body:
//     body-part+ newline
//
// body-part:
//     body-atom
//     body-atom qmark
//     body-atom star
//     body-atom plussign
//
// body-atom:
//     ident
//     lparen body-part+ rparen

template<typename T, typename... Args>
unique_ptr<T> make_unique(Args&&... args)
{
	return unique_ptr<T>(new T(forward<Args>(args)...));
}

struct Part;
typedef unique_ptr<Part> PPart;
struct Part
{
	virtual void collect_used(vector<string>& used) const = 0;
	virtual size_t count_used(const string& symbol) const = 0;
	virtual void write(ostream& o, bool root) const = 0;
	virtual bool epsilon() const = 0;
	virtual void collect_first(set<string>& x) const = 0;
	virtual void scan_follow(set<string> f) const = 0;
	virtual ~Part() {}
};

struct Ident : Part
{
	Ident(string&& src)
		: src(move(src))
	{}

	virtual void collect_used(vector<string>& used) const
	{
		auto it = find(used.begin(), used.end(), src);
		if (it == used.end())
			used.push_back(src);
	}

	virtual size_t count_used(const string& symbol) const
	{
		return src == symbol ? 1 : 0;
	}

	virtual void write(ostream& o, bool) const
	{
		if (terminals.count(src))
			o << src;
		else
			o << "<a href=\"" << src << ".html\">" << src << "</a>";
	}

	virtual void collect_first(set<string>& x) const;

	virtual bool epsilon() const { return epsilons.count(src); }

	virtual void scan_follow(set<string> f) const
	{
		for (string s : f)
			follows[src].insert(s);
	}

	string src;
};

struct Parts : Part
{
	Parts(vector<PPart>&& parts)
		: parts(move(parts))
	{}

	virtual void collect_used(vector<string>& used) const
	{
		for (const auto& part : parts)
			part->collect_used(used);
	}

	virtual size_t count_used(const string& symbol) const
	{
		size_t t = 0;

		for (const auto& part : parts)
			t += part->count_used(symbol);

		return t;
	}

	virtual void scan_follow(set<string> f) const
	{
		for (size_t i = parts.size(); i > 0; i--)
		{
			parts[i-1]->scan_follow(f);

			if (!parts[i-1]->epsilon())
				break;
		}

		for (size_t i = 0; i < parts.size(); i++)
			for (size_t j = i+1; j < parts.size(); j++)
			{
				set<string> g;
				parts[j]->collect_first(g);
				parts[i]->scan_follow(g);

				if (!parts[j]->epsilon())
					break;
			}
	}

	virtual void write(ostream& o, bool root) const
	{
		if (parts.size() == 0)
			throw logic_error("empty parts");
		else if (parts.size() == 1)
			parts[0]->write(o, root);
		else
		{
			if (!root)
				o << "(";

			for (size_t i = 0; i < parts.size()-1; i++)
			{
				parts[i]->write(o, false);
				o << " ";
			}
			parts[parts.size()-1]->write(o, false);

			if (!root)
				o << ")";
		}
	}

	virtual bool epsilon() const
	{
		for (const auto& part : parts)
			if (!part->epsilon())
				return false;

		return true;
	}

	virtual void collect_first(set<string>& x) const
	{
		for (const auto& part : parts)
		{
			part->collect_first(x);

			if (!part->epsilon())
				return;
		}
	}

	vector<PPart> parts;
};

enum QuantType
{
	zeroone,
	zeroinf,
	oneinf
};

struct Quant : Part
{
	Quant(QuantType type, PPart&& part)
		: type(type)
		, part(move(part))
	{}

	virtual void collect_used(vector<string>& used) const
	{
		part->collect_used(used);
	}

	virtual size_t count_used(const string& symbol) const
	{
		return part->count_used(symbol);
	}

	virtual void write(ostream& o, bool) const
	{
		part->write(o, false);

		if (type == zeroone)
			o << "?";
		else if (type == zeroinf)
			o << "*";
		else // type == oneinf
			o << "+";
	}

	virtual bool epsilon() const
	{
		if (type != oneinf)
			return true;

		return part->epsilon();
	}

	virtual void collect_first(set<string>& x) const
	{
		part->collect_first(x);
	}


	virtual void scan_follow(set<string> f) const
	{
		part->scan_follow(f);

		if (type == zeroone)
			return;

		set<string> g;
		part->collect_first(g);
		part->scan_follow(g);
	}

	QuantType type;
	PPart part;
};

struct Rule;
typedef unique_ptr<Rule> PRule;
struct Rule
{
	Rule(string&& head, vector<PPart>&& bodies)
		: head(move(head))
		, bodies(move(bodies))
	{}
	void collect_used(vector<string>& used) const
	{
		for (const auto& body : bodies)
			body->collect_used(used);
	}

	virtual size_t count_used(const string& symbol) const
	{
		size_t t = 0;

		for (const auto& body : bodies)
			t += body->count_used(symbol);

		return t;
	}

	void write(ostream& o) const
	{
		o << "<p>";
		o << "<a href=\"" << head << ".html\">" << head << "</a>" << ":" << "<br/>";
		for (const auto& body : bodies)
		{
			o << "&nbsp;&nbsp;&nbsp;&nbsp;";
			body->write(o, true);
			o << "<br/>";
		}

		o << "</p>";
	}

	bool epsilon()
	{
		for (const auto& body : bodies)
			if (body->epsilon())
				return true;

		return false;
	}

	void collect_first(set<string>& x)
	{
		for (const auto& body : bodies)
			body->collect_first(x);
	}

	void scan_follow()
	{
		set<string> f = follows[head];

		for (const auto& body : bodies)
			body->scan_follow(f);
	}

	string head;
	vector<PPart> bodies;
};

void Ident::collect_first(set<string>& x) const
{
	for (auto f : firsts[src])
		x.insert(f);
}

unique_ptr<string> parse_ident()
{
	string src = tokens[pos].src;
	if (tokens[pos++].type == ident)
		return make_unique<string>(src);
	else
		return nullptr;
}

bool parse_token(TokenType type)
{
	return (tokens[pos++].type == type);
}

unique_ptr<string> parse_head()
{
	auto x = parse_ident();
	if (x && parse_token(colon) && parse_token(newline))
		return x;
	else
		return nullptr;
}

PPart parse_body_part();

PPart parse_body_atom()
{
	size_t start = pos;

	auto x = parse_ident();

	if (x)
		return make_unique<Ident>(move(*x));

	pos = start;

	if (!parse_token(lparen))
		return nullptr;

	vector<PPart> parts;

nextloop:
	start = pos;

	PPart part = parse_body_part();

	if (part)
	{
		parts.emplace_back(move(part));
		goto nextloop;
	}

	pos = start;

	if (parts.empty() || !parse_token(rparen))
		return nullptr;

	return make_unique<Parts>(move(parts));
}

PPart parse_body_part()
{
	auto x = parse_body_atom();

	if (!x)
		return false;

	size_t after_atom = pos;

	if (parse_token(qmark))
		return make_unique<Quant>(zeroone, move(x));

	pos = after_atom;

	if (parse_token(star))
		return make_unique<Quant>(zeroinf, move(x));

	pos = after_atom;

	if (parse_token(plussign))
		return make_unique<Quant>(oneinf, move(x));

	pos = after_atom;

	return x;
}

PPart parse_body()
{
	size_t start;
	PPart part;

	vector<PPart> parts;

nextloop:
	start = pos;

	part = parse_body_part();

	if (part)
	{
		parts.emplace_back(move(part));
		goto nextloop;
	}

	pos = start;

	if (parts.empty() || !parse_token(newline))
		return nullptr;

	return make_unique<Parts>(move(parts));
}

PRule parse_production()
{
	auto head = parse_head();

	if (!head)
		return nullptr;

	vector<PPart> bodies;

	size_t start;
	PPart body;

nextloop:

	start = pos;

	body = parse_body();

	if (body)
	{
		bodies.emplace_back(move(body));
		goto nextloop;
	}

	pos = start;

	if (bodies.empty() || !parse_token(newline))
		return nullptr;

	return make_unique<Rule>(move(*head), move(bodies));
}

void skip_newlines()
{
	while (true)
	{
		size_t start = pos;
		if (!parse_token(newline))
		{
			pos = start;
			break;
		}
	}
}

PRule parse_grammar_part()
{
	skip_newlines();
	auto x = parse_production();
	skip_newlines();
	return x;
}

vector<PRule> rules;

void parse_grammar()
{
	size_t start;
	PRule rule;

nextloop:

	start = pos;

	rule = parse_grammar_part();

	if (rule)
	{
		rules.emplace_back(move(rule));
		goto nextloop;
	}

	pos = start;

	if (!parse_token(eof))
		throw logic_error(to_string(tokens[pos-1].linenum) + ": expected eof");
}

int main(int, char**)
{
	try
	{
		ostringstream oss;
		oss << cin.rdbuf();

		string input = oss.str();
		input += '\n';
		input += '\n';
		input += '\0';

		string acc;

		size_t linenum = 0;

		for (char c : input)
		{
			if (c == '\n')
				linenum++;

			static bool skipnext = false;

			if (c == '\\')
			{
				skipnext = true;
				continue;
			};

			if (skipnext)
			{
				if (c != '\n')
					throw logic_error("expected newline after \\: " + string(1,c));

				skipnext = false;
				continue;
			}

			if (isidchar(c))
			{
				acc.push_back(c);
				continue;
			}

			if (!acc.empty())
			{
				tokens.emplace_back(ident, acc, linenum);
				acc.clear();
			}

			switch (c)
			{
			case '?': tokens.emplace_back(qmark, "?", linenum); break;
			case '+': tokens.emplace_back(plussign, "+", linenum); break;
			case '*': tokens.emplace_back(star, "*", linenum); break;
			case ':': tokens.emplace_back(colon, ":", linenum); break;
			case '\n': tokens.emplace_back(newline, "\\n", linenum); break;
			case '(': tokens.emplace_back(lparen, "(", linenum); break;
			case ')': tokens.emplace_back(rparen, ")", linenum); break;
			default:
				if (c && !isspace(c))
					throw logic_error(to_string(linenum) + ": unexpected char: " + string(1,c));
			}
		}
		tokens.emplace_back(eof, "EOF", linenum);

		parse_grammar();

		for (size_t i = 0; i < rules.size(); i++)
		{
			string head = rules[i]->head;

			if (rulemap.count(head))
				throw logic_error("multiple definitions: " + head);

			rulemap[head] = rules[i].get();
		}

		vector<string> used;
		for (const auto& rule : rules)
		{
			for (const auto& body : rule->bodies)
			{
				body->collect_used(used);
			}
		}

		for (auto u : used)
		{
			if (rulemap.count(u) == 0)
			{
				cout << "TERMINAL: " << u << endl;
				terminals.insert(u);
			}
		}

		for (const auto& rule : rules)
		{
			string head = rule->head;

			if (find(used.begin(), used.end(), head) == used.end())
				cout << "UNUSED: " << head << endl;
		}

		for (const auto& rule : rules)
		{
			vector<string> u;
			rule->collect_used(u);

			if (find(u.begin(), u.end(), rule->head) != u.end())
				cout << "DIRECT-SELF: " << rule->head << endl;
		}

		while (true)
		{
			size_t n = epsilons.size();

			for (const auto& rule : rules)
				if (rule->epsilon())
					epsilons.insert(rule->head);

			if (epsilons.size() == n)
				break;
		}

		for (auto x : epsilons)
			cout << "EPSILON: " << x << endl;

		for (const auto& rule : rules)
		{
			set<string> x;
			cout << "COLLECT_FIRST: " << rule->head << endl;
			rule->collect_first(x);
		}

		for (auto terminal : terminals)
			firsts[terminal].insert(terminal);

		while (true)
		{
			size_t n = 0;

			for (const auto& f : firsts)
				n += f.second.size();

			for (const auto& rule : rules)
			{
				set<string> fs;
				rule->collect_first(fs);
				for (auto f : fs)
					firsts[rule->head].insert(f);
			}

			for (const auto& f : firsts)
				n -= f.second.size();

			if (n == 0)
				break;
		}

		while (true)
		{
			size_t n = 0;

			for (const auto& f : follows)
				n += f.second.size();

			for (const auto& rule : rules)
				rule->scan_follow();

			for (const auto& f : follows)
				n -= f.second.size();

			if (n == 0)
				break;
		}

		{
			ofstream o("terminals.html");

			o << "<html><head><title>Terminals</title></head><body><h1>List of Terminals</h1>";

			for (string terminal : terminals)
			{
				o << "<p><b>" << terminal << "</b>: ";

				for (const auto& rule : rules)
				{
					size_t t = rule->count_used(terminal);

					if (t == 1)
						o << "<a href=\"" << rule->head << ".html\">" << rule->head << "</a>, ";
					else if (t > 1)
						o << "<a href=\"" << rule->head << ".html\">" << rule->head << "</a> (x " << t << "), ";
				}

				o << "</p>";

			}
			o << "</body></html>";
		}
		for (const auto& rule : rules)
		{
			string head = rule->head;

			ofstream o(head + ".html");

			o << "<html><head><title>";
			o << head;
			o << "</title></head><body>";

			o << "<h1>" << head << "</h1>";

			o << "<p><b>USED BY:</b> ";
			for (const auto& rule2 : rules)
			{
				size_t t = rule2->count_used(head);

				if (t == 1)
					o << "<a href=\"" << rule2->head << ".html\">" << rule2->head << "</a>, ";
				else if (t > 1)
					o << "<a href=\"" << rule2->head << ".html\">" << rule2->head << "</a> (x " << t << "), ";
			}
			o << "</p>";

			o << "<p><b>FIRST:</b> ";
			set<string> x;
			rule->collect_first(x);

			for (auto y : x)
				o << y << " ";
			o << "</p>";

			o << "<p><b>FOLLOW:</b> ";
			x = follows[head];
			for (auto y : x)
				o << y << " ";
			o << "</p>";

			vector<string> todo;
			unordered_set<string> displayed;

			todo.push_back(head);

			while (!todo.empty())
			{
				string x = todo.front();
				todo.erase(todo.begin());

				if (displayed.count(x))
					continue;

				auto it = rulemap.find(x);
				if (it == rulemap.end())
					continue;

				it->second->write(o);
				displayed.insert(x);

				vector<string> u;
				Rule* r = it->second;
				r->collect_used(u);
				for (auto y : u)
					todo.push_back(y);
			}

			o << "</body></html>";
		}
	}

	catch (exception& e)
	{
		cerr << "ERROR: " << e.what() << endl;
		return EXIT_FAILURE;
	}
}
