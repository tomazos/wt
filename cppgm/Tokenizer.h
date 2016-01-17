#pragma once

#include "Tokens.h"

struct PPStringLiteral
{
	bool is_ud;
	SourceInfo src;
};

PToken TokenizePPNumber(SourceInfo src);
PToken TokenizeCharacterLiteral(SourceInfo src);
PToken TokenizeUserDefinedCharacterLiteral(SourceInfo src);
PToken TokenizeStringLiteral(SourceInfo src);
PToken TokenizeUserDefinedStringLiteral(SourceInfo src);

PToken TokenizeStringLiteralSequence(SourceInfo src, const vector<PPStringLiteral>& literals);
