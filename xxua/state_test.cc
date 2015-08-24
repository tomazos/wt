#include "xxua/state.h"

#include <vector>

#include "core/collection_functions.h"
#include "testing.h"
#include "xxua/debug_allocator.h"

namespace xxua {
namespace {

void Smoke() { xxua::State state; }

void ErrorTest() {
  DebugAllocator allocator;
  xxua::State state(allocator);

  bool threw = false;
  try {
    state.PushString("foo");
    state.Error();
  } catch (const std::exception& e) {
    threw = true;
    string error_message = e.what();
    MUST(Contains(error_message, "foo"));
  }
  MUST(threw);
}

void StackQuery() {
  DebugAllocator allocator;
  xxua::State state(allocator);

  MUST(state.CheckStack(10));
  MUST(!state.CheckStack(1 << 30));

  state.PushInteger(1);
  state.PushInteger(2);
  state.PushInteger(3);
  MUST_EQ(state.Top(), 3);
  MUST_EQ(state.StackSize(), 3);

  MUST_EQ(state.AbsIndex(-1), 3);
  MUST_EQ(state.AbsIndex(-2), 2);
  MUST_EQ(state.AbsIndex(-3), 1);
  MUST_EQ(state.AbsIndex(1), 1);
  MUST_EQ(state.AbsIndex(2), 2);
  MUST_EQ(state.AbsIndex(3), 3);
  MUST_EQ(state.AbsIndex(4), 4);
  MUST_EQ(state.AbsIndex(LUA_REGISTRYINDEX), LUA_REGISTRYINDEX);
  MUST_EQ(state.AbsIndex(State::UPVALUE(1)), State::UPVALUE(1));
  MUST_EQ(state.AbsIndex(State::UPVALUE(2)), State::UPVALUE(2));
  MUST_EQ(state.AbsIndex(State::UPVALUE(3)), State::UPVALUE(3));
}

void StackMutate() {
  DebugAllocator allocator;
  xxua::State state(allocator);

  state.PushInteger(1);
  state.PushInteger(2);
  state.PushInteger(3);
  MUST_EQ(state.StackSize(), 3);

  state.ResizeStack(2);
  MUST_EQ(state.ToInteger(1), 1);
  MUST_EQ(state.ToInteger(2), 2);
  MUST_EQ(state.StackSize(), 2);

  state.ResizeStack(4);
  MUST_EQ(state.ToInteger(1), 1);
  MUST_EQ(state.ToInteger(2), 2);
  MUST(state.GetType(3) == Type::NIL);
  MUST(state.GetType(4) == Type::NIL);
  MUST(state.GetType(5) == Type::NONE);
  MUST_EQ(state.StackSize(), 4);
  state.Pop(4);

  state.PushInteger(1);
  state.PushInteger(2);
  state.PushInteger(3);
  state.PushInteger(4);
  state.Insert(2);
  MUST(state.ToInteger(1), 1);
  MUST(state.ToInteger(2), 4);
  MUST(state.ToInteger(3), 2);
  MUST(state.ToInteger(4), 3);
  state.Pop(4);
}

int push42_f(lua_State* L) {
  lua_pushinteger(L, 42);
  lua_pushinteger(L, 43);
  return 1;
}

void ElementQueryCreation() {
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

  int x;
  void* p = &x;
  state.PushLightUserdata(p);
  MUST(state.GetType(1) == Type::LIGHT_USERDATA);
  MUST(state.ToUserdata(1) == p);
  state.Pop();

  state.PushNewTable();
  state.PushNewTable(3, 0);
  state.PushNewTable(0, 3);
  state.PushNewTable(3, 3);
  for (int i = 1; i <= 4; i++) MUST(state.GetType(i) == Type::TABLE);
  state.Pop(4);

  void* userdata = state.PushNewUserdata(1 << 20);
  MUST(state.GetType(1) == Type::USERDATA);
  MUST(userdata);
  MUST(userdata == state.ToUserdata(1));
  MUST(userdata == state.ToPointer(1));
  state.Pop();

  MUST_EQ(state.StackSize(), 0);

  state.PushCFunction(push42_f);
  MUST_EQ(state.StackSize(), 1);
  MUST(state.GetType(1) == Type::CFUNCTION);
  MUST(state.ToCFunction(1) == push42_f);
  MUST_EQ(state.StackSize(), 1);
  state.Call(0, 1);
  MUST_EQ(state.StackSize(), 1);
  MUST(state.GetType(1) == Type::INTEGER);
  MUST_EQ(state.ToInteger(1), 43);
  state.Pop(1);
}

}  // namespace
}  // namespace xxua

void TestMain() {
  xxua::Smoke();
  xxua::ErrorTest();
  xxua::StackQuery();
  xxua::StackMutate();
  xxua::ElementQueryCreation();
}
