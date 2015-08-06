#include "rand_float.h"

#include "testing.h"

void TestRandFloat() {
  float64 a = RandFloat();
  float64 b = RandFloat();
  MUST_GT(a, 0.0);
  MUST_LT(a, 1.0);
  MUST_GT(b, 0.0);
  MUST_LT(b, 1.0);
  MUST_NE(a, b);
}

void TestMain() {
  std::unique_ptr<std::thread> ts[100];
  for (auto& t : ts) t = std::make_unique<std::thread>(TestRandFloat);
  for (auto& t : ts) {
    MUST(t);
    t->join();
  }
}
