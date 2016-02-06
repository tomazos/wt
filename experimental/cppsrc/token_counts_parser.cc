#include <iostream>
#include <map>

#include "core/must.h"

void Main() {
  size_t total;
  std::cin >> total;
  std::map<string, int64> token_counts;
  for (size_t i = 0; i < total; ++i) {
    if ((i & (i - 1)) == 0) {
      LOGEXPR(token_counts.size());
    }

    size_t count;
    size_t token_size;
    std::cin >> count >> token_size;
    MUST_EQ(std::cin.get(), ' ');
    std::vector<char> v(token_size);
    std::cin.read(&v[0], token_size);
    MUST_EQ(std::cin.get(), '\n');
    string token(v.begin(), v.end());
    token_counts[token] = count;
  }

  int64 total_tokens = 0;
  std::multimap<int64, string> count_tokens;
  for (const auto& kv : token_counts) {
    total_tokens += kv.second;
    count_tokens.insert(std::make_pair(kv.second, kv.first));
  }
  LOGEXPR(total_tokens);
  for (const auto& kv : count_tokens) {
    if (kv.first > 100000) std::cout << kv.first << " " << kv.second << "\n";
  }

  std::cout << std::endl;
}
