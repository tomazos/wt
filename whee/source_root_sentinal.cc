#include "whee/source_root_sentinal.h"

#include <boost/algorithm/string.hpp>
#include <regex>
#include <sstream>

using boost::algorithm::split;

const string& GetSourceRootSentinal() {
  static const string source_root_sentinal = "FynmfH4Tn6CA";
  return source_root_sentinal;
}

string ReplaceSourceRootSentinal(const string& input) {
  static const std::regex re("\\.whee\\/.*\\/" + GetSourceRootSentinal() +
                             "\\/");
  std::vector<string> lines_in, lines_out;
  std::ostringstream oss;
  split(lines_in, input, boost::is_any_of("\n"));
  for (const string& line_in : lines_in)
    oss << std::regex_replace(line_in, re, "") << std::endl;
  string output = oss.str();
  output.resize(output.size() - 1);
  return output;
}
