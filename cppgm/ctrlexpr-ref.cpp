#include "std.pch"

#include "CtrlExprInputStream.h"
#include "PPTokenize.h"

string VERSION = "ctrlexpr-ref 3.0.13";

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

		CtrlExprInputStream x;

		PPTokenize(input, x);
	}
	catch (exception& e)
	{
		cerr << "ERROR: " << e.what() << endl;
		return EXIT_FAILURE;
	}
}
