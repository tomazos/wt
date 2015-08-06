#include "token_tree.h"

#include <boost/algorithm/string.hpp>
#include <cctype>
#include <iostream>

#include "file_functions.h"
#include "must.h"
#include "string_functions.h"

namespace token_tree {

using boost::algorithm::split;
using boost::algorithm::token_compress_on;

std::unique_ptr<Sequence> ParseSequenceFile(
    const boost::filesystem::path& token_tree_file) {
  const string token_tree_string = GetFileContents(token_tree_file);

  std::vector<string> lines;
  split(lines, token_tree_string, boost::is_any_of("\n"));
  int linenum = 0;
  std::vector<std::unique_ptr<Sequence>> open_sequences;
  open_sequences.push_back(
      std::make_unique<Sequence>(token_tree_file.string()));

  for (const string& line : lines) {
    ++linenum;
#define PARSEFAIL(...)                                                       \
  ThrowError(token_tree_file.string().c_str(), linenum, __VA_ARGS__, "\n  ", \
             line)
    int indent = 0;
    for (const char c : line) {
      if (c == ' ')
        ++indent;
      else
        break;
    }
    const string terminal = line.substr(indent);
    if (terminal.empty()) continue;

    if (indent % 2 != 0) PARSEFAIL("bad ident");
    const size_t level = indent / 2;
    while (level + 1 < open_sequences.size()) {
      std::unique_ptr<Sequence> last_sequence =
          std::move(open_sequences.back());
      if (last_sequence->elements.empty()) PARSEFAIL("empty sequence");
      open_sequences.pop_back();
      open_sequences.back()->AddElement(std::move(last_sequence));
    }
    if (level + 1 != open_sequences.size()) PARSEFAIL("bad indent");
    if (Contains(terminal, '=')) {
      auto pos = terminal.find(" = ");
      if (pos == string::npos) {
        PARSEFAIL("expected ' = '");
      }
      const string key = terminal.substr(0, pos);
      const string value = terminal.substr(pos + 3);
      if (AnyOf(key, [](char c) { return std::isspace(c); }))
        PARSEFAIL("key contains whitespace");
      if (AnyOf(value, IsSpace)) PARSEFAIL("value contains whitespace");
      open_sequences.back()->AddElement(std::make_unique<KeyVal>(key, value));
    } else if (Contains(terminal, ':')) {
      if (AnyOf(terminal, IsSpace)) PARSEFAIL("contains whitespace");
      if (terminal.back() != ':') PARSEFAIL("bad ':'");
      const string key = terminal.substr(0, terminal.size() - 1);
      if (key.empty()) PARSEFAIL("bad key");
      open_sequences.push_back(std::make_unique<Sequence>(key));
    } else {
      if (AnyOf(terminal, IsSpace)) PARSEFAIL("contains whitespace");
      open_sequences.back()->AddElement(std::make_unique<Leaf>(terminal));
    }
  }

  while (open_sequences.size() > 1) {
    std::unique_ptr<Sequence> last_sequence = std::move(open_sequences.back());
    if (last_sequence->elements.empty())
      ThrowError(token_tree_file.string().c_str(), linenum, "empty sequence");
    open_sequences.pop_back();
    open_sequences.back()->AddElement(std::move(last_sequence));
  }

  return std::move(open_sequences.front());
}

}  // namespace token_tree
