#include "std.pch"

#include "Preprocessor.h"
#include "PPTokenize.h"
#include "MacroReplacement.h"
#include "PreprocessingTokenParser.h"
#include "CtrlExprParser.h"
#include "DebugPostTokenOutputStream.h"
#include "PostTokenInputStream.h"

bool SearchStdInc = false;

// OPTIONAL: Also search `PA5StdIncPaths` on `--stdinc` command-line switch
vector<string> PA5StdIncPaths =
{
    "/usr/include/c++/4.7/",
    "/usr/include/c++/4.7/x86_64-linux-gnu/",
    "/usr/include/c++/4.7/backward/",
    "/usr/lib/gcc/x86_64-linux-gnu/4.7/include/",
    "/usr/local/include/",
    "/usr/lib/gcc/x86_64-linux-gnu/4.7/include-fixed/",
    "/usr/include/x86_64-linux-gnu/",
    "/usr/include/"
};

struct Preprocessor : PreprocessingTokenParser
{
	unordered_map<string, MacroDefinition> macros;

	vector<PreprocessingToken> output;

	Preprocessor(vector<PreprocessingToken>&& input)
		: PreprocessingTokenParser(move(input))
	{

	}

	void preprocess()
	{
		while (true)
		{
			skipws();

			if (peek_type() == PP_EOF)
			{
				output.push_back(pop());
				return;
			}
			if (peek().is_hash())
				process_directive();
			else
				process_text();
		}
	}

	void process_directive()
	{
		pos++;

		skipws();

		auto directive_start = pos;

		while (peek_type() != PP_NEW_LINE)
			pos++;

		auto directive_end = pos;

		if (directive_start == directive_end)
			throw logic_error("empty preprocessing directive");

		if (peek_type(directive_start) != PP_IDENTIFIER)
			throw logic_error("non-identifier preprocessing directive");

		string directive = peek(directive_start).src.spelling;

		if (directive == "define")
		{
			pos = directive_start + 1;

			skipws();

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
		else if (directive == "undef")
		{
			pos = directive_start + 1;

			skipws();

			if (peek_type() != PP_IDENTIFIER)
				throw logic_error("expected identifier");

			string macro_name = pop().src.spelling;

			if (macro_name == "__VA_ARGS__")
				throw logic_error("invalid __VA_ARGS__ use");

			skipws();

			if (peek_type() != PP_NEW_LINE)
				throw logic_error("expected new line");

			pop();

			skipws();

			auto it = macros.find(macro_name);

			if (it != macros.end())
				macros.erase(it);
		}
		else
			throw logic_error("unknown preprocessing directive");
	}

	void process_text()
	{
		vector<PreprocessingToken> text;

		while (true)
		{
			while (peek_type() != PP_NEW_LINE)
				text.push_back(pop());

			text.push_back(pop());

			while (pos < input.size() && input[pos].pp_type == PP_WHITESPACE)
			{
				text.push_back(input[pos]);
				pos++;
			}

			if (peek().is_hash() || peek_type() == PP_EOF)
				break;
		}

		MacroReplacer replacer(macros, move(text), nullptr);

		replacer.replace();

		for (auto token : replacer.output)
			output.push_back(token);
	}

};

void Preprocess(vector<PreprocessingToken>& input)
{
	Preprocessor preprocessor(move(input));
	preprocessor.preprocess();

	input = move(preprocessor.output);
}

// system-wide unique file id
typedef pair<unsigned long int, unsigned long int> PA5FileId;

// bootstrap system call interface
extern "C" long int syscall(long int n, ...) throw ();

// PA5GetFileId returns true iff file found at path `path`.
// out parameter `out_fileid` is set to file id
bool PA5GetFileId(const string& path, PA5FileId& out_fileid)
{
	struct
	{
			unsigned long int dev;
			unsigned long int ino;
			long int unused[16];
	} data;

	int res = syscall(4, path.c_str(), &data);

	out_fileid = make_pair(data.dev, data.ino);

	return res == 0;
}

struct PreprocessorStream :  IPPTokenStream
{
	SourcePositionTracker& tracker;
	MacroTable macro_table;
	vector<string> current_file;
	set<PA5FileId> already_included;

