#include "whee/token_tree.h"

#include <boost/algorithm/string.hpp>
#include <cctype>
#include <iostream>

#include "core/file_functions.h"
#include "core/must.h"
#include "core/string_functions.h"

#include "xxua/context.h"
#include "xxua/value.h"

namespace token_tree {

using boost::algorithm::split;
using boost::algorithm::token_compress_on;
using namespace xxua;

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

Values AddRule(Sequence& sequence, string_view rule, const Values& args) {
  if (args.size() != 1) Throw("expected 1 argument");

  auto rule_sequence = std::make_unique<Sequence>(rule);
  for (const std::pair<Value, Value>& kv : args.at(0)) {
    const Value& key = kv.first;
    const Value& value = kv.second;

    if (key.type() == Type::INTEGER) {
      rule_sequence->AddElement(std::make_unique<Leaf>(string(value)));
    } else {
      MUST(key.type() == Type::STRING);
      if (value.type() == Type::TABLE) {
        auto sub_sequence = std::make_unique<Sequence>(string(key));
        for (std::pair<Value, Value> kv : value) {
          const Value& key = kv.first;
          const Value& value = kv.second;
          MUST(key.type() == Type::INTEGER);
          MUST(value.type() == Type::STRING);
          sub_sequence->AddElement(std::make_unique<Leaf>(string(value)));
        }
        rule_sequence->AddElement(std::move(sub_sequence));
      } else if (value.type() == Type::STRING) {
        rule_sequence->AddElement(
            std::make_unique<KeyVal>(string(key), string(value)));
      }
    }
  }

  sequence.AddElement(std::move(rule_sequence));
  return {};
}

std::unique_ptr<Sequence> ParseNewSequenceFile(
    const filesystem::path& token_tree_file) {
  State state;
  Context context(state);

  Value rules = Compile(GetFileContents(token_tree_file));
  auto sequence = std::make_unique<Sequence>(token_tree_file.string());
  for (auto rule : {"test", "library", "program", "proto"})
    Global().insert(rule, MakeFunction([&, rule](const Values& args) -> Values {
      return AddRule(*sequence, rule, args);
    }));
  rules({});
  return sequence;
}

}  // namespace token_tree
