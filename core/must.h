#pragma once

#include <iostream>
#include <system_error>
#include <utility>

#include "core/error_class.h"
#include "core/string_functions.h"
#include "whee/source_root_sentinal.h"

#define THROW_ERRNO(...)                                 \
  throw std::system_error(errno, std::system_category(), \
                          EncodeAsString(__VA_ARGS__, " (", errno, ")"))

#define LOG(...) LogImpl(__FILE__, __LINE__, ##__VA_ARGS__)
#define LOGEXPR(expr) DumpExprImpl(__FILE__, __LINE__, #expr, (expr))
#define FAIL(...) FailImpl(__FILE__, __LINE__, ##__VA_ARGS__)
#define MUST(condition, ...) \
  MustImpl(bool(condition), #condition, __FILE__, __LINE__, ##__VA_ARGS__)
#define MUST_EQ(expected, observed, ...)                                      \
  MustEqualImpl(expected, observed, #expected, #observed, __FILE__, __LINE__, \
                ##__VA_ARGS__)
#define MUST_NE(expected, observed, ...)                               \
  MustNotEqualImpl(expected, observed, #expected, #observed, __FILE__, \
                   __LINE__, ##__VA_ARGS__)
#define MUST_LT(lhs, rhs, ...) \
  MustLessThanImpl(lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, ##__VA_ARGS__)
#define MUST_GT(lhs, rhs, ...) \
  MustGreaterThanImpl(lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, ##__VA_ARGS__)
#define MUST_LE(lhs, rhs, ...) \
  MustLessEqualImpl(lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, ##__VA_ARGS__)
#define MUST_GE(lhs, rhs, ...) \
  MustGreaterEqualImpl(lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, ##__VA_ARGS__)

template <typename... Args>
string EncodeSourceMessage(const char* file, int64 line, Args&&... args) {
  return EncodeAsString(ReplaceSourceRootSentinal(file), ":", line, ":", 1,
                        ": ", std::forward<Args>(args)...);
}

template <typename... Args>
void LogImpl(const char* file, int64 line, Args&&... args) {
  std::cout << EncodeSourceMessage(file, line, std::forward<Args>(args)...)
            << std::endl;
}

template <typename Expr>
void DumpExprImpl(const char* file, int64 line, const char* expr_string,
                  Expr&& expr) {
  LogImpl(file, line, expr_string, " = ", std::forward<Expr>(expr));
}

template <typename... Args>
[[noreturn]] void ThrowError(const char* file, int64 line, Args&&... args) {
  throw Error(
      EncodeSourceMessage(file, line, "error: ", std::forward<Args>(args)...));
}

template <typename... Args>
[[noreturn]] void FailImpl(const char* file, int64 line, Args&&... args) {
  ThrowError(file, line, "FAIL()", (sizeof...(Args) > 0 ? ": " : ""),
             std::forward<Args>(args)...);
}

template <typename... Args>
void MustImpl(bool condition_bool, const char* condition_string,
              const char* file, int64 line, Args&&... args) {
  if (!condition_bool) {
    ThrowError(file, line, "MUST(", condition_string, ")",
               (sizeof...(Args) > 0 ? ": " : ""), std::forward<Args>(args)...);
  }
}

template <typename A, typename B, typename... Args>
void MustEqualImpl(const A& a, const B& b, const char* a_string,
                   const char* b_string, const char* file, int64 line,
                   Args&&... args) {
  if (a != b) {
    ThrowError(file, line, "MUST_EQ(", a_string, " (", EncodeAsString(a), "), ",
               b_string, " (", EncodeAsString(b), "))",
               (sizeof...(Args) > 0 ? ": " : ""), std::forward<Args>(args)...);
  }
}

template <typename A, typename B, typename... Args>
void MustNotEqualImpl(const A& a, const B& b, const char* a_string,
                      const char* b_string, const char* file, int64 line,
                      Args&&... args) {
  if (a == b) {
    ThrowError(file, line, "MUST_NE(", a_string, " (", EncodeAsString(a), "), ",
               b_string, " (", EncodeAsString(b), "))",
               (sizeof...(Args) > 0 ? ": " : ""), std::forward<Args>(args)...);
  }
}

template <typename A, typename B, typename... Args>
void MustLessThanImpl(const A& a, const B& b, const char* a_string,
                      const char* b_string, const char* file, int64 line,
                      Args&&... args) {
  if (a >= b) {
    ThrowError(file, line, "MUST_LT(", a_string, " (", EncodeAsString(a), "), ",
               b_string, " (", EncodeAsString(b), "))",
               (sizeof...(Args) > 0 ? ": " : ""), std::forward<Args>(args)...);
  }
}

template <typename A, typename B, typename... Args>
void MustGreaterThanImpl(const A& a, const B& b, const char* a_string,
                         const char* b_string, const char* file, int64 line,
                         Args&&... args) {
  if (a <= b) {
    ThrowError(file, line, "MUST_GT(", a_string, " (", EncodeAsString(a), "), ",
               b_string, " (", EncodeAsString(b), "))",
               (sizeof...(Args) > 0 ? ": " : ""), std::forward<Args>(args)...);
  }
}

template <typename A, typename B, typename... Args>
void MustLessEqualImpl(const A& a, const B& b, const char* a_string,
                       const char* b_string, const char* file, int64 line,
                       Args&&... args) {
  if (a > b) {
    ThrowError(file, line, "MUST_LE(", a_string, " (", EncodeAsString(a), "), ",
               b_string, " (", EncodeAsString(b), "))",
               (sizeof...(Args) > 0 ? ": " : ""), std::forward<Args>(args)...);
  }
}

template <typename A, typename B, typename... Args>
void MustGreaterEqualImpl(const A& a, const B& b, const char* a_string,
                          const char* b_string, const char* file, int64 line,
                          Args&&... args) {
  if (a < b) {
    ThrowError(file, line, "MUST_GE(", a_string, " (", EncodeAsString(a), "), ",
               b_string, " (", EncodeAsString(b), "))",
               (sizeof...(Args) > 0 ? ": " : ""), std::forward<Args>(args)...);
  }
}
