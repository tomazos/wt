#include <map>
#include <unordered_map>

#include "core/file_functions.h"
#include "core/must.h"
#include "core/string_functions.h"
#include "core/utf8.h"
#include "database/postgresql/connection.h"
#include "database/postgresql/result.h"
#include "experimental/cppsrc/tokenizer.h"
#include "main/noargs.h"

using database::postgresql::Connection;
using database::postgresql::Result;
using database::postgresql::bytea;

static filesystem::path files_dir = "/media/second/cppsrc/files";

enum TokenKind { HEADER, IDENTIFIER, NUMBER, CHARACTER, STRING, OPERATOR };

struct Token {
  TokenKind kind;
  string spelling;
  size_t linenum;

  operator string() const {
    std::ostringstream oss;
    switch (kind) {
      case HEADER:
        oss << 'H';
        break;
      case IDENTIFIER:
        oss << 'I';
        break;
      case NUMBER:
        oss << 'N';
        break;
      case CHARACTER:
        oss << 'C';
        break;
      case STRING:
        oss << 'S';
        break;
      case OPERATOR:
        oss << 'O';
        break;
    }
    oss << ' ' << spelling;
    return oss.str();
  }
};

struct CapturePPTokenStream : IPPTokenStream {
  std::vector<Token> tokens;
  virtual void emit_whitespace_sequence() {}
  virtual void emit_new_line() {}
  virtual void emit_header_name(SourceInfo src) {
    tokens.push_back({HEADER, src.spelling, src.linenum});
  }
  virtual void emit_identifier(SourceInfo src) {
    tokens.push_back({IDENTIFIER, src.spelling, src.linenum});
  }
  virtual void emit_pp_number(SourceInfo src) {
    tokens.push_back({NUMBER, src.spelling, src.linenum});
  }
  virtual void emit_character_literal(SourceInfo src) {
    tokens.push_back({CHARACTER, src.spelling, src.linenum});
  }
  virtual void emit_user_defined_character_literal(SourceInfo src) {
    tokens.push_back({CHARACTER, src.spelling, src.linenum});
  }
  virtual void emit_string_literal(SourceInfo src) {
    tokens.push_back({STRING, src.spelling, src.linenum});
  }
  virtual void emit_user_defined_string_literal(SourceInfo src) {
    tokens.push_back({STRING, src.spelling, src.linenum});
  }
  virtual void emit_preprocessing_op_or_punc(SourceInfo src) {
    tokens.push_back({OPERATOR, src.spelling, src.linenum});
  }
  virtual void emit_non_whitespace_char(SourceInfo src) {}
  virtual void emit_eof() {}
};

void Main() {
  Connection connection(
      "postgresql://postgres:klaseddOcs0@localhost?dbname=cppsrc");

  Result files = connection.ExecuteCommand(
      "select id, abspath from files where cext order by id");

  std::map<string, int64> errors;
  int64 num_files = 0;
  int64 num_tokens = 0;
  std::unordered_map<string, int64> token_counts;

  for (int i = 0; i < files.NumRows(); ++i) {
    // int64 id = files.Get<int64>(i, 0).value();
    string abspath = files.Get<string>(i, 1).value();
    string content = GetFileContents(abspath);

    CapturePPTokenStream stream;

    try {
      PPTokenize(content, stream);
      num_files++;
      num_tokens += stream.tokens.size();
      for (const Token& token : stream.tokens) {
        token_counts[token]++;
      }
    } catch (std::exception& e) {
      string error = e.what();
      errors[error]++;
    }
  }
  std::map<string, int64> ordered_token_counts;
  for (const auto& kv : token_counts) {
    ordered_token_counts.insert(kv);
  }

  std::cout << ordered_token_counts.size() << std::endl;
  for (const auto& kv : ordered_token_counts) {
    const string& token = kv.first;
    int64 count = kv.second;
    std::cout << count << " " << (token.size() - 2) << " " << token << "\n";
  }
  std::cout.flush();
}

/// home/zos/wt/experimental/cppsrc/files_tokenizer.cc:99:1: error = invalid
/// characters in raw string delimiter
/// home/zos/wt/experimental/cppsrc/files_tokenizer.cc:100:1: count = 26
/// home/zos/wt/experimental/cppsrc/files_tokenizer.cc:99:1: error = invalid
/// code point
/// home/zos/wt/experimental/cppsrc/files_tokenizer.cc:100:1: count = 15
/// home/zos/wt/experimental/cppsrc/files_tokenizer.cc:99:1: error = invalid
/// escape sequence
/// home/zos/wt/experimental/cppsrc/files_tokenizer.cc:100:1: count = 1961
/// home/zos/wt/experimental/cppsrc/files_tokenizer.cc:99:1: error = invalid hex
/// escape sequence
/// home/zos/wt/experimental/cppsrc/files_tokenizer.cc:100:1: count = 6
/// home/zos/wt/experimental/cppsrc/files_tokenizer.cc:99:1: error = partial
/// comment
/// home/zos/wt/experimental/cppsrc/files_tokenizer.cc:100:1: count = 37
/// home/zos/wt/experimental/cppsrc/files_tokenizer.cc:99:1: error = raw string
/// delimiter too long
/// home/zos/wt/experimental/cppsrc/files_tokenizer.cc:100:1: count = 5
/// home/zos/wt/experimental/cppsrc/files_tokenizer.cc:99:1: error =
/// unterminated character literal
/// home/zos/wt/experimental/cppsrc/files_tokenizer.cc:100:1: count = 857
/// home/zos/wt/experimental/cppsrc/files_tokenizer.cc:99:1: error =
/// unterminated header name
/// home/zos/wt/experimental/cppsrc/files_tokenizer.cc:100:1: count = 12
/// home/zos/wt/experimental/cppsrc/files_tokenizer.cc:99:1: error =
/// unterminated raw string literal
/// home/zos/wt/experimental/cppsrc/files_tokenizer.cc:100:1: count = 5
/// home/zos/wt/experimental/cppsrc/files_tokenizer.cc:99:1: error =
/// unterminated string literal
/// home/zos/wt/experimental/cppsrc/files_tokenizer.cc:100:1: count = 176
/// home/zos/wt/experimental/cppsrc/files_tokenizer.cc:99:1: error = utf8
/// expected trailing byte (10xxxxxx)
/// home/zos/wt/experimental/cppsrc/files_tokenizer.cc:100:1: count = 13355
/// home/zos/wt/experimental/cppsrc/files_tokenizer.cc:99:1: error = utf8
/// invalid unit (111111xx)
/// home/zos/wt/experimental/cppsrc/files_tokenizer.cc:100:1: count = 1824
/// home/zos/wt/experimental/cppsrc/files_tokenizer.cc:99:1: error = utf8
/// trailing code unit (10xxxxxx) at start
/// home/zos/wt/experimental/cppsrc/files_tokenizer.cc:100:1: count = 7568
/// home/zos/wt/experimental/cppsrc/files_tokenizer.cc:103:1: total_errors =
/// 25847
/// home/zos/wt/experimental/cppsrc/files_tokenizer.cc:104:1: num_files =
/// 2566989
/// home/zos/wt/experimental/cppsrc/files_tokenizer.cc:105:1: num_tokens =
/// 4689316529
/// home/zos/wt/experimental/cppsrc/files_tokenizer.cc:106:1:
/// token_counts.size() = 50325647
