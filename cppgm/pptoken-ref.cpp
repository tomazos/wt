#include "std.pch"

#include "DebugPPTokenStream.h"
#include "PPTokenize.h"

string VERSION = "pptoken-ref 1.0.13";

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

		DebugPPTokenStream output;

		PPTokenize(input, output);
	}
	catch (exception& e)
	{
		cerr << "ERROR: " << e.what() << endl;
		return EXIT_FAILURE;
	}
}
