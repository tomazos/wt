#include "testing.h"

#include "xxua/debug_allocator.h"
#include "xxua/state.h"

namespace {

void Smoke() {
  xxua::DebugAllocator debug_allocator;
  xxua::State state(debug_allocator);
}

}  // namespace

void TestMain() { Smoke(); }
