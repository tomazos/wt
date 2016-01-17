#include "std.pch"

#include "MacroReplacement.h"
#include "PPTokenize.h"
#include "PreprocessingTokenParser.h"

//  #define __CPPGM__ 201303L              // CPPGM course run version
//  #define __CPPGM_AUTHOR__ "John Smith"  // Your full real name as enrolled in the course
//
//  #define __cplusplus 201103L            // C++ version
//  #define __STDC_HOSTED__ 1              // hosted implementation
//
//  #define __FILE__ "foo"                 // current presumed source file name
//  #define __LINE__ 123                   // current presumed source line number
//
//  #define __DATE__ "Mmm dd yyyy"         // build date from asctime
//  #define __TIME__ "hh:mm:ss"            // build time from asctime

string BuildDate = "uninitialized";
string BuildTime = "uninitialized";

void SetBuildDate()
{
    time_t result = time(NULL);
    string timestamp = asctime(localtime(&result));

    BuildDate = "\"" + timestamp.substr(4,6) + " " + timestamp.substr(20, 4) + "\"";
    BuildTime = "\"" + timestamp.substr(11,8) + "\"";
}

PreprocessingToken JoinPreprocessingTokens(const PreprocessingToken& lhs, const PreprocessingToken& rhs)
{
	if (lhs.is_placemarker())
		return rhs;

	if (rhs.is_placemarker())
		return lhs;

	string source = lhs.src.spelling + rhs.src.spelling;

	PreprocesingTokenPPTokenStream stream;

	PPTokenize(source, stream);

	return stream.tokens[0];
}

vector<PreprocessingToken> ProcessHash2(vector<PreprocessingToken> input)
{
	vector<PreprocessingToken> output;

	size_t pos = 0;

	while (true)
	{
		if (pos == input.size())
			break;

		size_t pos_lhs = pos;

		size_t pos_hash2 = pos+1;

		if (pos_hash2 < input.size() && input[pos_hash2].is_ws())
			pos_hash2++;

		if (pos_hash2 >= input.size() || !input[pos_hash2].is_hash2_blessed())
		{
			output.push_back(input[pos]);
			pos++;
			continue;
		}

		size_t pos_rhs = pos_hash2+1;

		if (pos_rhs < input.size() && input[pos_rhs].is_ws())
			pos_rhs++;

		if (pos_rhs >= input.size())
		{
			output.push_back(input[pos]);
			pos++;
			continue;
		}

		input[pos_rhs] = JoinPreprocessingTokens(input[pos_lhs], input[pos_rhs]);
		pos = pos_rhs;
	}

	for (auto& x : output)
		x.blessed = false;

	NormalizeWhitespace(output);
	CheckVarargs(output);

	return output;
}

MacroDefinition::MacroDefinition(const string& macro_name, const vector<PreprocessingToken>& replacement_list)
	: function_like(false)
	, macro_name(macro_name)
	, replacement_list(replacement_list)
	, var_args(false)
{
	bless();

	if (macro_name == "__VA_ARGS__")
		throw logic_error("invalid __VA_ARGS__ use");

	for (auto x : replacement_list)
		if (x.pp_type == PP_IDENTIFIER && x.src.spelling == "__VA_ARGS__")
			throw logic_error("invalid __VA_ARGS__ use");

	if (!this->replacement_list.empty() && (this->replacement_list.front().is_hash2_blessed() || this->replacement_list.back().is_hash2_blessed()))
		throw logic_error("## at edge of replacement list");

//		dump();
}

