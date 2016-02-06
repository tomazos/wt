#include <algorithm>
#include <map>

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
  std::vector<int64> ids;

  for (int64 id = 1; id <= nfiles; ++id) ids.push_back(id);

  std::random_shuffle(ids.begin(), ids.end());

  Connection connection(
      "postgresql://postgres:klaseddOcs0@localhost?dbname=cppsrc");

  connection.ExecuteCommand("begin");

  try {
    for (int64 i = 0; i < nfiles; ++i)
      connection.ExecuteCommand("update files set id=$1 where baseid=$2",
                                ids[i], i + 1);
    connection.ExecuteCommand("commit");
  } catch (...) {
    connection.ExecuteCommand("rollback");
  }
}
