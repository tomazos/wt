#include "std.pch"

#include "PostTokenInputStream.h"
#include "DebugPostTokenOutputStream.h"
#include "PPTokenize.h"

string VERSION = "posttoken-ref 2.0.13";

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

		DebugPostTokenOutputStream output;

		PostTokenInputStream mid(output);

		PPTokenize(input, mid);
	}
	catch (exception& e)
	{
		cerr << "ERROR: " << e.what() << endl;
		return EXIT_FAILURE;
	}
}
