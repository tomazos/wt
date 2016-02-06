#pragma once

#include "IPPTokenStream.h"

struct DebugPPTokenStream : IPPTokenStream {
  void emit_whitespace_sequence() { cout << "whitespace-sequence 0 " << endl; }

  void emit_new_line() { cout << "new-line 0 " << endl; }

  void emit_header_name(SourceInfo src) {
    write_token("header-name", src.spelling);
  }

  void emit_identifier(SourceInfo src) {
    write_token("identifier", src.spelling);
  }

  void emit_pp_number(SourceInfo src) {
    write_token("pp-number", src.spelling);
  }

  void emit_character_literal(SourceInfo src) {
    write_token("character-literal", src.spelling);
  }

  void emit_user_defined_character_literal(SourceInfo src) {
    write_token("user-defined-character-literal", src.spelling);
  }

  void emit_string_literal(SourceInfo src) {
    write_token("string-literal", src.spelling);
  }

  void emit_user_defined_string_literal(SourceInfo src) {
    write_token("user-defined-string-literal", src.spelling);
  }

  void emit_preprocessing_op_or_punc(SourceInfo src) {
    write_token("preprocessing-op-or-punc", src.spelling);
  }

  void emit_non_whitespace_char(SourceInfo src) {
    write_token("non-whitespace-character", src.spelling);
  }

  void emit_eof() { cout << "eof" << endl; }

 private:
  void write_token(const string& type, const string& data) {
    cout << type << " " << data.size() << " ";
    cout.write(data.data(), data.size());
    cout << endl;
  }
};
