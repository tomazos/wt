#include "xxua/api.h"

#include "gtest/gtest.h"
#include "xxua/debug_allocator.h"

namespace xxua {
namespace {

class ApiTest : public testing::Test {
 public:
  ApiTest() : state(allocator) {}
  ~ApiTest() { EXPECT_EQ(state.StackSize(), 0); }

  DebugAllocator allocator;
  State state;
};

struct TestObject {
  TestObject(int& total, int x) : total_(total), y_(x * 2) { total_ += y_; }
  ~TestObject() { total_ -= y_; }

  int& total_;
  int y_;
};

TEST_F(ApiTest, Smoke) {}

TEST_F(ApiTest, EmplaceObject) {
  int total = 0;
  EmplaceObject<TestObject>(&state, total, 21);
  EmplaceObject<TestObject>(&state, total, 7);
  EmplaceObject<TestObject>(&state, total, 13);
  EXPECT_EQ(total, 82);
  EXPECT_EQ(ToObject<TestObject>(&state, 1).y_, 42);
  EXPECT_EQ(ToObject<TestObject>(&state, 2).y_, 14);
  EXPECT_EQ(ToObject<TestObject>(&state, 3).y_, 26);
  state.Pop(3);
  state.GCCollect();
  EXPECT_EQ(total, 0);
}

TEST_F(ApiTest, PushFunction) {
  int total = 0;
  PushFunction(&state, [&] {
    total++;
    return 0;
  });
  for (int i = 0; i < 1000; i++) {
    state.PushCopy(1);
    state.Call(0, 0);
  }
  EXPECT_EQ(total, 1000);
  state.Pop();
}

}  // namespace
}  // namespace xxua
