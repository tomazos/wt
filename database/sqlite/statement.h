#pragma once

//#include <boost/filesystem.hpp>

#include "database/sqlite/type.h"

struct sqlite3_stmt;

namespace database {
namespace sqlite {

class Statement {
 public:
  Statement(Statement&&);
  Statement& operator=(Statement&& that);
  ~Statement();

  void BindNull(int pos);
  void BindInteger(int pos, int64 i);
  void BindReal(int pos, float64 r);
  void BindText(int pos, string_view sv);
  void BindBlob(int pos, string_view sv);

  void Execute();

  [[gnu::warn_unused_result]] bool Step();

  Type ColumnType(int pos);
  int64 ColumnInteger(int pos);
  float64 ColumnReal(int pos);
  string_view ColumnText(int pos);
  string_view ColumnBlob(int pos);

  void Reset();

 private:
  Statement(sqlite3_stmt* stmt);

  void Close();

  sqlite3_stmt* stmt_ = nullptr;

  Statement(const Statement&) = delete;
  Statement& operator=(const Statement&) = delete;
  friend class Connection;
};

}  // namespace sqlite
}  // namespace database
