#include "xxua/api.h"

#include "gtest/gtest.h"
#include "xxua/debug_allocator.h"

namespace xxua {
namespace {

class ApiTest : public testing::Test {
 public:
  ApiTest() : internal_state(allocator), context(internal_state) {}
  ~ApiTest() { EXPECT_EQ(StackSize(), 0); }

  DebugAllocator allocator;
  State internal_state;
  Context context;
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
  EmplaceObject<TestObject>(total, 21);
  EmplaceObject<TestObject>(total, 7);
  EmplaceObject<TestObject>(total, 13);
  EXPECT_EQ(total, 82);
  EXPECT_EQ(ToObject<TestObject>(1).y_, 42);
  EXPECT_EQ(ToObject<TestObject>(2).y_, 14);
  EXPECT_EQ(ToObject<TestObject>(3).y_, 26);
  Pop(3);
  GCCollect();
  EXPECT_EQ(total, 0);
}

TEST_F(ApiTest, PushFunction) {
  int total = 0;
  PushFunction([&] {
    total++;
    return 0;
  });
  for (int i = 0; i < 1000; i++) {
    PushCopy(1);
    Call(0, 0);
  }
  EXPECT_EQ(total, 1000);
  Pop();
}

}  // namespace
}  // namespace xxua
