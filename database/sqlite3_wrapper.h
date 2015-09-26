#pragma once

#include <sqlite3.h>

#include "core/must.h"

#define S3CALL(operation, ...)                     \
  do {                                             \
    int result = sqlite3_##operation(__VA_ARGS__); \
    if (result != SQLITE_OK) {                     \
      const char* errstr = sqlite3_errstr(result); \
      FAIL(#operation, ": ", errstr);              \
    }                                              \
  } while (false)
