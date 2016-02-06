#pragma once

#pragma once

#include "IPostTokenOutputStream.h"

#include "Text.h"

struct FilePostTokenOutputStream : IPostTokenOutputStream {
  FilePostTokenOutputStream(ostream& out) : out(out) {}

  virtual void emit_invalid(const string& source) {
    throw logic_error("invalid token: " + source);
  }

  virtual void emit_simple(const string& source, ETokenType token_type) {
    out << "simple " << source << " " << SimpleTokenTypeToString(token_type)
        << endl;
  }

  virtual void emit_identifier(const string& source) {
    out << "identifier " << source << endl;
  }

  virtual void emit_literal(const string& source, EFundamentalType type,
                            void* data, size_t nbytes) {
    out << "literal " << source << " " << FundamentalTypeToString(type) << " "
        << HexDump(data, nbytes) << endl;
  }

  virtual void emit_literal_array(const string& source, size_t num_elements,
                                  EFundamentalType type, void* data,
                                  size_t nbytes) {
    out << "literal " << source << " array of " << num_elements << " "
        << FundamentalTypeToString(type) << " " << HexDump(data, nbytes)
        << endl;
  }

  virtual void emit_user_defined_literal_character(const string& source,
                                                   const string& ud_suffix,
                                                   EFundamentalType type,
                                                   void* data, size_t nbytes) {
    out << "user-defined-literal " << source << " " << ud_suffix
        << " character " << FundamentalTypeToString(type) << " "
        << HexDump(data, nbytes) << endl;
  }

  virtual void emit_user_defined_literal_string_array(
      const string& source, const string& ud_suffix, size_t num_elements,
      EFundamentalType type, void* data, size_t nbytes) {
    out << "user-defined-literal " << source << " " << ud_suffix
        << " string array of " << num_elements << " "
        << FundamentalTypeToString(type) << " " << HexDump(data, nbytes)
        << endl;
  }

  virtual void emit_user_defined_literal_integer(const string& source,
                                                 const string& ud_suffix,
                                                 const string& prefix) {
    out << "user-defined-literal " << source << " " << ud_suffix << " integer "
        << prefix << endl;
  }

  virtual void emit_user_defined_literal_floating(const string& source,
                                                  const string& ud_suffix,
                                                  const string& prefix) {
    out << "user-defined-literal " << source << " " << ud_suffix << " floating "
        << prefix << endl;
  }

  virtual void emit_eof() { out << "eof" << endl; }
};
