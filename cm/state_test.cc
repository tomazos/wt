#include "cm/state.h"

#include <vector>

#include "core/collection_functions.h"
#include "gtest/gtest.h"
#include "cm/debug_allocator.h"

namespace cm {
namespace {

class StateTest : public testing::Test {
 public:
  StateTest() : state(allocator) {}
  ~StateTest() { EXPECT_EQ(state.StackSize(), 0); }

  DebugAllocator allocator;
  cm::State state;
};

TEST_F(StateTest, Smoke) {}

TEST_F(StateTest, Throw) {
  DebugAllocator allocator;
  cm::State state(allocator);

  bool threw = false;
  try {
    state.PushString("foo");
    state.Throw();
  } catch (const std::exception& e) {
    threw = true;
    string error_message = e.what();
    EXPECT_TRUE(Contains(error_message, "foo"));
  }
  EXPECT_TRUE(threw);
}

TEST_F(StateTest, StackQuery) {
  EXPECT_TRUE(state.CheckStack(10));
  EXPECT_TRUE(!state.CheckStack(1 << 30));

  state.PushInteger(1);
  state.PushInteger(2);
  state.PushInteger(3);
  EXPECT_EQ(state.Top(), 3);
  EXPECT_EQ(state.StackSize(), 3);

  EXPECT_EQ(state.AbsIndex(-1), 3);
  EXPECT_EQ(state.AbsIndex(-2), 2);
  EXPECT_EQ(state.AbsIndex(-3), 1);
  EXPECT_EQ(state.AbsIndex(1), 1);
  EXPECT_EQ(state.AbsIndex(2), 2);
  EXPECT_EQ(state.AbsIndex(3), 3);
  EXPECT_EQ(state.AbsIndex(4), 4);
  EXPECT_EQ(state.AbsIndex(LUA_REGISTRYINDEX), LUA_REGISTRYINDEX);
  EXPECT_EQ(state.AbsIndex(State::UPVALUE(1)), State::UPVALUE(1));
  EXPECT_EQ(state.AbsIndex(State::UPVALUE(2)), State::UPVALUE(2));
  EXPECT_EQ(state.AbsIndex(State::UPVALUE(3)), State::UPVALUE(3));
  state.Pop(3);
}

TEST_F(StateTest, ResizeStack) {
  state.PushInteger(1);
  state.PushInteger(2);
  state.PushInteger(3);
  EXPECT_EQ(state.StackSize(), 3);

  state.ResizeStack(2);
  EXPECT_EQ(state.ToInteger(1), 1);
  EXPECT_EQ(state.ToInteger(2), 2);
  EXPECT_EQ(state.StackSize(), 2);

  state.ResizeStack(4);
  EXPECT_EQ(state.ToInteger(1), 1);
  EXPECT_EQ(state.ToInteger(2), 2);
  EXPECT_TRUE(state.GetType(3) == Type::NIL);
  EXPECT_TRUE(state.GetType(4) == Type::NIL);
  EXPECT_TRUE(state.GetType(5) == Type::NONE);
  EXPECT_EQ(state.StackSize(), 4);
  state.Pop(4);
}

TEST_F(StateTest, Insert) {
  state.PushInteger(1);
  state.PushInteger(2);
  state.PushInteger(3);
  state.PushInteger(4);
  state.Insert(2);
  EXPECT_EQ(state.ToInteger(1), 1);
  EXPECT_EQ(state.ToInteger(2), 4);
  EXPECT_EQ(state.ToInteger(3), 2);
  EXPECT_EQ(state.ToInteger(4), 3);
  state.Pop(4);
}

TEST_F(StateTest, Rotate) {
  state.PushInteger(1);
  state.PushInteger(2);
  state.PushInteger(3);
  state.PushInteger(4);
  state.PushInteger(5);
  state.Rotate(2, 3);
  EXPECT_EQ(state.ToInteger(1), 1);
  EXPECT_EQ(state.ToInteger(2), 3);
  EXPECT_EQ(state.ToInteger(3), 4);
  EXPECT_EQ(state.ToInteger(4), 5);
  EXPECT_EQ(state.ToInteger(5), 2);
  state.Rotate(3, -1);
  EXPECT_EQ(state.ToInteger(1), 1);
  EXPECT_EQ(state.ToInteger(2), 3);
  EXPECT_EQ(state.ToInteger(3), 5);
  EXPECT_EQ(state.ToInteger(4), 2);
  EXPECT_EQ(state.ToInteger(5), 4);
  state.Pop(5);
}

TEST_F(StateTest, Copy) {
  state.PushInteger(1);
  state.PushInteger(2);
  state.PushInteger(3);
  state.PushInteger(4);
  state.PushInteger(5);
  state.Copy(2, 4);
  EXPECT_EQ(state.ToInteger(1), 1);
  EXPECT_EQ(state.ToInteger(2), 2);
  EXPECT_EQ(state.ToInteger(3), 3);
  EXPECT_EQ(state.ToInteger(4), 2);
  EXPECT_EQ(state.ToInteger(5), 5);
  state.Pop(5);
}

TEST_F(StateTest, PushCopy) {
  state.PushInteger(1);
  state.PushInteger(2);
  state.PushInteger(3);
  state.PushInteger(4);
  state.PushInteger(5);
  state.PushCopy(2);
  EXPECT_EQ(state.ToInteger(1), 1);
  EXPECT_EQ(state.ToInteger(2), 2);
  EXPECT_EQ(state.ToInteger(3), 3);
  EXPECT_EQ(state.ToInteger(4), 4);
  EXPECT_EQ(state.ToInteger(5), 5);
  EXPECT_EQ(state.ToInteger(6), 2);
  state.Pop(6);
}

TEST_F(StateTest, Remove) {
  state.PushInteger(1);
  state.PushInteger(2);
  state.PushInteger(3);
  state.PushInteger(4);
  state.PushInteger(5);
  state.Remove(3);
  EXPECT_EQ(state.ToInteger(1), 1);
  EXPECT_EQ(state.ToInteger(2), 2);
  EXPECT_EQ(state.ToInteger(3), 4);
  EXPECT_EQ(state.ToInteger(4), 5);
  EXPECT_TRUE(state.GetType(5) == Type::NONE);
  state.Pop(4);
}

TEST_F(StateTest, Replace) {
  state.PushInteger(1);
  state.PushInteger(2);
  state.PushInteger(3);
  state.PushInteger(4);
  state.PushInteger(5);
  state.Replace(3);
  EXPECT_EQ(state.ToInteger(1), 1);
  EXPECT_EQ(state.ToInteger(2), 2);
  EXPECT_EQ(state.ToInteger(3), 5);
  EXPECT_EQ(state.ToInteger(4), 4);
  state.Pop(4);
}

int push42_f(lua_State* L) {
  lua_pushinteger(L, 42);
  lua_pushinteger(L, 43);
  return 1;
}

TEST_F(StateTest, ElementQueryCreation) {
  EXPECT_EQ(state.StackSize(), 0);
  EXPECT_TRUE(state.GetType(1) == Type::NONE);

  state.PushNil();
  EXPECT_TRUE(state.GetType(1) == Type::NIL);
  EXPECT_EQ(state.StackSize(), 1);
  state.Pop();

  state.PushBoolean(true);
  state.PushBoolean(false);
  EXPECT_TRUE(state.GetType(1) == Type::BOOLEAN);
  EXPECT_TRUE(state.ToBoolean(1));
  EXPECT_TRUE(state.GetType(2) == Type::BOOLEAN);
  EXPECT_FALSE(state.ToBoolean(2));
  state.Pop(2);

  static constexpr int64 hi64 = std::numeric_limits<int64>::max();
  static constexpr int64 lo64 = std::numeric_limits<int64>::min();

  for (int64 i : std::vector<int64>{0, 42, -42, hi64, lo64, hi64 - 1, hi64 - 2,
                                    lo64 + 1, lo64 + 2}) {
    state.PushInteger(i);
    EXPECT_TRUE(state.GetType(1) == Type::INTEGER);
    EXPECT_EQ(state.ToInteger(1), i);
    state.Pop();
  }

  for (float64 f : std::vector<float64>{0, 0.1, 0.24}) {
    state.PushFloat(f);
    EXPECT_TRUE(state.GetType(1) == Type::FLOAT);
    EXPECT_EQ(state.ToFloat(1), f);
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
    EXPECT_TRUE(state.GetType(1) == Type::STRING);
    EXPECT_EQ(state.ToString(1), s);
    state.Pop();
  }

  string foo0bar1("foo\0bar", 7);
  string foo0bar2("foo\0bar", 7);
  string foo0baz("foo\0baz", 7);
  state.PushString(foo0bar1);
  EXPECT_TRUE(state.GetType(1) == Type::STRING);
  EXPECT_EQ(state.ToString(1), foo0bar2);
  EXPECT_NE(state.ToString(1), foo0baz);
  state.Pop();

  int x;
  void* p = &x;
  state.PushLightUserdata(p);
  EXPECT_TRUE(state.GetType(1) == Type::LIGHT_USERDATA);
  EXPECT_TRUE(state.ToUserdata(1) == p);
  state.Pop();

  state.PushNewTable();
  state.PushNewTable(3, 0);
  state.PushNewTable(0, 3);
  state.PushNewTable(3, 3);
  for (int i = 1; i <= 4; i++) MUST(state.GetType(i) == Type::TABLE);
  state.Pop(4);

  void* userdata = state.PushNewUserdata(1 << 20);
  EXPECT_TRUE(state.GetType(1) == Type::USERDATA);
  EXPECT_TRUE(userdata);
  EXPECT_TRUE(userdata == state.ToUserdata(1));
  EXPECT_TRUE(userdata == state.ToPointer(1));
  state.Pop();

  EXPECT_EQ(state.StackSize(), 0);

  state.PushCFunction(push42_f);
  EXPECT_EQ(state.StackSize(), 1);
  EXPECT_TRUE(state.GetType(1) == Type::CFUNCTION);
  EXPECT_TRUE(state.ToCFunction(1) == push42_f);
  EXPECT_EQ(state.StackSize(), 1);
  state.Call(0, 1);
  EXPECT_EQ(state.StackSize(), 1);
  EXPECT_TRUE(state.GetType(1) == Type::INTEGER);
  EXPECT_EQ(state.ToInteger(1), 43);
  state.Pop(1);
}

TEST_F(StateTest, Arithmetic) {
  state.PushInteger(2);
  state.PushInteger(3);
  state.ADD();
  EXPECT_TRUE(state.GetType(1) == Type::INTEGER);
  EXPECT_EQ(state.ToInteger(1), 5);
  state.Pop();

  state.PushInteger(5);
  state.PushInteger(3);
  state.SUB();
  EXPECT_TRUE(state.GetType(1) == Type::INTEGER);
  EXPECT_EQ(state.ToInteger(1), 2);
  state.Pop();

  state.PushInteger(5);
  state.PushInteger(3);
  state.MUL();
  EXPECT_TRUE(state.GetType(1) == Type::INTEGER);
  EXPECT_EQ(state.ToInteger(1), 15);
  state.Pop();

  state.PushInteger(9);
  state.PushInteger(4);
  state.DIV();
  EXPECT_TRUE(state.GetType(1) == Type::INTEGER);
  EXPECT_EQ(state.ToInteger(1), 2);
  state.Pop();

  state.PushFloat(9);
  state.PushFloat(4);
  state.DIV();
  EXPECT_TRUE(state.GetType(1) == Type::FLOAT);
  EXPECT_EQ(state.ToFloat(1), 2.25);
  state.Pop();

  state.PushInteger(9);
  state.PushInteger(4);
  state.MOD();
  EXPECT_TRUE(state.GetType(1) == Type::INTEGER);
  EXPECT_EQ(state.ToInteger(1), 1);
  state.Pop();

  state.PushFloat(9.75);
  state.PushFloat(3.75);
  state.MOD();
  EXPECT_TRUE(state.GetType(1) == Type::FLOAT);
  EXPECT_EQ(state.ToFloat(1), 2.25);
  state.Pop();

  state.PushInteger(10);
  state.NEG();
  EXPECT_TRUE(state.GetType(1) == Type::INTEGER);
  EXPECT_EQ(state.ToInteger(1), -10);
  state.Pop();

  const int64 xforneg = 0xF8A8'C2D1'924F'01AC;
  state.PushInteger(xforneg);
  state.BNOT();
  EXPECT_TRUE(state.GetType(1) == Type::INTEGER);
  EXPECT_EQ(state.ToInteger(1), ~xforneg);
  state.Pop();

  state.PushInteger(0b1010'0011);
  state.PushInteger(0b0010'1010);
  state.BAND();
  EXPECT_TRUE(state.GetType(1) == Type::INTEGER);
  EXPECT_EQ(state.ToInteger(1), 0b0010'0010);
  state.Pop();

  state.PushInteger(0b1010'0011);
  state.PushInteger(0b0010'1010);
  state.BOR();
  EXPECT_TRUE(state.GetType(1) == Type::INTEGER);
  EXPECT_EQ(state.ToInteger(1), 0b1010'1011);
  state.Pop();

  state.PushInteger(0b1010'0011);
  state.PushInteger(0b0010'1010);
  state.XOR();
  EXPECT_TRUE(state.GetType(1) == Type::INTEGER);
  EXPECT_EQ(state.ToInteger(1), 0b1000'1001);
  state.Pop();

  state.PushInteger(0b1010'0011);
  state.PushInteger(2);
  state.SHL();
  EXPECT_TRUE(state.GetType(1) == Type::INTEGER);
  EXPECT_EQ(state.ToInteger(1), 0b1010'0011'00);
  state.Pop();

  state.PushInteger(0b1010'0011);
  state.PushInteger(2);
  state.SHR();
  EXPECT_TRUE(state.GetType(1) == Type::INTEGER);
  EXPECT_EQ(state.ToInteger(1), 0b1010'00);
  state.Pop();
}

TEST_F(StateTest, Comparison) {
  state.PushInteger(3);
  state.PushInteger(3);
  state.PushInteger(4);
  state.PushInteger(2);

  EXPECT_TRUE(state.EQ(1, 2));
  EXPECT_FALSE(state.EQ(1, 3));
  EXPECT_FALSE(state.NE(1, 2));
  EXPECT_TRUE(state.NE(1, 3));
  EXPECT_TRUE(state.LT(2, 3));
  EXPECT_FALSE(state.LT(1, 2));
  EXPECT_FALSE(state.LT(1, 4));
  EXPECT_TRUE(state.GT(1, 4));
  EXPECT_FALSE(state.GT(1, 2));
  EXPECT_FALSE(state.GT(1, 3));
  EXPECT_TRUE(state.GE(1, 4));
  EXPECT_TRUE(state.GE(1, 2));
  EXPECT_FALSE(state.GE(1, 3));
  EXPECT_TRUE(state.LE(2, 3));
  EXPECT_TRUE(state.LE(1, 2));
  EXPECT_FALSE(state.LE(1, 4));

  state.Pop(4);
}

TEST_F(StateTest, Strings) {
  state.PushString("foo");
  state.PushString("barr");
  state.LEN(1);
  state.LEN(2);

  EXPECT_EQ(state.ToInteger(3), 3);
  EXPECT_EQ(state.ToInteger(4), 4);
  state.Pop(2);

  state.PushString("baz");
  state.Concat(3);
  EXPECT_EQ(state.ToString(1), "foobarrbaz");

  state.Pop(1);
}

[[noreturn]] int protected_test_function(lua_State* L) {
  lua_pushlstring(L, "fail", 4);
  lua_error(L);
}

TEST_F(StateTest, Calls) {
  state.LoadFromString("x,y = ...; return x + y;", "test");
  state.PushInteger(2);
  state.PushInteger(3);
  state.Call(2, 1);
  EXPECT_EQ(state.ToInteger(1), 5);
  state.Pop();

  state.LoadFromString("return ...;", "test");
  state.PushInteger(1);
  state.PushInteger(2);
  state.PushInteger(3);
  state.CallMultret(3);
  EXPECT_EQ(state.ToInteger(1), 1);
  EXPECT_EQ(state.ToInteger(2), 2);
  EXPECT_EQ(state.ToInteger(3), 3);
  state.Pop(3);

  state.PushCFunction(protected_test_function);
  EXPECT_ANY_THROW(state.CallProtected(0, 0));
}

TEST_F(StateTest, LoadSave) {
  state.LoadFromString("x,y = ...; return x + y;", "test",
                       State::ChunkFormat::TEXT);
  string saved = state.SaveToString();
  state.Pop();
  EXPECT_EQ(state.StackSize(), 0);
  state.LoadFromString(saved, "test", State::ChunkFormat::BINARY);
  state.PushInteger(2);
  state.PushInteger(3);
  state.Call(2, 1);
  EXPECT_EQ(state.ToInteger(1), 5);
  state.Pop();
}

TEST_F(StateTest, GCSmoke) {
  state.GCStop();
  state.GCRestart();
  state.GCCollect();
  state.PushNewUserdata(10'000);
  EXPECT_GE(state.GCCount(), 10'000);
  state.Pop();
  state.GCCollect();
  EXPECT_LT(state.GCCount(), 10'000);
  state.GCSetPause(true);
  state.GCSetPause(false);
  state.GCSetStepMultiplier(10);
  EXPECT_TRUE(state.GCIsRunning());
  state.GCStop();
  EXPECT_FALSE(state.GCIsRunning());
}

TEST_F(StateTest, Tables) {
  state.LoadFromString("y = 2 * x;");
  EXPECT_EQ(state.StackSize(), 1);
  state.PushGlobalTable();
  EXPECT_EQ(state.StackSize(), 2);
  state.PushString("x");
  EXPECT_EQ(state.StackSize(), 3);
  state.PushInteger(21);
  EXPECT_EQ(state.StackSize(), 4);
  state.PopField(2);
  EXPECT_EQ(state.StackSize(), 2);
  state.Pop();
  EXPECT_EQ(state.StackSize(), 1);
  state.Call(0, 0);
  EXPECT_EQ(state.StackSize(), 0);
  state.PushGlobalTable();
  EXPECT_EQ(state.StackSize(), 1);
  state.PushString("y");
  EXPECT_EQ(state.StackSize(), 2);
  state.PushField(1);
  EXPECT_EQ(state.StackSize(), 2);
  EXPECT_EQ(state.ToInteger(2), 42);
  state.Pop(2);
}

TEST_F(StateTest, Uservalue) {
  state.PushNewUserdata(1);
  state.PushInteger(42);
  state.PopUservalue(1);
  EXPECT_EQ(state.StackSize(), 1);
  state.PushUservalue(1);
  EXPECT_EQ(state.ToInteger(2), 42);
  state.Pop(2);
}

TEST_F(StateTest, Metatable) {
  state.PushString("foo");  // 1
  EXPECT_FALSE(state.PushMetatable(1));
  state.PushNewTable();  // 2
  state.PushString("__index");
  state.PushNewTable();  // 4
  state.PushString("bar");
  state.LoadFromString("self = ...; baz = self;");
  state.PopField(4);
  state.PopField(2);
  state.PopMetatable(1);
  state.Pop();

  state.LoadFromString("local s = \"qux\"; return s:bar();");
  state.Call(0, 0);
  state.PushGlobalTable();
  state.PushString("baz");
  state.PushField(1);
  EXPECT_EQ(state.ToString(2), "qux");
  state.Pop(2);
}

}  // namespace
}  // namespace cm
