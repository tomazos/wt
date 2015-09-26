#pragma once

#include <algorithm>
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

class Regex {
 public:
  // Construction Options
  // PCRE2_ANCHORED           Force pattern anchoring
  // PCRE2_ALT_BSUX           Alternative handling of \u, \U, and \x
  // PCRE2_ALT_CIRCUMFLEX     Alternative handling of ^ in multiline mode
  // PCRE2_AUTO_CALLOUT       Compile automatic callouts
  // PCRE2_CASELESS           Do caseless matching
  // PCRE2_DOLLAR_ENDONLY     $ not to match newline at end
  // PCRE2_DOTALL             . matches anything including NL
  // PCRE2_DUPNAMES           Allow duplicate names for subpatterns
  // PCRE2_EXTENDED           Ignore white space and # comments
  // PCRE2_FIRSTLINE          Force matching to be before newline
  // PCRE2_MATCH_UNSET_BACKREF  Match unset back references
  // PCRE2_MULTILINE          ^ and $ match newlines within data
  // PCRE2_NEVER_BACKSLASH_C  Lock out the use of \C in patterns
  // PCRE2_NEVER_UCP          Lock out PCRE2_UCP, e.g. via (*UCP)
  // PCRE2_NEVER_UTF          Lock out PCRE2_UTF, e.g. via (*UTF)
  // PCRE2_NO_AUTO_CAPTURE    Disable numbered capturing paren-
  //                          theses (named ones available)
  // PCRE2_NO_AUTO_POSSESS    Disable auto-possessification
  // PCRE2_NO_DOTSTAR_ANCHOR  Disable automatic anchoring for .*
  // PCRE2_NO_START_OPTIMIZE  Disable match-time start optimizations
  // PCRE2_NO_UTF_CHECK       Do not check the pattern for UTF validity
  //                           (only relevant if PCRE2_UTF is set)
  // PCRE2_UCP                Use Unicode properties for \d, \w, etc.
  // PCRE2_UNGREEDY           Invert greediness of quantifiers
  // PCRE2_UTF                Treat pattern and subjects as UTF strings
  Regex(string_view pattern, uint32 options = 0);

  bool Matches(string_view subject);

  template <typename F>
  [[gnu::warn_unused_result]] bool Capture(string_view subject,
                                           F capture_function);

  template <typename F>
  size_t Scan(string_view subject, F capture_function);

  class Results;

 private:
  //  PCRE2_ANCHORED          Match only at the first position
  //  PCRE2_NOTBOL            Subject string is not the beginning of a line
  //  PCRE2_NOTEOL            Subject string is not the end of a line
  //  PCRE2_NOTEMPTY          An empty string is not a valid match
  //  PCRE2_NOTEMPTY_ATSTART  An empty string at the start of the subject
  //                           is not a valid match
  //  PCRE2_NO_UTF_CHECK      Do not check the subject for UTF
  //                           validity (only relevant if PCRE2_UTF
  //                           was set at compile time)
  //  PCRE2_PARTIAL_SOFT      Return PCRE2_ERROR_PARTIAL for a partial
  //                            match if no full matches are found
  //  PCRE2_PARTIAL_HARD      Return PCRE2_ERROR_PARTIAL for a partial match
  //                           if that is found before a full match
  Results Match(string_view subject, size_t startoffset, uint32 options);

  std::unique_ptr<pcre2_code, void (*)(pcre2_code*)> code_;
};

class Regex::Results {
 public:
  size_t size() const;

  string_view operator[](size_t pos) const;

 private:
  Results(pcre2_match_data* md);

  std::unique_ptr<pcre2_match_data, void (*)(pcre2_match_data*)> md_;
  string_view subject_;
  int pcre2_match_result_;

  friend class Regex;
};

template <typename F>
bool Regex::Capture(string_view subject, F capture_function) {
  Results results = Match(subject, 0 /* startoffset */, 0 /* options */);
  if (results.size() > 0) {
    capture_function(std::move(results));
    return true;
  } else
    return false;
}

template <typename F>
size_t Regex::Scan(string_view subject, F capture_function) {
  size_t hits = 0;
  size_t pos = 0;
  while (pos < subject.size()) {
    Results results = Match(subject, pos, 0 /* options */);
    if (results.size() == 0) break;
    const size_t begin_match = results[0].data() - subject.data();
    const size_t end_match = begin_match + results[0].size();
    capture_function(std::move(results));
    pos = std::max({pos + 1, begin_match, end_match});
    ++hits;
  }
  return hits;
}