MacroDefinition::MacroDefinition(const string& macro_name, bool var_args, const vector<string>& parameters_in,
                const vector<PreprocessingToken>& replacement_list_in)
	: function_like(true)
	, macro_name(macro_name)
	, replacement_list(replacement_list_in)
	, var_args(var_args)
	, parameters(parameters_in)
{

	if (macro_name == "__VA_ARGS__")
		throw logic_error("invalid __VA_ARGS__ use");

	for (size_t i = 0; i < parameters.size(); i++)
	{
		if (parameters[i] == "__VA_ARGS__")
			throw logic_error("__VA_ARGS__ in macro parameter list");

		for (size_t j = i+1; j < parameters.size(); j++)
			if (parameters[i] == parameters[j])
				throw logic_error("duplicate parameter " + parameters[i] + " in macro definition");
	}

	if (var_args)
		this->parameters.push_back("__VA_ARGS__");
	else
	{
		for (auto x : replacement_list)
			if (x.pp_type == PP_IDENTIFIER && x.src.spelling == "__VA_ARGS__")
				throw logic_error("invalid __VA_ARGS__ use");
	}
	bless();

	if (!this->replacement_list.empty() && (this->replacement_list.front().is_hash2_blessed() || this->replacement_list.back().is_hash2_blessed()))
		throw logic_error("## at edge of replacement list");

	for (size_t i = 0; i < replacement_list.size(); i++)
		if (replacement_list[i].is_hash())
		{
			if (i == replacement_list.size()-1)
				throw logic_error("# at end of function-like macro replacement list");
			else if (i == replacement_list.size() - 2)
			{
				if (replacement_list[i+1].pp_type != PP_IDENTIFIER || !has_param(replacement_list[i+1].src.spelling))
					throw logic_error("# must be followed by parameter in function-like macro");
			}
			else
			{
				if (replacement_list[i+1].pp_type == PP_WHITESPACE)
				{
					if (replacement_list[i+2].pp_type != PP_IDENTIFIER || !has_param(replacement_list[i+2].src.spelling))
						throw logic_error("# must be followed by parameter in function-like macro");
				}
				else
				{
					if (replacement_list[i+1].pp_type != PP_IDENTIFIER || !has_param(replacement_list[i+1].src.spelling))
						throw logic_error("# must be followed by parameter in function-like macro");
				}
			}
		}

	for (string param : parameters)
		for (size_t i = 0; i < replacement_list.size(); i++)
			if (replacement_list[i].pp_type == PP_IDENTIFIER && replacement_list[i].src.spelling == param)
			{
				if (i > 0 && replacement_list[i-1].is_hash())
					param_usage.insert(make_pair(param, eStringized));
				else if (i > 1 && replacement_list[i-1].is_ws() && replacement_list[i-2].is_hash())
					param_usage.insert(make_pair(param, eStringized));
				else if (i > 0 && replacement_list[i-1].is_hash2_blessed())
					param_usage.insert(make_pair(param, eUnexpanded));
				else if (i > 1 && replacement_list[i-1].is_ws() && replacement_list[i-2].is_hash2_blessed())
					param_usage.insert(make_pair(param, eUnexpanded));
				else if (i+1 < replacement_list.size() && replacement_list[i+1].is_hash2_blessed())
					param_usage.insert(make_pair(param, eUnexpanded));
				else if (i+2 < replacement_list.size() && replacement_list[i+1].is_ws() && replacement_list[i+2].is_hash2_blessed())
					param_usage.insert(make_pair(param, eUnexpanded));
				else
					param_usage.insert(make_pair(param, eExpanded));
			}

//	cerr << "HELLO??" << endl;

//	for (string param : parameters)
//	{
//		if (param_usage.count(make_pair(param, eStringized)))
//			cerr << "DEBUG: " << param << " stringized";
//
//		if (param_usage.count(make_pair(param, eUnexpanded)))
//			cerr << "DEBUG: " << param << " unexpanded";
//
//		if (param_usage.count(make_pair(param, eExpanded)))
//			cerr << "DEBUG: " << param << " expanded";
//
//	}

//		dump();
}

struct MacroDefinitionParser : PreprocessingTokenParser
{
	unordered_map<string, MacroDefinition>& macros;
	MacroDefinitionParser(unordered_map<string, MacroDefinition>& macros, vector<PreprocessingToken>&& input)
		: PreprocessingTokenParser(move(input))
		, macros(macros)
	{

	}

