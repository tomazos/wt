#pragma once

#include "PreprocessingToken.h"
#include "IPostTokenOutputStream.h"

void Preprocess(vector<PreprocessingToken>& input);

void Preprocess(const string& srcfile, IPostTokenOutputStream& outfile,
                SourcePositionTracker& tracker);

extern bool SearchStdInc;