	IPPTokenStream& output;

	PreprocessorStream(SourcePositionTracker& tracker, IPPTokenStream& output)
		: tracker(tracker)
		, macro_table(tracker)
		, output(output)
	{}

	vector<PreprocessingToken> tokens;

	enum GroupState
	{
		fileroot,
		if_future,
		if_active,
		if_past,
		if_inactive,
		else_active,
		else_past,
		else_inactive
	};

	vector<GroupState> group_state = vector<GroupState>{fileroot};

	bool active()
	{
		if (group_state.empty())
			throw logic_error("group state empty");

		switch (group_state.back())
		{
		case fileroot:
		case if_active:
		case else_active:
			return true;

		case if_past:
		case if_future:
		case if_inactive:
		case else_past:
		case else_inactive:
			return false;
		default:
			throw logic_error("internal error: unexpected PreprocessorStream state (#1): " + to_string(int(group_state.back())));
		}
	}

	bool inactive()
	{
		return !active();
	}

	bool eval_ctrlexpr()
	{
		for (size_t i = 0; i < tokens.size(); i++)
			if (tokens[i].src.spelling == "defined")
			{
				tokens[i].available = false;
				for (size_t j = i+1; j < tokens.size(); j++)
					if (tokens[j].pp_type == PP_IDENTIFIER)
					{
						tokens[j].available = false;
						break;
					}
			}

//		cerr << "DEBUG: eval_ctrlexpr: " << PpTokensToStr(tokens) << endl;

		macro_table.expand(tokens);

		function<bool(const string&)> f = [&](const string& macro_name) { return macro_table.is_defined(macro_name); };
		CtrlExprParser parser(CtrlExpr_ConvertPreprocessingTokens(tokens),  f);
		CtrlExprVal val = parser.parse();
		if (val.is_error)
			throw logic_error("invalid controlling expression");
		return val.eval_bool();
	}

	void do_if()
	{
		if (inactive())
			group_state.push_back(if_inactive);
		else
		{
			if (eval_ctrlexpr())
				group_state.push_back(if_active);
			else
				group_state.push_back(if_future);
		}
	}

	void do_ifdef()
	{
		if (inactive())
		{
			group_state.push_back(if_inactive);
			return;
		}

		if (tokens.size() != 1 || tokens[0].pp_type != PP_IDENTIFIER)
			throw logic_error("ifdef expected identifier");

		tokens.insert(tokens.begin(), PreprocessingToken(SourceInfo{"defined","",0}, PP_IDENTIFIER));

		do_if();
	}

	void do_ifndef()
	{
		if (inactive())
		{
			group_state.push_back(if_inactive);
			return;
		}

		if (tokens.size() != 1 || tokens[0].pp_type != PP_IDENTIFIER)
			throw logic_error("ifndef expected identifier");

		tokens.insert(tokens.begin(), PreprocessingToken(SourceInfo{"defined","",0}, PP_IDENTIFIER));
		tokens.insert(tokens.begin(), PreprocessingToken(SourceInfo{"!","",0}, PP_OP_OR_PUNC));

		do_if();
	}

	void do_elif()
	{
		switch (group_state.back())
		{
		case fileroot:
		case else_active:
		case else_past:
		case else_inactive:
			throw logic_error("unexpected #elif");

		case if_inactive:
			break;

		case if_future:
			if (eval_ctrlexpr())
				group_state.back() = if_active;
			else
				group_state.back() = if_future;
			break;

		case if_active:
		case if_past:
			eval_ctrlexpr();
			group_state.back() = if_past;
			break;

		default:
			throw logic_error("internal error: unexpected PreprocessorStream state (#4): " + to_string(int(group_state.back())));
		};
	}