	void do_define()
	{
		if (peek_type() != PP_IDENTIFIER)
			throw logic_error("expected identifier");

		string macro_name = pop().src.spelling;

		if (macro_name == "__VA_ARGS__")
			throw logic_error("invalid __VA_ARGS__ use");

		if (peek().is_lparen())
		{
			// function-like

			pop();

			skipws();

			vector<string> parameters;
			bool var_args = false;

			if (!peek().is_rparen())
			{
				if (peek().is_dots())
				{
					pop();
					skipws();
					var_args = true;

					if (!peek().is_rparen())
						throw logic_error("expected rparen after dots");
				}
				else if (peek_type() != PP_IDENTIFIER)
					throw logic_error("expected identifier after lparen");
				else
					parameters.push_back(pop().src.spelling);

				skipws();

				while (peek().is_comma())
				{
					pop();
					skipws();

					if (peek_type() == PP_IDENTIFIER)
					{
						parameters.push_back(pop().src.spelling);
						skipws();
					}
					else if (!peek().is_dots())
						throw logic_error("expected identifier");
				}

				if (peek().is_dots())
				{
					pop();
					skipws();
					var_args = true;
				}

				if (!peek().is_rparen())
					throw logic_error("expected rparen (#2): " + peek().to_string());
			}

			pop();
			skipws();

			vector<PreprocessingToken> replacement_list;

			while (peek_type() != PP_NEW_LINE)
				replacement_list.push_back(pop());

			while (!replacement_list.empty() && replacement_list.back().pp_type == PP_WHITESPACE)
				replacement_list.pop_back();

			auto it = macros.find(macro_name);

			MacroDefinition macro_definition(macro_name, var_args, parameters, replacement_list);

			if (it != macros.end())
			{
				if (it->second != macro_definition)
					throw logic_error("macro redefined");
			}
			else
			{
				macros.emplace(make_pair(macro_name, macro_definition));
			}
		}
		else if (peek_type() == PP_WHITESPACE || peek_type() == PP_NEW_LINE)
		{
			// object-like

			skipws();

			vector<PreprocessingToken> replacement_list;

			while (peek_type() != PP_NEW_LINE)
				replacement_list.push_back(pop());

			while (!replacement_list.empty() && replacement_list.back().pp_type == PP_WHITESPACE)
				replacement_list.pop_back();

			auto it = macros.find(macro_name);

			MacroDefinition macro_definition(macro_name, replacement_list);

			if (it != macros.end())
			{
				if (it->second != macro_definition)
					throw logic_error("macro redefined");
			}
			else
			{
				macros.emplace(make_pair(macro_name, macro_definition));
			}
		}
		else
			throw logic_error("invalid macro definition");
	}
};

void MacroTable::define(const vector<PreprocessingToken>& preprocessing_directive)
{
	vector<PreprocessingToken> tokens = preprocessing_directive;
	tokens.push_back(PreprocessingToken(SourceInfo{"\n", "", 0}, PP_NEW_LINE));

	MacroDefinitionParser parser(macros, move(tokens));
	parser.do_define();
}

void MacroTable::undef(const vector<PreprocessingToken>& preprocessing_directive)
{
	if (preprocessing_directive.size() != 1 || preprocessing_directive[0].pp_type != PP_IDENTIFIER)
		throw logic_error("#undef expected id");

	string id = preprocessing_directive[0].src.spelling;

	if (id == "__VA_ARGS__")
		throw logic_error("#undef __VA_ARGS__, invalid use of __VA_ARGS__");

	auto it = macros.find(id);

	if (it != macros.end())
		macros.erase(it);
}

void MacroTable::expand(vector<PreprocessingToken>& tokens)
{
	MacroReplacer replacer(macros, tokens, &tracker);

	replacer.replace();

	tokens = replacer.output;
}

const unordered_set<string> PredefinedMacroNames =
{
	"__CPPGM__",
	"__CPPGM_AUTHOR__",
	"__cplusplus",
	"__STDC_HOSTED__",
	"__FILE__",
	"__LINE__",
	"__DATE__",
	"__TIME__"
};

bool MacroTable::is_defined(const string& macro_name)
{
	if (macros.count(macro_name))
		return true;

	if (PredefinedMacroNames.count(macro_name))
		return true;

	return false;
}

