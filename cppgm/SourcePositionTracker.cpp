#include "std.pch"

#include "SourcePositionTracker.h"

string CalcRelFilePath(const string& base, const string& path)
{
	if (path.empty())
		throw logic_error("rel path empty");

	if (path[0] == '/')
		return path;

	auto x = base.rfind('/');

	if (x == string::npos)
		return path;
	else
		return base.substr(0,x+1) + path;
}
