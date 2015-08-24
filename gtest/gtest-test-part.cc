// The Google C++ Testing Framework (Google Test)

#include "gtest/gtest-test-part.h"

// Indicates that this translation unit is part of Google Test's
// implementation.  It must come before gtest-internal-inl.h is
// included, or there will be a compiler error.  This trick exists to
// prevent the accidental inclusion of gtest-internal-inl.h in the
// user's code.
#define GTEST_IMPLEMENTATION_ 1
#include "gtest/gtest-internal-inl.h"
#undef GTEST_IMPLEMENTATION_

namespace testing {

using internal::GetUnitTestImpl;

// Gets the summary of the failure message by omitting the stack trace
// in it.
std::string TestPartResult::ExtractSummary(const char* message) {
  const char* const stack_trace = strstr(message, internal::kStackTraceMarker);
  return stack_trace == NULL ? message : std::string(message, stack_trace);
}

// Prints a TestPartResult object.
std::ostream& operator<<(std::ostream& os, const TestPartResult& result) {
  return os << result.file_name() << ":" << result.line_number() << ": "
            << (result.type() == TestPartResult::kSuccess
                    ? "Success"
                    : result.type() == TestPartResult::kFatalFailure
                          ? "Fatal failure"
                          : "Non-fatal failure") << ":\n" << result.message()
            << std::endl;
}

// Appends a TestPartResult to the array.
void TestPartResultArray::Append(const TestPartResult& result) {
  array_.push_back(result);
}

// Returns the TestPartResult at the given index (0-based).
const TestPartResult& TestPartResultArray::GetTestPartResult(int index) const {
  if (index < 0 || index >= size()) {
    printf("\nInvalid index (%d) into TestPartResultArray.\n", index);
    internal::posix::Abort();
  }

  return array_[index];
}

// Returns the number of TestPartResult objects in the array.
int TestPartResultArray::size() const {
  return static_cast<int>(array_.size());
}

namespace internal {

HasNewFatalFailureHelper::HasNewFatalFailureHelper()
    : has_new_fatal_failure_(false),
      original_reporter_(
          GetUnitTestImpl()->GetTestPartResultReporterForCurrentThread()) {
  GetUnitTestImpl()->SetTestPartResultReporterForCurrentThread(this);
}

HasNewFatalFailureHelper::~HasNewFatalFailureHelper() {
  GetUnitTestImpl()->SetTestPartResultReporterForCurrentThread(
      original_reporter_);
}

void HasNewFatalFailureHelper::ReportTestPartResult(
    const TestPartResult& result) {
  if (result.fatally_failed()) has_new_fatal_failure_ = true;
  original_reporter_->ReportTestPartResult(result);
}

}  // namespace internal

}  // namespace testing
