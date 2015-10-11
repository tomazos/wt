#include <boost/filesystem.hpp>
#include <fstream>

#include "core/must.h"
#include "main/args.h"
#include "whee/token_tree.h"

using namespace token_tree;

void DumpEntity(std::ostream& os, const Entity* entity, int level);
void DumpSequence(std::ostream& os, const Sequence& sequence, int level);
void DumpKeyVal(std::ostream& os, const KeyVal& keyval, int level);
void DumpLeaf(std::ostream& os, const Leaf& leaf, int level);

void DumpEntity(std::ostream& os, const Entity* entity, int level) {
  auto s = dynamic_cast<const Sequence*>(entity);
  if (s) {
    DumpSequence(os, *s, level);
  }
  auto kv = dynamic_cast<const KeyVal*>(entity);
  if (kv) {
    DumpKeyVal(os, *kv, level);
  }
  auto l = dynamic_cast<const Leaf*>(entity);
  if (l) {
    DumpLeaf(os, *l, level);
  }
}

void DumpSequence(std::ostream& os, const Sequence& sequence, int level) {
  os << string(level * 2, ' ') << sequence.key << " = {\n";
  for (const auto& entity : sequence.elements) {
    DumpEntity(os, entity.get(), level + 1);
  }
  os << string(level * 2, ' ') << "},\n";
}

void DumpKeyVal(std::ostream& os, const KeyVal& keyval, int level) {
  os << string(level * 2, ' ') << keyval.key << " = \"" << keyval.value
     << "\",\n";
}

void DumpLeaf(std::ostream& os, const Leaf& leaf, int level) {
  os << string(level * 2, ' ') << "\"" << leaf.token << "\",\n";
}

void Main(const std::vector<string>& args) {
  MUST_EQ(args.size(), 2u);
  filesystem::path src = args.at(0);
  filesystem::path dest = args.at(1);
  MUST(exists(src));
  {
    std::ofstream os(dest.string());

    std::unique_ptr<Sequence> sequence = ParseSequenceFile(src);
    for (const auto& element : sequence->elements) {
      const Sequence& s = dynamic_cast<const Sequence&>(*element);
      os << s.key << "{\n";
      for (const auto& e2 : s.elements) {
        DumpEntity(os, e2.get(), 1);
      }
      os << "};\n\n";
    }
  }
  std::unique_ptr<Sequence> sequence = ParseSequenceFile(src);
  std::unique_ptr<Sequence> new_sequence = ParseNewSequenceFile(dest);

  sequence->key = "";
  new_sequence->key = "";
  if (sequence->ToString() != new_sequence->ToString()) {
    std::cout << "ERROR:" << std::endl;
    std::cout << sequence->ToString() << std::endl;
    std::cout << std::endl;
    std::cout << new_sequence->ToString() << std::endl;
  }
}
