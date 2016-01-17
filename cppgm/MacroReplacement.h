// (C) 2013, Andrew Tomazos <andrew@tomazos.com>.  All Rights Reserved.

#pragma once

#include "PreprocessingToken.h"

struct MacroDefinition
{
	bool function_like;
	string macro_name;
	vector<PreprocessingToken> replacement_list;

	bool var_args;
	vector<string> parameters;

	enum EMacroParamUsage
	{
		eStringized,
		eUnexpanded,
		eExpanded
	};

	set<pair<string, EMacroParamUsage>> param_usage;

	MacroDefinition(const string& macro_name, const vector<PreprocessingToken>& replacement_list);
	MacroDefinition(const string& macro_name, bool var_args, const vector<string>& parameters,
	                const vector<PreprocessingToken>& replacement_list);

	vector<PreprocessingToken> invoke(
		const vector<vector<PreprocessingToken>>& arguments,
		const vector<vector<PreprocessingToken>>& replaced_arguments,
		const vector<string>& stringified_arguments) const;

	void dump();

	void bless()
	{
		for (auto& x : replacement_list)
			x.blessed = true;
	}

	bool operator==(const MacroDefinition& that) const
	{
		return function_like == that.function_like &&
			macro_name == that.macro_name &&
			replacement_list == that.replacement_list &&
			var_args == that.var_args &&
			parameters == that.parameters;
	}

	bool operator!=(const MacroDefinition& that) const
	{
		return !(*this == that);
	}

	size_t get_param(const string& id) const
	{
		auto it = find(parameters.begin(), parameters.end(), id);

		if (it == parameters.end())
			throw logic_error("parameter not found: " + id);

		return it - parameters.begin();
	}

	bool has_param(const string& id) const
	{
		auto it = find(parameters.begin(), parameters.end(), id);

		return it != parameters.end();
	}

};

struct MacroTable
{
	SourcePositionTracker& tracker;

	MacroTable(SourcePositionTracker& tracker) : tracker(tracker) {}

	void define(const vector<PreprocessingToken>& preprocessing_directive);
	void undef(const vector<PreprocessingToken>& preprocessing_directive);
	void expand(vector<PreprocessingToken>& input);
	bool is_defined(const string& macro_name);

	unordered_map<string, MacroDefinition> macros;
};

struct MacroReplacer
{
	SourcePositionTracker* tracker;
	const unordered_map<string, MacroDefinition>& macros;

	vector<PreprocessingToken> input;
	vector<PreprocessingToken> output;

	MacroReplacer(const unordered_map<string, MacroDefinition>& macros, vector<PreprocessingToken> raw_input, SourcePositionTracker* tracker);

	const PreprocessingToken& peek(size_t i);

	bool check_blacklist();

	void replace();

};

string MacroStringify(const vector<PreprocessingToken>& input);

void SetBuildDate();

