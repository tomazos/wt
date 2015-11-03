#include "testing.h"

#include "cm/debug_allocator.h"
#include "cm/state.h"

namespace {

void Smoke() {
  cm::DebugAllocator debug_allocator;
  cm::State state(debug_allocator);
}

}  // namespace

void TestMain() { Smoke(); }
