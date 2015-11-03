#pragma once

#include "cm/private/lua.h"
#include "cm/private/object.h"

namespace cm {

enum class Type : uint8 {
  NONE = XLUA_TNONE,
  NIL = LUA_TNIL,
  BOOLEAN = LUA_TBOOLEAN,
  INTEGER = LUA_TNUMINT,
  FLOAT = LUA_TNUMFLT,
  STRING = LUA_TSTRING,
  TABLE = LUA_TTABLE,
  LCLOSURE = LUA_TLCL,
  CFUNCTION = LUA_TLCF,
  CCLOSURE = LUA_TCCL,
  LIGHT_USERDATA = LUA_TLIGHTUSERDATA,
  USERDATA = LUA_TUSERDATA
};

string_view TypeToString(Type t);

}  // namespace cm
