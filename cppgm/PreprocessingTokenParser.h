#pragma once

#include "PreprocessingToken.h"

struct PreprocessingTokenParser
{
	PreprocessingTokenParser(vector<PreprocessingToken> input)
		: input(input)
	{}

	vector<PreprocessingToken> input;
	size_t pos = 0;

	const PreprocessingToken& peek()
	{
		static PreprocessingToken invalid;

		if (pos < input.size())
			return input[pos];
		else
			return invalid;
	}

	PreprocessingToken pop()
	{
		PreprocessingToken x = peek();
		pos++;
		return x;
	}

	EPreprocessingTokenType peek_type()
	{
		return peek().pp_type;
	}

	const PreprocessingToken& peek(size_t i)
	{
		static PreprocessingToken invalid;

		if (i < input.size())
			return input[i];
		else
			return invalid;
	}

	EPreprocessingTokenType peek_type(size_t i)
	{
		return peek(i).pp_type;
	}


	void skipws()
	{
		while (pos < input.size() && input[pos].pp_type == PP_WHITESPACE)
			pos++;
	}

};
