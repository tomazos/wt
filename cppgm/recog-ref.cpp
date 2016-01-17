#include "std.pch"

#include "PostTokenInputStream.h"
#include "DebugPostTokenOutputStream.h"
#include "PPTokenize.h"
#include "Preprocessor.h"
#include "MacroReplacement.h"
#include "CppParser.h"
#include "TokenSequencePostTokenOutputStream.h"

string VERSION = "recog-ref 5.0.13";

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

		bool trace = false;

		for (size_t i = 0; i < args.size(); i++)
		{
			if (args[i] == "--stdinc")
				SearchStdInc = true;
			else if (args[i] == "--trace" || args[i] == "-t")
				trace = true;
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

		out << "recog " << srcfiles.size() << endl;

		SourcePositionTracker tracker;

		for (auto srcfile : srcfiles)
		{
			out << srcfile << " ";

			try
			{
				TokenSequencePostTokenOutputStream output;

				Preprocess(srcfile, output, tracker);

				CppParse(move(output.tokens), trace);

				out << "OK" << endl;
			}
			catch (exception& e)
			{
				cout << "ERROR: " << e.what() << endl;
				out << "BAD" << endl;
			}
		}
	}
	catch (exception& e)
	{
		cerr << "ERROR: " << e.what() << endl;
		return EXIT_FAILURE;
	}
}
