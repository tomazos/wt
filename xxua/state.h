#pragma once

#include "lua/lua.h"
#include "xxua/allocator.h"

namespace xxua {

class State {
 public:
  State();
  State(Allocator& allocator);
  State(State&& other);
  ~State();
  State& operator=(State&& other);

 private:
  void AtPanic(lua_CFunction panic_function);
  void Close();

  State(lua_State* L_in);

  lua_State* L;
  State(const State& state) = delete;
  State& operator=(const State& other) = delete;
};

}  // namespace xxua
