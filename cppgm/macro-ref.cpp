#include "std.pch"

#include "PreprocessingToken.h"
#include "PPTokenize.h"
#include "Preprocessor.h"
#include "PostTokenInputStream.h"
#include "DebugPostTokenOutputStream.h"

string VERSION = "macro-ref 4.0.13";

int main(int argc, char** argv)
{
	if (argc == 2 && string(argv[1]) == "-v")
	{
		cout << VERSION << endl;
		return EXIT_FAILURE;
	}

	try
	{
		ostringstream oss;
		oss << cin.rdbuf();

		string input = oss.str();

		PreprocesingTokenPPTokenStream x;

		PPTokenize(input, x);

		Preprocess(x.tokens);

		DebugPostTokenOutputStream output;

		PostTokenInputStream mid(output);

		WritePPTokens(x.tokens, mid);
	}
	catch (exception& e)
	{
		cerr << "ERROR: " << e.what() << endl;
		return EXIT_FAILURE;
	}
}
