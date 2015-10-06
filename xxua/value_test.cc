#include "xxua/value.h"

#include "xxua/context.h"
#include "xxua/debug_allocator.h"

#include "gtest/gtest.h"

namespace xxua {

struct ValueTest : testing::Test {
  ValueTest() : state(allocator), context(state) {}
  ~ValueTest() {
    PushNil();
    while (Next(State::REGISTRY)) {
      EXPECT_FALSE(GetType(-2) == Type::LIGHT_USERDATA);
      Pop();
    }
    EXPECT_EQ(StackSize(), 0);
  }

  DebugAllocator allocator;
  State state;
  Context context;
};

TEST_F(ValueTest, DefaultConstruction) {
  Value v;
  EXPECT_TRUE(v.type() == Type::NIL);
  EXPECT_TRUE(v.empty());
}

TEST_F(ValueTest, BooleanConstruction) {
  Value t = true;
  EXPECT_TRUE(t.type() == Type::BOOLEAN);
  EXPECT_TRUE(t);
  EXPECT_FALSE(t.empty());

  Value f = false;
  EXPECT_TRUE(f.type() == Type::BOOLEAN);
  EXPECT_FALSE(f);
  EXPECT_FALSE(f.empty());
}

template <typename T>
void IntegerConstructionTest() {
  T i(42);
  Value v = i;
  EXPECT_FALSE(v.empty());
  EXPECT_TRUE(v.type() == Type::INTEGER);
  T j = v;
  EXPECT_EQ(i, j);
}

TEST_F(ValueTest, IntegerConstruction) {
  IntegerConstructionTest<char>();
  IntegerConstructionTest<int8>();
  IntegerConstructionTest<uint8>();
  IntegerConstructionTest<int16>();
  IntegerConstructionTest<uint16>();
  IntegerConstructionTest<int32>();
  IntegerConstructionTest<uint32>();
  IntegerConstructionTest<int64>();
  IntegerConstructionTest<uint64>();
}

template <typename T>
void FloatConstructionTest() {
  T f1(2.25);
  Value v = f1;
  EXPECT_FALSE(v.empty());
  EXPECT_TRUE(v.type() == Type::FLOAT);
  T f2 = v;
  EXPECT_EQ(f1, f2);
}

TEST_F(ValueTest, FloatConstruction) {
  FloatConstructionTest<float32>();
  FloatConstructionTest<float64>();
}

TEST_F(ValueTest, LightUserdataConstruction) {
  void* p1 = &p1;
  Value v = MakePointer(p1);
  EXPECT_FALSE(v.empty());
  EXPECT_TRUE(v.type() == Type::LIGHT_USERDATA);
  void* p2(v);
  EXPECT_EQ(p1, p2);
}

void StringConstructionTest(string_view sv) {
  Value v = sv;
  EXPECT_FALSE(v.empty());
  EXPECT_TRUE(v.type() == Type::STRING);
  string s(v);
  EXPECT_EQ(s, sv);
}

TEST_F(ValueTest, StringConstruction) {
  StringConstructionTest("");
  StringConstructionTest("foo");
  StringConstructionTest("bar");
  StringConstructionTest(string(1'000'000, 'x'));
  //  StringConstructionTest(string(5'000'000'000, 'x'));
}

void TableConstructionTest(std::initializer_list<std::pair<Value, Value>> il) {
  Value table(il);
  EXPECT_FALSE(table.empty());
  EXPECT_TRUE(table.type() == Type::TABLE);
  std::map<Value, Value> map;
  for (const auto& kv : il) {
    const Value& key = kv.first;
    const Value& val = kv.second;
    EXPECT_TRUE(table[key] == val);
    map.insert({key, val});
  }
  for (const auto& kv : table) {
    const Value& key = kv.first;
    const Value& val = kv.second;
    EXPECT_TRUE(map[key] == val);
  }
}

TEST_F(ValueTest, TableConstruction) {
  TableConstructionTest({{1, 2}, {3, 4}, {5, 6}});

  //  std::map<Value, Value> m;
  //  TableConstructionTest({{1, "foo"}, {"bar", 42.0}, {'x', (void*)nullptr}});
}

namespace {

std::vector<Value> reverse(const std::vector<Value>& args) {
  std::vector<Value> results = args;
  std::reverse(results.begin(), results.end());
  return results;
}

}  // namespace

TEST_F(ValueTest, FunctionConstruction) {
  Value f = MakeFunction(reverse);
  std::vector<Value> v = f({1, 2, 3});
  EXPECT_EQ(v.size(), 3u);
  EXPECT_TRUE((v == std::vector<Value>{3, 2, 1}));
}

TEST_F(ValueTest, Compile) {
  Value f = Compile(R"(

   x = 42;


          )");
  f({});
  for (auto kv : Global()) {
    LOGEXPR(kv.first);
    LOGEXPR(kv.second);
  }
  EXPECT_EQ(Global()[Value(string_view("x"))], 42);
}

}  // namespace xxua
