#pragma once

#include <exception>

class Error : public std::exception {
 public:
  Error(string error_message) : error_message_(std::move(error_message)) {}

  const char* what() const noexcept override { return error_message_.c_str(); }

 private:
  string error_message_;
};
