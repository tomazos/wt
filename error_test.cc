#include "error.h"

#include "testing.h"

void TestMain() {
  Error error("foo");
  MUST_EQ("foo", string(error.what()));
}
