#pragma once

struct SourceInfo {
  string spelling;
  string filename;
  size_t linenum;
};

struct IPPTokenStream {
  virtual void emit_whitespace_sequence() = 0;
  virtual void emit_new_line() = 0;
  virtual void emit_header_name(SourceInfo src) = 0;
  virtual void emit_identifier(SourceInfo src) = 0;
  virtual void emit_pp_number(SourceInfo src) = 0;
  virtual void emit_character_literal(SourceInfo src) = 0;
  virtual void emit_user_defined_character_literal(SourceInfo src) = 0;
  virtual void emit_string_literal(SourceInfo src) = 0;
  virtual void emit_user_defined_string_literal(SourceInfo src) = 0;
  virtual void emit_preprocessing_op_or_punc(SourceInfo src) = 0;
  virtual void emit_non_whitespace_char(SourceInfo src) = 0;
  virtual void emit_eof() = 0;

  virtual ~IPPTokenStream() {}
};

void PPTokenize(string_view input, IPPTokenStream& output);
