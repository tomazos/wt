#include "core/env.h"

#include "core/must.h"

bool HasEnv(const string& name) { return getenv(name.c_str()); }

string GetEnv(const string& name) {
  char* getenv_result = getenv(name.c_str());
  if (getenv_result == nullptr) {
    FAIL("no such environment variable: ", name);
  }
  return getenv_result;
}

void SetEnv(const string& name, const string& value, bool overwrite) {
  int setenv_result = setenv(name.c_str(), value.c_str(), overwrite);
  if (setenv_result != 0)
    THROW_ERRNO("setenv(", name, ",", value, ",", overwrite, ")");
}