void MacroDefinition::dump()
{
	cout << "DEBUG: Macro: " << macro_name << endl;
	cout << "Type: " << (function_like ? "function-like" : "object-like") << endl;
	if (function_like)
	{
		cout << "Parameters: ";
		for (auto x : parameters)
			cout << x << ", ";
		cout << endl;
	}
	cout << "Replacement List: ";
	for (auto x : replacement_list)
		cout << x.to_string() << ", ";
	cout << endl;
}

vector<PreprocessingToken> MacroDefinition::invoke(
	const vector<vector<PreprocessingToken>>& arguments,
	const vector<vector<PreprocessingToken>>& replaced_arguments,
	const vector<string>& stringified_arguments) const
{
	vector<PreprocessingToken> after_hash, after_subst, after_join, output;

	size_t pos = 0;

	while (pos < replacement_list.size())
	{
		if (replacement_list[pos].is_hash())
		{
			pos++;

			if (pos == replacement_list.size())
				throw logic_error("# at the end of replacement list");

			if (replacement_list[pos].is_ws())
				pos++;

			if (pos == replacement_list.size())
				throw logic_error("# at the end of replacement list");

			if (replacement_list[pos].pp_type != PP_IDENTIFIER)
				throw logic_error("expected parameter name after # in replacement list");

			string param_name = replacement_list[pos].src.spelling;

			size_t idx = get_param(param_name);

			after_hash.push_back(PreprocessingToken(SourceInfo{stringified_arguments[idx], "", 0}, PP_STRING_LITERAL));
			pos++;
		}
		else
		{
			after_hash.push_back(replacement_list[pos]);
			pos++;
		}
	}

	pos = 0;

	vector<bool> next_to_hash2(after_hash.size(), false);

	for (size_t pos_hash2 = 0; pos_hash2 < after_hash.size(); pos_hash2++)
	{
		if (!after_hash[pos_hash2].is_hash2_blessed())
			continue;

		if (pos_hash2 == 0)
			throw logic_error("## at start of replacement list (#1)");

		size_t pos_lhs = pos_hash2-1;

		if (after_hash[pos_lhs].is_ws())
		{
			if (pos_lhs == 0)
				throw logic_error("## at start of replacement list (#2)");

			pos_lhs--;
		}

		if (after_hash[pos_lhs].pp_type == PP_IDENTIFIER)
		{
			string identifier = after_hash[pos_lhs].src.spelling;

			if (has_param(identifier))
			{
				next_to_hash2[pos_lhs] = true;
			}
		}

		size_t pos_rhs = pos_hash2+1;

		if (pos_rhs >= after_hash.size())
			throw logic_error("## at end of replacement list (#1)");

		if (after_hash[pos_rhs].is_ws())
			pos_rhs++;

		if (pos_rhs >= after_hash.size())
			throw logic_error("## at end of replacement list (#2)");

		if (after_hash[pos_rhs].pp_type == PP_IDENTIFIER)
		{
			string identifier = after_hash[pos_rhs].src.spelling;

			if (has_param(identifier))
			{
				next_to_hash2[pos_rhs] = true;
			}
		}
	}

	for (size_t i = 0; i < after_hash.size(); i++)
	{
		const PreprocessingToken& token = after_hash[i];

		if (token.pp_type != PP_IDENTIFIER)
		{
			after_subst.push_back(token);
			continue;
		}

		string identifier = token.src.spelling;

		if (!has_param(identifier))
		{
			after_subst.push_back(token);
			continue;
		}

		size_t param_id = get_param(identifier);

		if (next_to_hash2[i])
		{
			if (arguments[param_id].empty())
				after_subst.push_back(PreprocessingToken(SourceInfo(), PP_PLACEMARKER));
			else
				for (const auto& x : arguments[param_id])
					after_subst.push_back(x);
		}
		else
		{
			for (const auto& x : replaced_arguments[param_id])
				after_subst.push_back(x);
		}
	}

	output = ProcessHash2(after_subst);

	return output;
}

