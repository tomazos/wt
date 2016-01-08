#include "database/sqlite/connection.h"

#include "database/sqlite/sqlite3_wrapper.h"

namespace database {
namespace sqlite {

void Initialize() { S3CALL(config, SQLITE_CONFIG_SERIALIZED); }

Connection::Connection(const filesystem::path& filename) {
  S3CALL(open, filename.string().c_str(), &db_);
}

Connection::Connection(const filesystem::path& filename, int flags) {
  S3CALL(open_v2, filename.string().c_str(), &db_, flags, nullptr);
}

Connection::Connection(Connection&& that) : db_(that.db_) {
  that.db_ = nullptr;
}

Connection& Connection::operator=(Connection&& that) {
  Close();
  db_ = that.db_;
  that.db_ = nullptr;
  return *this;
}

Connection::~Connection() { Close(); }

void Connection::Close() {
  S3CALL(close, db_);
  db_ = nullptr;
}

void Connection::operator()(string_view sql) { Prepare(sql).Execute(); }

Statement Connection::Prepare(string_view sql) {
  sqlite3_stmt* stmt = nullptr;
  MUST_LE(sql.size(), size_t(std::numeric_limits<int>::max()));
  const char* tail = nullptr;
  S3CALL(prepare_v2, db_, sql.data(), sql.size(), &stmt, &tail);
  if (tail != sql.data() + sql.size()) {
    S3CALL(finalize, stmt);
    FAIL("prepare: extra sql: ", sql.substr(tail - sql.data()));
  }
  return Statement(stmt);
}

}  // namespace sqlite
}  // namespace database