	void do_else()
	{
		switch (group_state.back())
		{
		case fileroot:
		case else_active:
		case else_past:
		case else_inactive:
			throw logic_error("unexpected #else");

		case if_future:
			if (tokens.size() > 0)
				throw logic_error("#else with tokens after it");
			group_state.back() = else_active;
			break;

		case if_active:
		case if_past:
			if (tokens.size() > 0)
				throw logic_error("#else with tokens after it");
			group_state.back() = else_inactive;
			break;

		case if_inactive:
			group_state.back() = else_inactive;
			break;

		default:
			throw logic_error("internal error: unexpected PreprocessorStream state (#2): " + to_string(int(group_state.back())));
		};

	}

	void do_endif()
	{
		switch (group_state.back())
		{
		case fileroot:
			throw logic_error("unexpected #endif");

		case if_future:
		case if_active:
		case if_past:
		case else_past:
		case else_active:
			if (tokens.size() > 0)
				throw logic_error("#endif with tokens after it");
			// fallthrough

		case if_inactive:
		case else_inactive:
			group_state.pop_back();
			break;

		default:
			throw logic_error("internal error: unexpected PreprocessorStream state (#3): " + to_string(int(group_state.back())));
		};
	}

	void do_include()
	{
		macro_table.expand(tokens);

		NormalizeWhitespace(tokens);

		if (tokens.size() != 1)
			throw logic_error("invalid include: " + MacroStringify(tokens));

		string pathf;
		if (tokens[0].pp_type == PP_HEADER_NAME)
		{
			string header_name = tokens[0].src.spelling;

			if (header_name.size() < 3)
				throw logic_error("invalid header-name: " + header_name);

			pathf = header_name.substr(1, header_name.size() - 2);
		}
		else if (tokens[0].pp_type == PP_STRING_LITERAL)
			pathf = StrFromStringLiteral(tokens[0].src);

		if (pathf.empty())
			throw logic_error("empty include filename");

		vector<string> search_files;

		search_files.push_back(tracker.calc_rel_top(pathf));
		search_files.push_back(pathf);

		if (SearchStdInc)
		{
			for (string s : PA5StdIncPaths)
				search_files.push_back(CalcRelFilePath(s, pathf));
		}

//		for (string fn : search_files)
//			cout << "DEBUG: " << fn << endl;

		bool found = false;
		for (string fn : search_files)
		{
			PA5FileId id;
			if (!PA5GetFileId(fn, id))
				continue;

			found = true;

			if (already_included.count(id))
				break;

			ifstream in(fn);

			if (!in.good())
				throw logic_error("unable to open: " + fn);

			tracker.push_include(fn);
			group_state.push_back(fileroot);

			size_t gsentersize =group_state.size();

			tokens.clear();
			state = start;

			ostringstream oss;
			oss << in.rdbuf();
			string input = oss.str();

			PPTokenize(input, *this, tracker);

			if (gsentersize != group_state.size())
				throw logic_error("include completed in bad group state (maybe unterminated #if)");

			group_state.pop_back();
			tracker.pop_include();
			break;
		}

		if (!found)
			throw logic_error("include not found: " + pathf);
	}

	void do_define()
	{
		macro_table.define(tokens);
	}

	void do_undef()
	{
		macro_table.undef(tokens);
	}

	void do_line()
	{
		macro_table.expand(tokens);

		if (tokens.empty())
			throw logic_error("empty #line directive");

		auto x = ValFromPpNumber(tokens[0]);

		size_t pos = 1;

		if (pos < tokens.size() && tokens[pos].pp_type == PP_WHITESPACE)
			pos++;

		if (pos == tokens.size())
		{
			tracker.set_line(x.u-1);
			return;
		}

		tracker.set_line(x.u-1, StrFromStringLiteral(tokens[pos].src));
	}

	void pragma_once_file()
	{
		PA5FileId id;
		if (!PA5GetFileId(tracker.get_file_str(), id))
			throw logic_error("unable to find file for pragma once: " + tracker.get_file_str());

		already_included.insert(id);
	}

	void do_pragma()
	{
		if (tokens.size() != 1 || tokens[0].src.spelling != "once")
			return;

		pragma_once_file();
	}

