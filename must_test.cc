#include "must.h"

#include "testing.h"

void TestMain() {
  try {
    FAIL("fail");
    std::exit(1);
  } catch (const Error& error) {
  }

  MUST(true);

  try {
    MUST(false);
    std::exit(1);
  } catch (const Error& error) {
  }

  MUST_EQ(3, 6 / 2);

  try {
    MUST_EQ(3, 4 / 2);
    std::exit(1);
  } catch (const Error& error) {
  }

  MUST_NE(3, 4 / 2);

  try {
    MUST_NE(3, 6 / 2);
    std::exit(1);
  } catch (const Error& error) {
  }

  MUST_LT(2, 3);

  try {
    MUST_LT(3, 3);
    std::exit(1);
  } catch (const Error& error) {
  }

  try {
    MUST_LT(3, 2);
    std::exit(1);
  } catch (const Error& error) {
  }

  MUST_GT(3, 2);

  try {
    MUST_GT(3, 3);
    std::exit(1);
  } catch (const Error& error) {
  }

  try {
    MUST_GT(2, 3);
    std::exit(1);
  } catch (const Error& error) {
  }

  MUST_LE(2, 3);
  MUST_LE(3, 3);

  try {
    MUST_LE(3, 2);
    std::exit(1);
  } catch (const Error& error) {
  }

  MUST_GE(3, 2);
  MUST_GE(3, 3);

  try {
    MUST_GE(2, 3);
    std::exit(1);
  } catch (const Error& error) {
  }
}
