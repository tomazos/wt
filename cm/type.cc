#include "cm/type.h"

#include "core/must.h"

namespace cm {

string_view TypeToString(Type t) {
  switch (t) {
    case Type::NONE:
      return "NONE";
    case Type::NIL:
      return "NIL";
    case Type::BOOLEAN:
      return "BOOLEAN";
    case Type::INTEGER:
      return "INTEGER";
    case Type::FLOAT:
      return "FLOAT";
    case Type::STRING:
      return "STRING";
    case Type::TABLE:
      return "TABLE";
    case Type::LCLOSURE:
      return "LCLOSURE";
    case Type::CFUNCTION:
      return "CFUNCTION";
    case Type::CCLOSURE:
      return "CCLOSURE";
    case Type::LIGHT_USERDATA:
      return "LIGHT_USERDATA";
    case Type::USERDATA:
      return "USERDATA";
    default:
      FAIL("unknown type");
  }
}
}  // namespace cm
