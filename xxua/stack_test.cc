#include "xxua/stack.h"

#include <vector>

#include "core/collection_functions.h"
#include "gtest/gtest.h"
#include "xxua/debug_allocator.h"

namespace xxua {
namespace {

class StackTest : public testing::Test {
 public:
  StackTest() : state(allocator), context(state) {}
  ~StackTest() { EXPECT_EQ(StackSize(), 0); }

  DebugAllocator allocator;
  State state;
  Context context;
};

TEST_F(StackTest, Iteration) {
  PushInteger(1);
  PushInteger(2);
  PushInteger(3);
  State state2;
  state2.PushInteger(4);
  state2.PushInteger(5);

  Stack stack1;
  Stack stack2(state2);

  Integer total = 0;
  for (auto ref : stack1) {
    total += ref.ToInteger();
  }
  EXPECT_EQ(total, 6);
  EXPECT_EQ(stack1.size(), 3);
  EXPECT_EQ(stack1.front().ToInteger(), 1);
  EXPECT_EQ(stack1.back().ToInteger(), 3);

  total = 0;
  for (auto ref : stack2) {
    total += ref.ToInteger();
  }
  EXPECT_EQ(total, 9);
  EXPECT_EQ(stack2.size(), 2);
  EXPECT_EQ(stack2.front().ToInteger(), 4);
  EXPECT_EQ(stack2.back().ToInteger(), 5);

  stack1.clear();
  stack2.clear();
}

}  // namespace
}  // namespace xxua
