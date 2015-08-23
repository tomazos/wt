#include "xxua/state.h"

#include <vector>

#include "testing.h"
#include "xxua/debug_allocator.h"

namespace xxua {
namespace {

void Smoke() { xxua::State state; }

void BasicPush() {
  DebugAllocator allocator;
  xxua::State state(allocator);
  MUST_EQ(state.StackSize(), 0);
  MUST(state.GetType(1) == Type::NONE);

  state.PushNil();
  MUST(state.GetType(1) == Type::NIL);
  MUST_EQ(state.StackSize(), 1);
  state.Pop();

  state.PushBoolean(true);
  state.PushBoolean(false);
  MUST(state.GetType(1) == Type::BOOLEAN);
  MUST_EQ(state.ToBoolean(1), true);
  MUST(state.GetType(2) == Type::BOOLEAN);
  MUST_EQ(state.ToBoolean(2), false);
  state.Pop(2);

  static constexpr int64 hi64 = std::numeric_limits<int64>::max();
  static constexpr int64 lo64 = std::numeric_limits<int64>::min();

  for (int64 i : std::vector<int64>{0, 42, -42, hi64, lo64, hi64 - 1, hi64 - 2,
                                    lo64 + 1, lo64 + 2}) {
    state.PushInteger(i);
    MUST(state.GetType(1) == Type::INTEGER);
    MUST_EQ(state.ToInteger(1), i);
    state.Pop();
  }

  for (float64 f : std::vector<float64>{0, 0.1, 0.24}) {
    state.PushFloat(f);
    MUST(state.GetType(1) == Type::FLOAT);
    MUST_EQ(state.ToFloat(1), f);
    state.Pop();
  }

  for (string s : std::vector<string>{"", "foo", "bar", R"(

 On the other hand, we denounce with righteous indignation and dislike men who
 are so beguiled and demoralized by the charms of pleasure of the moment, so
 blinded by desire, that they cannot foresee the pain and trouble that are
 bound to ensue; and equal blame belongs to those who fail in their duty
 through weakness of will, which is the same as saying through shrinking from
 toil and pain. These cases are perfectly simple and easy to distinguish. In a
 free hour, when our power of choice is untrammelled and when nothing prevents
 our being able to do what we like best, every pleasure is to be welcomed and
 every pain avoided. But in certain circumstances and owing to the claims of
 duty or the obligations of business it will frequently occur that pleasures
 have to be repudiated and annoyances accepted. The wise man therefore always
 holds in these matters to this principle of selection: he rejects pleasures to
 secure other greater pleasures, or else he endures pains to avoid worse pains.
       )"}) {
    state.PushString(s);
    MUST(state.GetType(1) == Type::STRING);
    MUST_EQ(state.ToString(1), s);
    state.Pop();
  }

  string foo0bar1("foo\0bar", 7);
  string foo0bar2("foo\0bar", 7);
  string foo0baz("foo\0baz", 7);
  state.PushString(foo0bar1);
  MUST(state.GetType(1) == Type::STRING);
  MUST_EQ(state.ToString(1), foo0bar2);
  MUST_NE(state.ToString(1), foo0baz);
  state.Pop();
}

}  // namespace
}  // namespace xxua

void TestMain() {
  xxua::Smoke();
  xxua::BasicPush();
}
