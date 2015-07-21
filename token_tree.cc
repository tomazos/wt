#include "token_tree.h"

using boost::algorithm::split;
using boost::algorithm::token_compress_on;

Sequence ParseSequence(const string& filename, const string& token_tree_file) {
  const string token_tree_string = GetFileContents(token_tree_file);

  std::cout << "TOKEN TREE STRING: " << token_tree_string << std::endl;

  std::vector<string> lines;
  split(lines, token_tree_string, boost::is_any_of("\n"), token_compress_on);
  for (const string& line : lines) {
    int indent = 0;
    for (const char c : line) {
      if (c == ' ')
        ++indent;
      else
        break;
    }
    std::vector<string> tokens;
    split(tokens, line.substr(ident), boost::is_any_of(" "));
  }
}
