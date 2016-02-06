#pragma once

#include "Tokens.h"

struct SemCParser;

struct SemC {
  SemCParser* parser;

  SemC();
  ~SemC();

  void parse(const string& srcfile, vector<PToken>&& tokens);

  void dump(ostream& out);
  void mock_nsinit(ostream& out);

  pair<string, size_t> error_loc();
};
