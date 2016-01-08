#pragma once

#include <postgresql/libpq-fe.h>

#include "database/postgresql/type_handlers.h"

namespace database {
namespace postgresql {

class Connection;

class Result {
 public:
  int NumRows() { return PQntuples(result_.get()); }
  int NumCols() { return PQnfields(result_.get()); }
  Oid ColType(int col) { return PQftype(result_.get(), col); }
  optional<string_view> RawGet(int row, int col) {
    if (PQgetisnull(result_.get(), row, col))
      return nullopt;
    else
      return string_view(PQgetvalue(result_.get(), row, col),
                         PQgetlength(result_.get(), row, col));
  }

  template <typename T>
  optional<T> Get(int row, int col);

 private:
  Result(Connection* connection, PGresult* result);

  Connection* connection_;
  std::unique_ptr<PGresult, void (*)(PGresult*)> result_;

  friend class Connection;
  template <std::size_t... I>
  friend class CommandExecutor;
};

template <typename T>
optional<T> Result::Get(int row, int col) {
  optional<string_view> raw_result = RawGet(row, col);
  if (!raw_result) return nullopt;

  Oid oid = ColType(col);
  return Type<T>::Decode(*raw_result, oid);
}

}  // namespace postgresql
}  // namespace database
