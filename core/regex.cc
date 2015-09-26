#include "core/regex.h"

#include "core/must.h"

namespace {

string GetErrorMessage(int errorcode) {
  constexpr size_t bufflen = 256;
  unsigned char buffer[bufflen];

  int result = pcre2_get_error_message(errorcode, buffer, bufflen);
  if (result >= 0)
    return std::string((const char*)buffer, result);
  else
    return std::to_string(errorcode);
}

pcre2_code* checked_pcre2_compile(string_view pattern, uint32 options) {
  int errorcode;
  size_t erroroffset;
  pcre2_code* code =
      pcre2_compile((const unsigned char*)pattern.data(), pattern.size(),
                    options, &errorcode, &erroroffset, nullptr);
  if (!code) {
    const string error_message = "pcre2_compile: " + GetErrorMessage(errorcode);
    size_t start_code_sample = (erroroffset > 20 ? erroroffset - 20 : 0);
    const string code_sample =
        pattern.substr(start_code_sample, 40).to_string();
    const string marker_string =
        std::string(erroroffset - start_code_sample, ' ') + '^';
    FAIL(error_message, "\n", code_sample, "\n", marker_string);
  }
  return code;
}

}  // namespace

Regex::Regex(string_view pattern, uint32 options)
    : code_(checked_pcre2_compile(pattern, options), pcre2_code_free) {}

bool Regex::Matches(string_view subject) {
  Results results = Match(subject, 0 /* startoffset */, 0 /* options */);
  return results.size() > 0;
}

Regex::Results Regex::Match(string_view subject, size_t startoffset,
                            uint32 options) {
  Results result(pcre2_match_data_create_from_pattern(code_.get(), nullptr));
  result.subject_ = subject;
  result.pcre2_match_result_ = pcre2_match(
      code_.get(), (const unsigned char*)subject.data(), subject.size(),
      startoffset, options, result.md_.get(), nullptr);

  if (result.pcre2_match_result_ < 0 &&
      result.pcre2_match_result_ != PCRE2_ERROR_NOMATCH)
    FAIL("pcre2_match: ", GetErrorMessage(result.pcre2_match_result_));
  return result;
}

Regex::Results::Results(pcre2_match_data* md)
    : md_(md, pcre2_match_data_free) {}

size_t Regex::Results::size() const {
  if (pcre2_match_result_ < 0)
    return 0;
  else
    return pcre2_match_result_;
}

string_view Regex::Results::operator[](size_t pos) const {
  MUST_LT(pos, size());
  size_t* ovector = pcre2_get_ovector_pointer(md_.get());
  size_t begin_pos = ovector[2 * pos + 0];
  size_t end_pos = ovector[2 * pos + 1];
  MUST_LE(begin_pos, end_pos);
  return subject_.substr(begin_pos, end_pos - begin_pos);
}