	void do_preprocessing_directive()
	{
//		dump_state("---> do_preprocessing_directive");

		NormalizeWhitespace(tokens);

		if (tokens.empty())
			return;

		if (!tokens[0].is_hash())
			throw logic_error("internal error: hash at start expected");

		if (tokens.size() == 1)
		{
			tokens.clear();
			return;
		}

		size_t name_pos = 1;

		if (tokens[name_pos].pp_type == PP_WHITESPACE)
			name_pos++;

		string directive_name = tokens[name_pos].src.spelling;

		tokens.erase(tokens.begin(), tokens.begin() + name_pos + 1);

		NormalizeWhitespace(tokens);

		if (directive_name == "if")
			do_if();
		else if (directive_name == "ifdef")
			do_ifdef();
		else if (directive_name == "ifndef")
			do_ifndef();
		else if (directive_name == "elif")
			do_elif();
		else if (directive_name == "else")
			do_else();
		else if (directive_name == "endif")
			do_endif();
		else if (active())
		{
			if (directive_name == "include")
				do_include();
			else if (directive_name == "define")
				do_define();
			else if (directive_name == "undef")
				do_undef();
			else if (directive_name == "line")
				do_line();
			else if (directive_name == "error")
				throw logic_error("#error " + MacroStringify(tokens));
			else if (directive_name == "pragma")
				do_pragma();
			else
				throw logic_error("active non-directive found: " + directive_name);
		}

		tokens.clear();

//		dump_state("<--- do_preprocessing_directive");
	}

	void do_text_sequence()
	{
//		dump_state("---> do_text_sequence");

		if (inactive())
		{
			tokens.clear();
			return;
		}

		NormalizeWhitespace(tokens);

		if (tokens.empty())
			return;

		CheckVarargs(tokens);

		macro_table.expand(tokens);

		vector<PreprocessingToken> newtokens;

		size_t pos = 0;

		while (pos < tokens.size())
		{
			if (tokens[pos].pp_type != PP_IDENTIFIER || tokens[pos].src.spelling != "_Pragma")
			{
				newtokens.push_back(tokens[pos]);
				pos++;
				continue;
			}

			pos++;

			while (pos < tokens.size() && tokens[pos].pp_type == PP_WHITESPACE)
				pos++;

			if (pos == tokens.size() || !tokens[pos].is_lparen())
				throw logic_error("expected ( after _Pragma: " + tokens[pos].src.spelling);

			pos++;

			while (pos < tokens.size() && tokens[pos].pp_type == PP_WHITESPACE)
				pos++;

			if (pos == tokens.size() || tokens[pos].pp_type != PP_STRING_LITERAL)
				throw logic_error("expected string literal after _Pragma(");

			string strlit = tokens[pos].src.spelling;

			pos++;

			while (pos < tokens.size() && tokens[pos].pp_type == PP_WHITESPACE)
				pos++;

			if (pos < tokens.size() && !tokens[pos].is_rparen())
				throw logic_error("expected ) after _Pragma(\"...\"");

			pos++;

			static unordered_set<string> once_strings = { "\"once\"", "u8\"once\"", "u\"once\"", "U\"once\"", "L\"once\"" };

			if (once_strings.count(strlit))
				pragma_once_file();
		}

		swap(tokens, newtokens);

		WritePPTokens(tokens, output);

		tokens.clear();

//		dump_state("<--- do_text_sequence");
	}

	enum
	{
		start,
		in_preprocessing_directive,
		in_text_sequence

	} state = start;

	void emit_whitespace_sequence()
	{
		tokens.push_back(PreprocessingToken(SourceInfo(), PP_WHITESPACE));
	}

	void dump_state(const string& msg)
	{
		cerr << "DEBUG: " << msg << " STATE: ";
		switch (state)
		{
		case start: cerr << "start"; break;
		case in_preprocessing_directive: cerr << "in_preprocessing_directive"; break;
		case in_text_sequence: cerr << "in_text_sequence"; break;
		}
		cerr << " " << PpTokensToStr(tokens) << endl;
	}

	void emit_new_line()
	{
//		dump_state("---> emit_new_line");

		tokens.push_back(PreprocessingToken(SourceInfo(), PP_NEW_LINE));

		switch (state)
		{
		case in_preprocessing_directive:
			do_preprocessing_directive();
			state = start;
			break;

		case in_text_sequence:
			state = start;
			break;
		}
		state = start;

//		dump_state("<--- emit_new_line");
	}

