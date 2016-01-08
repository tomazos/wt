#include <map>

#include "core/file_functions.h"
#include "core/must.h"
#include "database/postgresql/connection.h"
#include "main/args.h"

using database::postgresql::Connection;

using KeyVal = std::pair<string, std::vector<string>>;
using Entry = std::vector<KeyVal>;

class SourcesFileParser {
 public:
  SourcesFileParser(const string& text) : text_(text), pos_(0) {}

  std::vector<Entry> ParseFile() {
    std::vector<Entry> entries;
    while (Current() != '\0') entries.push_back(ParseEntry());
    return entries;
  }

 private:
  Entry ParseEntry() {
    Entry entry;
    while (Current() != '\n') {
      entry.push_back(ParseKeyVal());
    }
    Advance();
    return entry;
  }

  std::pair<string, std::vector<string>> ParseKeyVal() {
    string key = ParseKey();
    Expect(':');
    Advance();
    if (Current() == '\n') Advance();
    Expect(' ');
    std::vector<string> val = ParseVal();
    return std::make_pair(key, val);
  }

  string ParseKey() {
    const size_t key_start = Pos();
    while (IsKeyChar(Current())) Advance();
    const size_t key_length = Pos() - key_start;
    string key = text_.substr(key_start, key_length);
    return key;
  }

  bool IsKeyChar(char c) { return std::isalnum(c) || c == '-'; }

  std::vector<string> ParseVal() {
    std::vector<string> lines;
    while (Current() == ' ') {
      Advance();
      lines.push_back(ParseLine());
    }
    return lines;
  }

  string ParseLine() {
    const size_t line_start = Pos();
    while (Current() != '\n') Advance();
    const size_t line_length = Pos() - line_start;
    Advance();
    string line = text_.substr(line_start, line_length);
    return line;
  }

  void Expect(char c) {
    if (Current() != c) {
      FAIL("Expected '", c, "' at: ", text_.substr(pos_, 20));
    }
  }

  void Advance() {
    MUST_NE(Current(), '\0');
    pos_++;
  }

  char Current() { return text_[pos_]; }

  size_t Pos() { return pos_; }
  const string text_;
  size_t pos_;
};

// create table package_attributes(component text, package text, key text,
//                                lineno int4, value text);

void Main(const std::vector<string>& args) {
  MUST_EQ(args.size(), 2u, "Usage: sources_parser <component> ");
  string component = args.at(0);
  filesystem::path sources_path = args.at(1);
  const string sources_file = GetFileContents(sources_path);
  SourcesFileParser sources_file_parser(sources_file);
  std::vector<Entry> entries = sources_file_parser.ParseFile();

  Connection connection(
      "postgresql://postgres:klaseddOcs0@localhost?dbname=cppsrc");
  for (const Entry entry : entries) {
    string package;
    bool package_found = false;
    for (const KeyVal& keyval : entry) {
      string key = keyval.first;
      std::vector<string> value = keyval.second;
      if (key == "Package") {
        MUST_EQ(value.size(), 1u);
        package = value[0];
        package_found = true;
      }
    }
    MUST(package_found);
    MUST(!package.empty());

    for (const KeyVal& keyval : entry) {
      string key = keyval.first;
      std::vector<string> values = keyval.second;
      for (size_t lineno = 0; lineno < values.size(); ++lineno) {
        string value = values[lineno];
        connection.ExecuteCommand(R"(
          insert into package_attributes
            (component, package, key, lineno, value)
            values ($1, $2, $3, $4, $5) )",
                                  component, package, key, int32(lineno),
                                  value);
      }
    }
  }
}
