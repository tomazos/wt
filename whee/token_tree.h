#pragma once

#include <boost/algorithm/string/join.hpp>
#include <boost/filesystem.hpp>
#include <vector>
#include <memory>

#include "core/string_functions.h"

namespace token_tree {

struct Entity {
  virtual ~Entity() = default;
  virtual string ToString() const = 0;
};

struct Leaf : Entity {
  string token;

  Leaf(const string& token_in) : token(token_in) {}

  string ToString() const override {
    return EncodeAsString("LEAF(", token, ")");
  }
};

struct KeyVal : Entity {
  string key;
  string value;

  KeyVal(const string& key_in, const string& value_in)
      : key(key_in), value(value_in) {}

  string ToString() const override {
    return EncodeAsString("KEYVAL(", key, ",", value, ")");
  }
};

struct Sequence : public Entity {
  string key;
  std::vector<std::unique_ptr<Entity>> elements;

  Sequence(const string& key_in) : key(key_in) {}

  void AddElement(std::unique_ptr<Entity> element) {
    elements.push_back(std::move(element));
  }
  string ToString() const override {
    std::vector<string> element_strings;
    for (const auto& element : elements)
      element_strings.push_back(element->ToString());
    return EncodeAsString("SEQUENCE(", key, ", [",
                          boost::algorithm::join(element_strings, ", "), "])");
  }
};

std::unique_ptr<Sequence> ParseSequenceFile(
    const boost::filesystem::path& token_tree_file);

}  // namespace token_tree
