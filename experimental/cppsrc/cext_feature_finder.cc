#include <algorithm>
#include <map>
#include <unordered_map>

#include "core/file_functions.h"
#include "core/must.h"
#include "core/string_functions.h"
#include "core/utf8.h"
#include "database/postgresql/connection.h"
#include "database/postgresql/result.h"
#include "main/noargs.h"

using database::postgresql::Connection;
using database::postgresql::Result;
using database::postgresql::bytea;

constexpr int64 nfiles = 10150894;

void Main() {
  Connection connection(
      "postgresql://postgres:klaseddOcs0@localhost?dbname=cppsrc");

  Result files =
      connection.ExecuteCommand("select cext, abspath from files order by id");

  std::unordered_map<string, size_t> numerators[2];
  std::vector<size_t> denominators(2, 0);

  for (int i = 0; i < files.NumRows(); ++i) {
    const bool cext = files.Get<bool>(i, 0).value();
    const string abspath = files.Get<string>(i, 1).value();
    const string content = GetFileContents(abspath);
    const size_t size = content.size();

    denominators[cext] += size;

    for (size_t j = 0; j < size; ++j)
      for (size_t k = 1; k <= 3; ++k)
        if (j + k <= size) numerators[cext][content.substr(j, k)]++;

    if (!(i & (i - 1))) {
      connection.ExecuteCommand("begin");
      connection.ExecuteCommand("delete from ngrams");
      for (bool b : {true, false}) {
        const int64 denominator = denominators[b];
        std::cout << "Saving at " << i << "th " << b << " denom " << denominator
                  << std::endl;
        for (auto kv : numerators[b]) {
          const string ngram = kv.first;
          const int64 numerator = kv.second;
          connection.ExecuteCommand(
              "insert into ngrams (ngram, cext, numerator, denominator) values "
              "($1, $2, $3, $4)",
              bytea{ngram}, b, numerator, denominator);
        }
      }
      connection.ExecuteCommand("commit");
    }
  }
}
