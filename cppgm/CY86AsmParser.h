#pragma once

#include "Tokens.h"

struct CY86AsmParser;

struct CY86Asm {
  CY86AsmParser* parser;

  CY86Asm();
  ~CY86Asm();

  void parse(const string& srcfile, vector<PToken>&& tokens);

  void dump(ostream& out);
  void output_program(ostream& out);

  pair<string, size_t> error_loc();
};
