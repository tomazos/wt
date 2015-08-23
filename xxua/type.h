#pragma once

#include "lua/lua.h"
#include "lua/lobject.h"

namespace xxua {

enum class Type : uint8 {
  NONE = XLUA_TNONE,
  NIL = LUA_TNIL,
  BOOLEAN = LUA_TBOOLEAN,
  INTEGER = LUA_TNUMINT,
  FLOAT = LUA_TNUMFLT,
  STRING = LUA_TSTRING,
  LCLOSURE = LUA_TLCL,
  CFUNCTION = LUA_TLCF,
  CCLOSURE = LUA_TCCL
};

}  // namespace xxua
