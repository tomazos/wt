#include "std.pch"

#include "PostTokenInputStream.h"
#include "DebugPostTokenOutputStream.h"
#include "PPTokenize.h"
#include "Preprocessor.h"
#include "MacroReplacement.h"

string VERSION = "preproc-ref 5.0.13";

struct FilePostTokenOutputStream : IPostTokenOutputStream
{
	FilePostTokenOutputStream(ostream& out)
		: out(out)
	{}

	ostream& out;

	virtual void emit_invalid(SourceInfo src)
	{
		throw logic_error("invalid token: " + src.spelling);
	}

	virtual void emit_simple(SourceInfo src, ETokenType token_type)
	{
		out << "simple " << src.spelling << " " << SimpleTokenTypeToString(token_type) << endl;
	}

	virtual void emit_identifier(SourceInfo src)
	{
		out << "identifier " << src.spelling << endl;
	}

	virtual void emit_literal(SourceInfo src, EFundamentalType type, void* data, size_t nbytes)
	{
		out << "literal " << src.spelling << " " << FundamentalTypeToString(type) << " " << HexDump(data, nbytes) << endl;
	}

	virtual void emit_literal_array(SourceInfo src, size_t num_elements, EFundamentalType type, void* data, size_t nbytes)
	{
		out << "literal " << src.spelling << " array of " << num_elements << " " << FundamentalTypeToString(type) << " " << HexDump(data, nbytes) << endl;
	}

	virtual void emit_user_defined_literal_character(SourceInfo src, const string& ud_suffix, EFundamentalType type, void* data, size_t nbytes)
	{
		out << "user-defined-literal " << src.spelling << " " << ud_suffix << " character " << FundamentalTypeToString(type) << " " << HexDump(data, nbytes) << endl;
	}

	virtual void emit_user_defined_literal_string_array(SourceInfo src, const string& ud_suffix, size_t num_elements, EFundamentalType type, void* data, size_t nbytes)
	{
		out << "user-defined-literal " << src.spelling << " " << ud_suffix << " string array of " << num_elements << " " << FundamentalTypeToString(type) << " " << HexDump(data, nbytes) << endl;
	}

	virtual void emit_user_defined_literal_integer(SourceInfo src, const string& ud_suffix, const string& prefix)
	{
		out << "user-defined-literal " << src.spelling << " " << ud_suffix << " integer " << prefix << endl;
	}

	virtual void emit_user_defined_literal_floating(SourceInfo src, const string& ud_suffix, const string& prefix)
	{
		out << "user-defined-literal " << src.spelling  << " " << ud_suffix << " floating " << prefix << endl;
	}

	virtual void emit_eof()
	{
		out << "eof" << endl;
	}
};


int main(int argc, char** argv)
{
	try
	{
		SetBuildDate();

		if (argc == 2 && string(argv[1]) == "-v")
		{
			cout << VERSION << endl;
			return EXIT_FAILURE;
		}

		vector<string> args;

		for (size_t i = 1; i < argc; i++)
			args.emplace_back(argv[i]);

		string outfile;
		vector<string> srcfiles;

		for (size_t i = 0; i < args.size(); i++)
		{
			if (args[i] == "--stdinc")
				SearchStdInc = true;
			else if (args[i] == "-o")
			{
				if (i == args.size() - 1)
					throw logic_error("-o at end");

				outfile = args[i+1];
				i++;
			}
			else
				srcfiles.push_back(args[i]);
		}

		if (outfile.empty())
			throw logic_error("specify -o <outfile>");

		if (srcfiles.empty())
			throw logic_error("specify one of more srcfiles");

		ofstream out(outfile);
		FilePostTokenOutputStream output(out);

		out << "preproc " << srcfiles.size() << endl;

		SourcePositionTracker tracker;

		for (auto srcfile : srcfiles)
		{
			out << "sof " << srcfile << endl;

			Preprocess(srcfile, output, tracker);
		}
	}
	catch (exception& e)
	{
		cerr << "ERROR: " << e.what() << endl;
		return EXIT_FAILURE;
	}
}
