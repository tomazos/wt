#include "cm/context.h"

#include <vector>

#include "core/collection_functions.h"
#include "gtest/gtest.h"
#include "cm/debug_allocator.h"

namespace cm {
namespace {

class ContextTest : public testing::Test {
 public:
  ContextTest() : internal_state(allocator), context(internal_state) {}
  ~ContextTest() { EXPECT_EQ(StackSize(), 0); }

  DebugAllocator allocator;
  State internal_state;
  Context context;
};

TEST_F(ContextTest, Smoke) {}

TEST_F(ContextTest, Throw) {
  bool threw = false;
  try {
    PushString("foo");
    Throw();
  } catch (const std::exception& e) {
    threw = true;
    string error_message = e.what();
    EXPECT_TRUE(Contains(error_message, "foo"));
  }
  EXPECT_TRUE(threw);
  EXPECT_TRUE(GetType(-1) == Type::STRING);
  EXPECT_EQ(ToString(-1), "foo");
  Pop();
}

TEST_F(ContextTest, StackQuery) {
  EXPECT_TRUE(CheckStack(10));
  EXPECT_TRUE(!CheckStack(1 << 30));

  PushInteger(1);
  PushInteger(2);
  PushInteger(3);
  EXPECT_EQ(Top(), 3);
  EXPECT_EQ(StackSize(), 3);

  EXPECT_EQ(AbsIndex(-1), 3);
  EXPECT_EQ(AbsIndex(-2), 2);
  EXPECT_EQ(AbsIndex(-3), 1);
  EXPECT_EQ(AbsIndex(1), 1);
  EXPECT_EQ(AbsIndex(2), 2);
  EXPECT_EQ(AbsIndex(3), 3);
  EXPECT_EQ(AbsIndex(4), 4);
  EXPECT_EQ(AbsIndex(LUA_REGISTRYINDEX), LUA_REGISTRYINDEX);
  EXPECT_EQ(AbsIndex(State::UPVALUE(1)), State::UPVALUE(1));
  EXPECT_EQ(AbsIndex(State::UPVALUE(2)), State::UPVALUE(2));
  EXPECT_EQ(AbsIndex(State::UPVALUE(3)), State::UPVALUE(3));
  Pop(3);
}

TEST_F(ContextTest, ResizeStack) {
  PushInteger(1);
  PushInteger(2);
  PushInteger(3);
  EXPECT_EQ(StackSize(), 3);

  ResizeStack(2);
  EXPECT_EQ(ToInteger(1), 1);
  EXPECT_EQ(ToInteger(2), 2);
  EXPECT_EQ(StackSize(), 2);

  ResizeStack(4);
  EXPECT_EQ(ToInteger(1), 1);
  EXPECT_EQ(ToInteger(2), 2);
  EXPECT_TRUE(GetType(3) == Type::NIL);
  EXPECT_TRUE(GetType(4) == Type::NIL);
  EXPECT_TRUE(GetType(5) == Type::NONE);
  EXPECT_EQ(StackSize(), 4);
  Pop(4);
}

TEST_F(ContextTest, Insert) {
  PushInteger(1);
  PushInteger(2);
  PushInteger(3);
  PushInteger(4);
  Insert(2);
  EXPECT_EQ(ToInteger(1), 1);
  EXPECT_EQ(ToInteger(2), 4);
  EXPECT_EQ(ToInteger(3), 2);
  EXPECT_EQ(ToInteger(4), 3);
  Pop(4);
}

TEST_F(ContextTest, Rotate) {
  PushInteger(1);
  PushInteger(2);
  PushInteger(3);
  PushInteger(4);
  PushInteger(5);
  Rotate(2, 3);
  EXPECT_EQ(ToInteger(1), 1);
  EXPECT_EQ(ToInteger(2), 3);
  EXPECT_EQ(ToInteger(3), 4);
  EXPECT_EQ(ToInteger(4), 5);
  EXPECT_EQ(ToInteger(5), 2);
  Rotate(3, -1);
  EXPECT_EQ(ToInteger(1), 1);
  EXPECT_EQ(ToInteger(2), 3);
  EXPECT_EQ(ToInteger(3), 5);
  EXPECT_EQ(ToInteger(4), 2);
  EXPECT_EQ(ToInteger(5), 4);
  Pop(5);
}

TEST_F(ContextTest, Copy) {
  PushInteger(1);
  PushInteger(2);
  PushInteger(3);
  PushInteger(4);
  PushInteger(5);
  Copy(2, 4);
  EXPECT_EQ(ToInteger(1), 1);
  EXPECT_EQ(ToInteger(2), 2);
  EXPECT_EQ(ToInteger(3), 3);
  EXPECT_EQ(ToInteger(4), 2);
  EXPECT_EQ(ToInteger(5), 5);
  Pop(5);
}

TEST_F(ContextTest, PushCopy) {
  PushInteger(1);
  PushInteger(2);
  PushInteger(3);
  PushInteger(4);
  PushInteger(5);
  PushCopy(2);
  EXPECT_EQ(ToInteger(1), 1);
  EXPECT_EQ(ToInteger(2), 2);
  EXPECT_EQ(ToInteger(3), 3);
  EXPECT_EQ(ToInteger(4), 4);
  EXPECT_EQ(ToInteger(5), 5);
  EXPECT_EQ(ToInteger(6), 2);
  Pop(6);
}

TEST_F(ContextTest, Remove) {
  PushInteger(1);
  PushInteger(2);
  PushInteger(3);
  PushInteger(4);
  PushInteger(5);
  Remove(3);
  EXPECT_EQ(ToInteger(1), 1);
  EXPECT_EQ(ToInteger(2), 2);
  EXPECT_EQ(ToInteger(3), 4);
  EXPECT_EQ(ToInteger(4), 5);
  EXPECT_TRUE(GetType(5) == Type::NONE);
  Pop(4);
}

TEST_F(ContextTest, Replace) {
  PushInteger(1);
  PushInteger(2);
  PushInteger(3);
  PushInteger(4);
  PushInteger(5);
  Replace(3);
  EXPECT_EQ(ToInteger(1), 1);
  EXPECT_EQ(ToInteger(2), 2);
  EXPECT_EQ(ToInteger(3), 5);
  EXPECT_EQ(ToInteger(4), 4);
  Pop(4);
}

int push42_f(lua_State* L) {
  lua_pushinteger(L, 42);
  lua_pushinteger(L, 43);
  return 1;
}

TEST_F(ContextTest, ElementQueryCreation) {
  EXPECT_EQ(StackSize(), 0);
  EXPECT_TRUE(GetType(1) == Type::NONE);

  PushNil();
  EXPECT_TRUE(GetType(1) == Type::NIL);
  EXPECT_EQ(StackSize(), 1);
  Pop();

  PushBoolean(true);
  PushBoolean(false);
  EXPECT_TRUE(GetType(1) == Type::BOOLEAN);
  EXPECT_TRUE(ToBoolean(1));
  EXPECT_TRUE(GetType(2) == Type::BOOLEAN);
  EXPECT_FALSE(ToBoolean(2));
  Pop(2);

  static constexpr int64 hi64 = std::numeric_limits<int64>::max();
  static constexpr int64 lo64 = std::numeric_limits<int64>::min();

  for (int64 i : std::vector<int64>{0, 42, -42, hi64, lo64, hi64 - 1, hi64 - 2,
                                    lo64 + 1, lo64 + 2}) {
    PushInteger(i);
    EXPECT_TRUE(GetType(1) == Type::INTEGER);
    EXPECT_EQ(ToInteger(1), i);
    Pop();
  }

  for (float64 f : std::vector<float64>{0, 0.1, 0.24}) {
    PushFloat(f);
    EXPECT_TRUE(GetType(1) == Type::FLOAT);
    EXPECT_EQ(ToFloat(1), f);
    Pop();
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
    PushString(s);
    EXPECT_TRUE(GetType(1) == Type::STRING);
    EXPECT_EQ(ToString(1), s);
    Pop();
  }

  string foo0bar1("foo\0bar", 7);
  string foo0bar2("foo\0bar", 7);
  string foo0baz("foo\0baz", 7);
  PushString(foo0bar1);
  EXPECT_TRUE(GetType(1) == Type::STRING);
  EXPECT_EQ(ToString(1), foo0bar2);
  EXPECT_NE(ToString(1), foo0baz);
  Pop();

  int x;
  void* p = &x;
  PushLightUserdata(p);
  EXPECT_TRUE(GetType(1) == Type::LIGHT_USERDATA);
  EXPECT_TRUE(ToUserdata(1) == p);
  Pop();

  PushNewTable();
  PushNewTable(3, 0);
  PushNewTable(0, 3);
  PushNewTable(3, 3);
  for (int i = 1; i <= 4; i++) MUST(GetType(i) == Type::TABLE);
  Pop(4);

  void* userdata = PushNewUserdata(1 << 20);
  EXPECT_TRUE(GetType(1) == Type::USERDATA);
  EXPECT_TRUE(userdata);
  EXPECT_TRUE(userdata == ToUserdata(1));
  EXPECT_TRUE(userdata == ToPointer(1));
  Pop();

  EXPECT_EQ(StackSize(), 0);

  PushCFunction(push42_f);
  EXPECT_EQ(StackSize(), 1);
  EXPECT_TRUE(GetType(1) == Type::CFUNCTION);
  EXPECT_TRUE(ToCFunction(1) == push42_f);
  EXPECT_EQ(StackSize(), 1);
  Call(0, 1);
  EXPECT_EQ(StackSize(), 1);
  EXPECT_TRUE(GetType(1) == Type::INTEGER);
  EXPECT_EQ(ToInteger(1), 43);
  Pop(1);
}

TEST_F(ContextTest, Arithmetic) {
  PushInteger(2);
  PushInteger(3);
  ADD();
  EXPECT_TRUE(GetType(1) == Type::INTEGER);
  EXPECT_EQ(ToInteger(1), 5);
  Pop();

  PushInteger(5);
  PushInteger(3);
  SUB();
  EXPECT_TRUE(GetType(1) == Type::INTEGER);
  EXPECT_EQ(ToInteger(1), 2);
  Pop();

  PushInteger(5);
  PushInteger(3);
  MUL();
  EXPECT_TRUE(GetType(1) == Type::INTEGER);
  EXPECT_EQ(ToInteger(1), 15);
  Pop();

  PushInteger(9);
  PushInteger(4);
  DIV();
  EXPECT_TRUE(GetType(1) == Type::INTEGER);
  EXPECT_EQ(ToInteger(1), 2);
  Pop();

  PushFloat(9);
  PushFloat(4);
  DIV();
  EXPECT_TRUE(GetType(1) == Type::FLOAT);
  EXPECT_EQ(ToFloat(1), 2.25);
  Pop();

  PushInteger(9);
  PushInteger(4);
  MOD();
  EXPECT_TRUE(GetType(1) == Type::INTEGER);
  EXPECT_EQ(ToInteger(1), 1);
  Pop();

  PushFloat(9.75);
  PushFloat(3.75);
  MOD();
  EXPECT_TRUE(GetType(1) == Type::FLOAT);
  EXPECT_EQ(ToFloat(1), 2.25);
  Pop();

  PushInteger(10);
  NEG();
  EXPECT_TRUE(GetType(1) == Type::INTEGER);
  EXPECT_EQ(ToInteger(1), -10);
  Pop();

  const int64 xforneg = 0xF8A8'C2D1'924F'01AC;
  PushInteger(xforneg);
  BNOT();
  EXPECT_TRUE(GetType(1) == Type::INTEGER);
  EXPECT_EQ(ToInteger(1), ~xforneg);
  Pop();

  PushInteger(0b1010'0011);
  PushInteger(0b0010'1010);
  BAND();
  EXPECT_TRUE(GetType(1) == Type::INTEGER);
  EXPECT_EQ(ToInteger(1), 0b0010'0010);
  Pop();

  PushInteger(0b1010'0011);
  PushInteger(0b0010'1010);
  BOR();
  EXPECT_TRUE(GetType(1) == Type::INTEGER);
  EXPECT_EQ(ToInteger(1), 0b1010'1011);
  Pop();

  PushInteger(0b1010'0011);
  PushInteger(0b0010'1010);
  XOR();
  EXPECT_TRUE(GetType(1) == Type::INTEGER);
  EXPECT_EQ(ToInteger(1), 0b1000'1001);
  Pop();

  PushInteger(0b1010'0011);
  PushInteger(2);
  SHL();
  EXPECT_TRUE(GetType(1) == Type::INTEGER);
  EXPECT_EQ(ToInteger(1), 0b1010'0011'00);
  Pop();

  PushInteger(0b1010'0011);
  PushInteger(2);
  SHR();
  EXPECT_TRUE(GetType(1) == Type::INTEGER);
  EXPECT_EQ(ToInteger(1), 0b1010'00);
  Pop();
}

TEST_F(ContextTest, Comparison) {
  PushInteger(3);
  PushInteger(3);
  PushInteger(4);
  PushInteger(2);

  EXPECT_TRUE(EQ(1, 2));
  EXPECT_FALSE(EQ(1, 3));
  EXPECT_FALSE(NE(1, 2));
  EXPECT_TRUE(NE(1, 3));
  EXPECT_TRUE(LT(2, 3));
  EXPECT_FALSE(LT(1, 2));
  EXPECT_FALSE(LT(1, 4));
  EXPECT_TRUE(GT(1, 4));
  EXPECT_FALSE(GT(1, 2));
  EXPECT_FALSE(GT(1, 3));
  EXPECT_TRUE(GE(1, 4));
  EXPECT_TRUE(GE(1, 2));
  EXPECT_FALSE(GE(1, 3));
  EXPECT_TRUE(LE(2, 3));
  EXPECT_TRUE(LE(1, 2));
  EXPECT_FALSE(LE(1, 4));

  Pop(4);
}

TEST_F(ContextTest, Strings) {
  PushString("foo");
  PushString("barr");
  LEN(1);
  LEN(2);

  EXPECT_EQ(ToInteger(3), 3);
  EXPECT_EQ(ToInteger(4), 4);
  Pop(2);

  PushString("baz");
  Concat(3);
  EXPECT_EQ(ToString(1), "foobarrbaz");

  Pop(1);
}

[[noreturn]] int protected_test_function(lua_State* L) {
  lua_pushlstring(L, "fail", 4);
  lua_error(L);
}

TEST_F(ContextTest, Calls) {
  LoadFromString("x,y = ...; return x + y;", "test");
  PushInteger(2);
  PushInteger(3);
  Call(2, 1);
  EXPECT_EQ(ToInteger(1), 5);
  Pop();

  LoadFromString("return ...;", "test");
  PushInteger(1);
  PushInteger(2);
  PushInteger(3);
  CallMultret(3);
  EXPECT_EQ(ToInteger(1), 1);
  EXPECT_EQ(ToInteger(2), 2);
  EXPECT_EQ(ToInteger(3), 3);
  Pop(3);

  PushCFunction(protected_test_function);
  EXPECT_ANY_THROW(CallProtected(0, 0));
}

TEST_F(ContextTest, LoadSave) {
  LoadFromString("x,y = ...; return x + y;", "test", State::ChunkFormat::TEXT);
  string saved = SaveToString();
  Pop();
  EXPECT_EQ(StackSize(), 0);
  LoadFromString(saved, "test", State::ChunkFormat::BINARY);
  PushInteger(2);
  PushInteger(3);
  Call(2, 1);
  EXPECT_EQ(ToInteger(1), 5);
  Pop();
}

TEST_F(ContextTest, GCSmoke) {
  GCStop();
  GCRestart();
  GCCollect();
  PushNewUserdata(10'000);
  EXPECT_GE(GCCount(), 10'000);
  Pop();
  GCCollect();
  EXPECT_LT(GCCount(), 10'000);
  GCSetPause(true);
  GCSetPause(false);
  GCSetStepMultiplier(10);
  EXPECT_TRUE(GCIsRunning());
  GCStop();
  EXPECT_FALSE(GCIsRunning());
}

TEST_F(ContextTest, Tables) {
  LoadFromString("y = 2 * x;");
  EXPECT_EQ(StackSize(), 1);
  PushGlobalTable();
  EXPECT_EQ(StackSize(), 2);
  PushString("x");
  EXPECT_EQ(StackSize(), 3);
  PushInteger(21);
  EXPECT_EQ(StackSize(), 4);
  PopField(2);
  EXPECT_EQ(StackSize(), 2);
  Pop();
  EXPECT_EQ(StackSize(), 1);
  Call(0, 0);
  EXPECT_EQ(StackSize(), 0);
  PushGlobalTable();
  EXPECT_EQ(StackSize(), 1);
  PushString("y");
  EXPECT_EQ(StackSize(), 2);
  PushField(1);
  EXPECT_EQ(StackSize(), 2);
  EXPECT_EQ(ToInteger(2), 42);
  Pop(2);
}

TEST_F(ContextTest, Uservalue) {
  PushNewUserdata(1);
  PushInteger(42);
  PopUservalue(1);
  EXPECT_EQ(StackSize(), 1);
  PushUservalue(1);
  EXPECT_EQ(ToInteger(2), 42);
  Pop(2);
}

TEST_F(ContextTest, Metatable) {
  PushString("foo");  // 1
  EXPECT_FALSE(PushMetatable(1));
  PushNewTable();  // 2
  PushString("__index");
  PushNewTable();  // 4
  PushString("bar");
  LoadFromString("self = ...; baz = self;");
  PopField(4);
  PopField(2);
  PopMetatable(1);
  Pop();

  LoadFromString("local s = \"qux\"; return s:bar();");
  Call(0, 0);
  PushGlobalTable();
  PushString("baz");
  PushField(1);
  EXPECT_EQ(ToString(2), "qux");
  Pop(2);
}

}  // namespace
}  // namespace cm
