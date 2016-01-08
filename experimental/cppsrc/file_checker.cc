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

string_view category(string_view package) {
  if (package.substr(0, 3) == "lib")
    return package.substr(0, 4);
  else
    return package.substr(0, 1);
}

static filesystem::path files_dir = "/media/second/cppsrc/files";

void Main() {
  Connection connection(
      "postgresql://postgres:klaseddOcs0@localhost?dbname=cppsrc");

  Result packages =
      connection.ExecuteCommand("select component, id from packages");

  for (int i = 0; i < packages.NumRows(); ++i) {
    if (i % 100 == 99) LOGEXPR(i);
    const string component = packages.Get<string>(i, 0).value();
    const string package = packages.Get<string>(i, 1).value();
    Result files = connection.ExecuteCommand(
        "select relpath, abspath, size from files where package = $1", package);
    MUST_GT(files.NumRows(), 0);
    for (int j = 0; j < files.NumRows(); ++j) {
      const string relpath = files.Get<string>(j, 0).value();
      const string abspath = files.Get<string>(j, 1).value();
      const int64 size = files.Get<int64>(j, 2).value();
      if (filesystem::exists(abspath) && filesystem::is_regular_file(abspath)) {
        if (filesystem::file_size(abspath) != size_t(size))
          std::cout << "bad size: " << abspath;
      } else {
        std::cout << "bad abspath: " << abspath << std::endl;
      }
      if (abspath !=
          files_dir / component / category(package).to_string() / package /
              relpath) {
        std::cout << "bad relpath: " << abspath << std::endl;
        std::cout << "             " << relpath << std::endl;
      }
    }
  }
}
