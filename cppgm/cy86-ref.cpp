#include "std.pch"

#include "Pooled.h"
#include "PostTokenInputStream.h"
#include "DebugPostTokenOutputStream.h"
#include "PPTokenize.h"
#include "Preprocessor.h"
#include "MacroReplacement.h"
#include "CppParser.h"
#include "TokenSequencePostTokenOutputStream.h"
#include "CY86AsmParser.h"

string VERSION = "cy86-ref 9.0.13";

// bootstrap system call interface, used by PA9SetFileExecutable
extern "C" long int syscall(long int n, ...) throw ();

// PA9SetFileExecutable: sets file at `path` executable
// returns true on success
bool PA9SetFileExecutable(const string& path)
{
	int res = syscall(/* chmod */ 90, path.c_str(), 0755);

	return res == 0;
}

int main(int argc, char** argv)
{
	pair<string, size_t> error_loc;

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


		SourcePositionTracker tracker;
		CY86Asm toyasm;

		try
		{
			for (auto srcfile : srcfiles)
			{
				TokenSequencePostTokenOutputStream output;

				Preprocess(srcfile, output, tracker);

				toyasm.parse(srcfile, move(output.tokens));
			}
		}
		catch (exception& e)
		{
			error_loc = toyasm.error_loc();

			throw;
		}

		{
			ofstream out(outfile, ios::binary);
			toyasm.output_program(out);
		}
		if (!PA9SetFileExecutable(outfile))
			throw logic_error("unable to set outfile executable");
	}
	catch (exception& e)
	{
		cerr << error_loc.first << ":" << error_loc.second << ":1: error: "<< e.what() << endl;
		return EXIT_FAILURE;
	}

	Pooled::delete_all();
}
