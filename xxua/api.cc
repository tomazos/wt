#include "xxua/api.h"

#include <cstring>

namespace xxua {

int DispatchFunction(lua_State* L) {
  void* extraspace = lua_getextraspace(L);
  void* state_void;
  std::memcpy(&state_void, extraspace, LUA_EXTRASPACE);
  if (!state_void) {
    lua_pushliteral(L, "unable to read extraspace");
    lua_error(L);
  }
  State& state = *static_cast<State*>(state_void);
  Context context(state);
  ToObject<std::function<void()>>(UPVALUE(1))();
  return 0;
}

}  // namespace xxua