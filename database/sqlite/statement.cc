#include "database/sqlite/statement.h"

#include "database/sqlite/sqlite3_wrapper.h"

namespace database {
namespace sqlite {

Statement::Statement(sqlite3_stmt* stmt) : stmt_(stmt) {}

Statement::Statement(Statement&& that) : stmt_(that.stmt_) {
  that.stmt_ = nullptr;
}

Statement& Statement::operator=(Statement&& that) {
  Close();
  stmt_ = that.stmt_;
  that.stmt_ = nullptr;
  return *this;
}

Statement::~Statement() { Close(); }

void Statement::BindNull(int pos) { S3CALL(bind_null, stmt_, pos); }

void Statement::BindInteger(int pos, int64 i) {
  S3CALL(bind_int64, stmt_, pos, i);
}

void Statement::BindReal(int pos, float64 r) {
  S3CALL(bind_double, stmt_, pos, r);
}

void Statement::BindText(int pos, string_view sv) {
  S3CALL(bind_text64, stmt_, pos, sv.data(), sv.size(), SQLITE_TRANSIENT,
         SQLITE_UTF8);
}

void Statement::BindBlob(int pos, string_view sv) {
  S3CALL(bind_blob64, stmt_, pos, sv.data(), sv.size(), SQLITE_TRANSIENT);
}

void Statement::Execute() {
  if (Step()) FAIL("execute returned data");
}

bool Statement::Step() {
  int step_result = sqlite3_step(stmt_);
  switch (step_result) {
    case SQLITE_ROW:
      return true;
    case SQLITE_DONE:
      return false;
    default: {
      const char* errstr = sqlite3_errstr(step_result);
      FAIL("step: ", errstr);
    }
  }
}

Type Statement::ColumnType(int pos) {
  switch (sqlite3_column_type(stmt_, pos)) {
    case SQLITE_INTEGER:
      return TYPE_INTEGER;
    case SQLITE_FLOAT:
      return TYPE_REAL;
    case SQLITE_TEXT:
      return TYPE_TEXT;
    case SQLITE_BLOB:
      return TYPE_BLOB;
    case SQLITE_NULL:
      return TYPE_NULL;
    default:
      FAIL("unknown database type");
  }
}

int64 Statement::ColumnInteger(int pos) {
  return sqlite3_column_int64(stmt_, pos);
}

float64 Statement::ColumnReal(int pos) {
  return sqlite3_column_double(stmt_, pos);
}

string_view Statement::ColumnText(int pos) {
  const size_t len = sqlite3_column_bytes(stmt_, pos);
  const unsigned char* const data = sqlite3_column_text(stmt_, pos);
  return {(const char*)data, len};
}

string_view Statement::ColumnBlob(int pos) {
  const size_t len = sqlite3_column_bytes(stmt_, pos);
  const void* const data = sqlite3_column_blob(stmt_, pos);
  return {(const char*)data, len};
}

void Statement::Reset() { S3CALL(reset, stmt_); }

void Statement::Close() {
  S3CALL(finalize, stmt_);
  stmt_ = nullptr;
}

}  // namespace sqlite
}  // namespace database
