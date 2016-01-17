#include "std.pch"

#include "Declarations.h"

#include "Types.h"

Expression* CopyInitializer::create_initializer_expression(SymbolTable& symtab, Type* destination)
{
	Expression* expr = expression->annotate(symtab);

	return destination->convert(expr);
}
