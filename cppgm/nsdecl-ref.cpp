#include "std.pch"

#include "Pooled.h"
#include "PostTokenInputStream.h"
#include "DebugPostTokenOutputStream.h"
#include "PPTokenize.h"
#include "Preprocessor.h"
#include "MacroReplacement.h"
#include "CppParser.h"
#include "TokenSequencePostTokenOutputStream.h"
#include "SemC.h"

string VERSION = "nsdecl-ref 7.0.13";

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

		ofstream out(outfile);

		SourcePositionTracker tracker;
		SemC semc;

		try
		{
			for (auto srcfile : srcfiles)
			{
				TokenSequencePostTokenOutputStream output;

				Preprocess(srcfile, output, tracker);

				semc.parse(srcfile, move(output.tokens));
			}
		}
		catch (exception& e)
		{
			error_loc = semc.error_loc();

			throw;
		}

		semc.dump(out);
	}
	catch (exception& e)
	{
		cerr << error_loc.first << ":" << error_loc.second << ":1: error: "<< e.what() << endl;
		return EXIT_FAILURE;
	}

	Pooled::delete_all();
}
