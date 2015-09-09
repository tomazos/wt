#include "cryptography/proof_of_work.h"

#include "gtest/gtest.h"

TEST(ProofOfWorkTest, Simple) {
  auto proof_of_work = Work("Hello, World!", 2, "" /* init_suffix */, 10);
  ASSERT_TRUE(proof_of_work);
  ASSERT_TRUE(Verify(*proof_of_work));
  ASSERT_FALSE(Work("", 2, "", 0));
  ASSERT_FALSE(Work("foo", 32, "bar", 2));
}
