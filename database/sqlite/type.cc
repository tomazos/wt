#include "database/sqlite/type.h"

#include <ostream>

#include "core/must.h"

namespace database {

const char* ToString(Type type) {
  switch (type) {
    case TYPE_NULL:
      return "NULL";
    case TYPE_INTEGER:
      return "INTEGER";
    case TYPE_REAL:
      return "REAL";
    case TYPE_TEXT:
      return "TEXT";
    case TYPE_BLOB:
      return "BLOB";
    default:
      FAIL("unknown database type");
  }
}

std::ostream& operator<<(std::ostream& os, Type type) {
  os << ToString(type);
  return os;
}

}  // namespace database