string MacroStringify(const vector<PreprocessingToken>& input)
{
	ostringstream oss;

	oss << '"';

	for (const PreprocessingToken& token : input)
	{
		switch (token.pp_type)
		{
		case PP_WHITESPACE:
			oss << ' ';
			break;

		case PP_STRING_LITERAL:
		case PP_CHARACTER_LITERAL:
		case PP_USER_DEFINED_CHARACTER_LITERAL:
		case PP_USER_DEFINED_STRING_LITERAL:
			for (char c : token.src.spelling)
			{
				if (c == '\\')
					oss << "\\\\";
				else if (c == '"')
					oss << "\\\"";
				else
					oss << c;
			}
			break;

		default:
			oss << token.src.spelling;
			break;
		}
	}

	oss << '"';

	return oss.str();
}

MacroReplacer::MacroReplacer(const unordered_map<string, MacroDefinition>& macros, vector<PreprocessingToken> raw_input, SourcePositionTracker* tracker)
	: tracker(tracker)
	, macros(macros)
{
	reverse(raw_input.begin(), raw_input.end());
	NormalizeWhitespace(raw_input);
	CheckVarargs(raw_input);

	swap(input, raw_input);
}

const PreprocessingToken& MacroReplacer::peek(size_t i)
{
	static PreprocessingToken invalid;

	if (i >= input.size())
		return invalid;
	else
		return input[input.size() - i - 1];
}

bool MacroReplacer::check_blacklist()
{
	PreprocessingToken& id = input.back();

	if (!id.available)
		return false;

	if (id.blacklist.count(id.src.spelling) == 0)
		return true;

//	return true;

	id.available = false;

	return false;
}

