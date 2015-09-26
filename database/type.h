#pragma once

namespace database {

enum Type {
  TYPE_NULL,
  TYPE_INTEGER,
  TYPE_REAL,
  TYPE_TEXT,
  TYPE_BLOB,
};

const char* ToString(Type type);

std::ostream& operator<<(std::ostream& os, Type type);

}  // namespace database
