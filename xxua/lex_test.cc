#include "xxua/context.h"

#include "gtest/gtest.h"
#include "xxua/debug_allocator.h"

namespace xxua {
namespace {

class LexTest : public testing::Test {
 public:
  LexTest() : internal_state(allocator), context(internal_state) {}
  ~LexTest() { EXPECT_EQ(StackSize(), 0); }

  DebugAllocator allocator;
  State internal_state;
  Context context;
};

TEST_F(LexTest, Comments) {
  EXPECT_ANY_THROW(LoadFromString("/*"));
  LoadFromString("//");
  LoadFromString("// the quick brown fox");
  LoadFromString("/**/");
  LoadFromString(R"(
                 /*****

               ****Multiline comment****

                              ****/

               // the quick brown fox
                 // jumps over the
                 // lazy dog.


                 )");
  Pop(5);
}

}  // namespace
}  // namespace xxua