void MacroReplacer::replace()
{
	while (input.size() > 0)
	{
		bool keep = false;

		if (peek(0).pp_type == PP_IDENTIFIER && peek(0).available)
		{
			string macro_name = peek(0).src.spelling;

			if (PredefinedMacroNames.count(macro_name))
			{
				if (macro_name == "__FILE__")
				{
					SourceInfo src = peek(0).src;
					src.spelling = "\"" + src.filename + "\"";

					input.pop_back();
					input.push_back(PreprocessingToken(src, PP_STRING_LITERAL));
				}
				else if (macro_name == "__LINE__")
				{
					SourceInfo src = peek(0).src;
					src.spelling = to_string(src.linenum);

					input.pop_back();
					input.push_back(PreprocessingToken(src, PP_NUMBER));
				}
				else if (macro_name == "__DATE__")
				{
					input.pop_back();
					input.push_back(PreprocessingToken(SourceInfo{BuildDate, "", 0}, PP_STRING_LITERAL));
				}
				else if (macro_name == "__TIME__")
				{
					input.pop_back();
					input.push_back(PreprocessingToken(SourceInfo{BuildTime, "", 0}, PP_STRING_LITERAL));
				}
				else if (macro_name == "__CPPGM__")
				{
					input.pop_back();
					input.push_back(PreprocessingToken(SourceInfo{"201303L", "", 0}, PP_NUMBER));
				}
				else if (macro_name == "__CPPGM_AUTHOR__")
				{
					input.pop_back();
					input.push_back(PreprocessingToken(SourceInfo{"\"Andrew Tomazos\"", "", 0}, PP_STRING_LITERAL));
				}
				else if (macro_name == "__cplusplus")
				{
					input.pop_back();
					input.push_back(PreprocessingToken(SourceInfo{"201103L", "", 0}, PP_NUMBER));
				}
				else if (macro_name == "__STDC_HOSTED__")
				{
					input.pop_back();
					input.push_back(PreprocessingToken(SourceInfo{"1", "", 0}, PP_NUMBER));
				}
			}
		}

		if (peek(0).pp_type == PP_IDENTIFIER)
		{
			string macro_name = peek(0).src.spelling;

			auto blacklist = peek(0).blacklist;
			string head_filename = peek(0).src.filename;
			size_t head_linenum = peek(0).src.linenum;

			auto it = macros.find(macro_name);

			if (it != macros.end())
			{
				const MacroDefinition& macro = it->second;

				if (macro.function_like)
				{
					if ((peek(1).is_lparen() || (peek(1).is_ws() && peek(2).is_lparen())) && check_blacklist())
					{
						keep = true;
						// function-like invocation

						while (!peek(0).is_lparen())
							input.pop_back();

						input.pop_back();

						vector<vector<PreprocessingToken>> arguments;

						while (true) // scan arguments
						{
							while (peek(0).is_ws())
								input.pop_back();

							if (!peek(0).is_valid())
								throw logic_error("could not terminate function-like macro invocation");

							vector<PreprocessingToken> argument;

							size_t depth = 0;

							while (true)
							{
								if (!peek(0).is_valid())
									throw logic_error("could not terminate function-like macro invocation");

								if (depth == 0)
								{
									size_t arg_num = arguments.size();

									bool comma_ok = (!macro.var_args || arg_num < macro.parameters.size() - 1);

									if (comma_ok && peek(0).is_comma())
										break;

									if (peek(0).is_rparen())
										break;
								}

								argument.push_back(peek(0));

								if (peek(0).is_lparen())
									depth++;

								if (peek(0).is_rparen())
									depth--;

								input.pop_back();
							}

							if (!argument.empty() && argument.back().is_ws())
								argument.pop_back();

							arguments.push_back(argument);

							if (peek(0).is_rparen())
								break;

							input.pop_back();
						}

//							cout << "DEBUG: args = " << arguments.size() << endl;

						input.pop_back();

						if (macro.var_args && arguments.size() != macro.parameters.size())
							throw logic_error("macro function-like invocation wrong num of params: " + macro_name);

						if (!macro.var_args)
						{
							if (macro.parameters.size() == 0)
							{
								if (arguments.size() > 1 || (arguments.size() == 1 && arguments[0].size() > 0))
									throw logic_error("macro function-like invocation wrong num of params: " + macro_name);
							}
							else if (arguments.size() != macro.parameters.size())
								throw logic_error("macro function-like invocation wrong num of params: " + macro_name);
						}

						vector<vector<PreprocessingToken>> replaced_arguments(arguments.size());
						vector<string> stringified_arguments(arguments.size());

						for (size_t i = 0; i < arguments.size(); i++)
						{
							auto argument = arguments[i];

							string parameter;

							if (i < macro.parameters.size())
								parameter = macro.parameters[i];

							if (macro.param_usage.count(make_pair(parameter, MacroDefinition::eExpanded)))
							{
								MacroReplacer replacer(macros, argument, tracker);

								replacer.replace();

								replaced_arguments[i] = replacer.output;
							}

							if (macro.param_usage.count(make_pair(parameter, MacroDefinition::eStringized)))
								stringified_arguments[i] = MacroStringify(argument);
						}

						vector<PreprocessingToken> replacement_list = macro.invoke(arguments, replaced_arguments, stringified_arguments);

						for (auto& x : replacement_list)
							if (x.pp_type == PP_IDENTIFIER)
							{
								x.src.filename = head_filename;
								x.src.linenum = head_linenum;

								x.blacklist = blacklist;
								x.blacklist.insert(macro_name);

//									if (x.blacklist.count(x.source))
//										x.available = false; // TODO !!!
							}

						for (auto jt = replacement_list.rbegin(); jt != replacement_list.rend(); jt++)
							input.push_back(*jt);
					}
				}
				else if (check_blacklist())
				{
					keep = true;
					// object-like invocation

					input.pop_back();

					vector<PreprocessingToken> replacement_list = ProcessHash2(macro.replacement_list);

					for (auto& x : replacement_list)
						if (x.pp_type == PP_IDENTIFIER)
						{
							x.src.filename = head_filename;
							x.src.linenum = head_linenum;

							x.blacklist = blacklist;
							x.blacklist.insert(macro_name);
//								if (x.blacklist.count(x.source))
//									x.available = false; // TODO !!!

						}

					for (auto jt = replacement_list.rbegin(); jt != replacement_list.rend(); jt++)
						input.push_back(*jt);
				}
			}
		}

		if (!keep)
		{
			output.push_back(peek(0));
			input.pop_back();
		}
	}
}
