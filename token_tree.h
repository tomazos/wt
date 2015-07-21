#pragma once

namespace token_tree {

struct Entity {
  virtual ~Entity() = default;
};

struct Leaf : Entity {
  string token;

  Leaf(const string& token_in) : token(token_in);
}

struct KeyVal : Entity {
  string key;
  string value;

  KeyVal(const string& key_in, const string& value_in)
      : key(key_in), value(value_in) {}
}

struct Sequence : public Entity {
  std::vector<std::unique_ptr<Entity>> elements;

  void add_element(std::unique_ptr<Entity> element) {
    elements.push_back(element);
  }
}

Sequence
ParseTokenTreeFile(const boost::filesystem::path& token_tree_file);

}  // namespace token_tree
