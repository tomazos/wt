#pragma once

#include "IPPTokenStream.h"
#include "SourcePositionTracker.h"

void PPTokenize(const string& input, IPPTokenStream& output);
void PPTokenize(const string& input, IPPTokenStream& output, SourcePositionTracker& tracker);
