#include <algorithm>
#include <map>

#include "core/file_functions.h"
#include "core/must.h"
#include "core/process.h"
#include "core/string_functions.h"
#include "core/utf8.h"
#include "database/postgresql/connection.h"
#include "database/postgresql/result.h"
#include "experimental/cppsrc/tokenizer.h"
#include "main/noargs.h"

using database::postgresql::Connection;
using database::postgresql::Result;
using database::postgresql::bytea;

struct ForPPTokenStream : IPPTokenStream {
  bool found = false;
  virtual void emit_whitespace_sequence() {}
  virtual void emit_new_line() {}
  virtual void emit_header_name(SourceInfo src) {}
  virtual void emit_identifier(SourceInfo src) {
    if (src.spelling == "for") found = true;
  }
  virtual void emit_pp_number(SourceInfo src) {}
  virtual void emit_character_literal(SourceInfo src) {}

  virtual void emit_user_defined_character_literal(SourceInfo src) {}
  virtual void emit_string_literal(SourceInfo src) {}

  virtual void emit_user_defined_string_literal(SourceInfo src) {}

  virtual void emit_preprocessing_op_or_punc(SourceInfo src) {}
  virtual void emit_non_whitespace_char(SourceInfo src) {}
  virtual void emit_eof() {}
};

void Main() {
  Connection connection(
      "postgresql://postgres:klaseddOcs0@localhost?dbname=cppsrc");

  Result result = connection.ExecuteCommand(
      "select abspath from files where cext and (ext = 'cc' or ext = 'cpp') "
      "order by id desc");

  const int nfiles = result.NumRows();
  int nmatches = 0;

  std::cout << nfiles << " source files" << std::endl;

  for (int64 i = 0; i < nfiles; ++i) {
    string abspath = result.Get<string>(i, 0).value();
    string content = GetFileContents(abspath);

    ForPPTokenStream stream;

    try {
      PPTokenize(content, stream);
      if (stream.found == true) {
        nmatches++;
        std::cout << "Match: " << nmatches << std::endl;
        std::cout << "Searched: " << i + 1 << " source files" << std::endl;
        std::cout << "File: "
                  << abspath.substr(std::strlen("/media/second/cppsrc/files/"))
                  << std::endl;
        std::cout << std::endl;
        ExecuteShellCommand("grep -n20 '\\bfor\\b.(\\b' ", abspath);
        std::cout << std::endl;
        std::cout << "--------------------------------------------------------"
                  << std::endl;
        std::cout << std::endl;
      }
      if (nmatches == 1000) break;
    } catch (std::exception& e) {
    }
  }
}
