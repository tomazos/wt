#include "std.pch"

#include "Names.h"

#include "Types.h"

bool LinkerName::operator<(const LinkerName& that) const
{
	if (name < that.name)
		return true;
	else if (name > that.name)
		return false;

	if (!extra && !that.extra)
		return false;

	if (extra && that.extra)
		return extra->less_signature(that.extra);

	throw logic_error("function name mixed with non-function name");
}
