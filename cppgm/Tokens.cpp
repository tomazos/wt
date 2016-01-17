#include "std.pch"

#include "Tokens.h"

#include "Types.h"

Type* LiteralToken::get_type()
{
	if (is_array)
		return new ArrayType(new FundamentalType(fundamental_type), num_elements);
	else
		return new FundamentalType(fundamental_type);
}