	void enter_text_sequence()
	{
		if (state == start)
		{
//			dump_state("---> enter_text_sequence");
			state = in_text_sequence;
//			dump_state("<--- enter_text_sequence");
		}
	}

	void emit_header_name(SourceInfo src)
	{
		src.filename = tracker.get_pos().filename;
		src.linenum = tracker.get_pos().linenum;

		tokens.push_back(PreprocessingToken(src, PP_HEADER_NAME));

		enter_text_sequence();
	}

	void emit_identifier(SourceInfo src)
	{
		src.filename = tracker.get_pos().filename;
		src.linenum = tracker.get_pos().linenum;
		tokens.push_back(PreprocessingToken(src, PP_IDENTIFIER));
		enter_text_sequence();
	}

	void emit_pp_number(SourceInfo src)
	{
		src.filename = tracker.get_pos().filename;
		src.linenum = tracker.get_pos().linenum;
		tokens.push_back(PreprocessingToken(src, PP_NUMBER));
		enter_text_sequence();
	}

	void emit_character_literal(SourceInfo src)
	{
		src.filename = tracker.get_pos().filename;
		src.linenum = tracker.get_pos().linenum;
		tokens.push_back(PreprocessingToken(src, PP_CHARACTER_LITERAL));
		enter_text_sequence();
	}

	void emit_user_defined_character_literal(SourceInfo src)
	{
		src.filename = tracker.get_pos().filename;
		src.linenum = tracker.get_pos().linenum;
		tokens.push_back(PreprocessingToken(src, PP_USER_DEFINED_CHARACTER_LITERAL));
		enter_text_sequence();
	}

	void emit_string_literal(SourceInfo src)
	{
		src.filename = tracker.get_pos().filename;
		src.linenum = tracker.get_pos().linenum;
		tokens.push_back(PreprocessingToken(src, PP_STRING_LITERAL));
		enter_text_sequence();
	}

	void emit_user_defined_string_literal(SourceInfo src)
	{
		src.filename = tracker.get_pos().filename;
		src.linenum = tracker.get_pos().linenum;
		tokens.push_back(PreprocessingToken(src, PP_USER_DEFINED_STRING_LITERAL));
		enter_text_sequence();
	}

	void emit_preprocessing_op_or_punc(SourceInfo src)
	{
//		if (data == "#")
//		{
//			dump_state("--> hash");
//		}

		src.filename = tracker.get_pos().filename;
		src.linenum = tracker.get_pos().linenum;
		PreprocessingToken token(src, PP_OP_OR_PUNC);
		if (state == start)
		{
			if (token.is_hash())
			{
				do_text_sequence();
				state = in_preprocessing_directive;
			}
			else
				state = in_text_sequence;
		}

		tokens.push_back(token);

//		if (data == "#")
//		{
//			dump_state("<-- hash");
//		}
	}

	void emit_non_whitespace_char(SourceInfo src)
	{
		tokens.push_back(PreprocessingToken(src, PP_NONWHITESPACE_CHAR));
		enter_text_sequence();
	}

	void emit_eof()
	{
//		dump_state("---> eof");

		do_text_sequence();

		state = start;
//		dump_state("<--- eof");
	}
};

void Preprocess(const string& srcfile, IPostTokenOutputStream& outfile, SourcePositionTracker& tracker)
{
	PostTokenInputStream mid(outfile);

	PreprocessorStream preprocessor(tracker, mid);

	ifstream in(srcfile);

	if (!in.good())
		throw logic_error("unable to open: " + srcfile);

	tracker.push_include(srcfile);

	ostringstream oss;
	oss << in.rdbuf();
	string input = oss.str();

	PPTokenize(input, preprocessor, tracker);

	if (preprocessor.group_state.size() != 1)
		throw logic_error("bad group state on exit, maybe unterminated #if ?");

	mid.emit_eof();

	tracker.pop_include();
}
