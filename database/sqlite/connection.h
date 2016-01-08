#pragma once

#include <boost/filesystem.hpp>

#include "database/sqlite/statement.h"

typedef struct sqlite3 sqlite3;

namespace database {
namespace sqlite {

void Initialize();

class Connection {
 public:
  Connection(const filesystem::path& filename);
  Connection(const filesystem::path& filename, int flags);
  Connection(Connection&&);
  Connection& operator=(Connection&&);
  ~Connection();

  void operator()(string_view sql);
  Statement Prepare(string_view sql);

 private:
  void Close();

  sqlite3* db_ = nullptr;

  Connection(const Connection&) = delete;
  Connection& operator=(const Connection&) = delete;
};

}  // namespace sqlite
}  // namespace database
