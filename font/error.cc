#include "font/error.h"

#include <map>

#include "core/collection_functions.h"

namespace font {

const char* ErrorToString(FT_Error error) {
#undef __FTERRORS_H__
#define FT_ERRORDEF(e, v, s) \
  { e, s }                   \
  ,
#define FT_ERROR_START_LIST {
#define FT_ERROR_END_LIST \
  }                       \
  ;

  static std::map<int, const char*> error_strings =

#include FT_ERRORS_H

      auto error_message = FindOrNull(error_strings, error);
  if (error_message)
    return *error_message;
  else
    return "unknown error";
}

void ThrowOnError(FT_Error error) {
  if (error != 0) throw std::runtime_error(ErrorToString(error));
}

}  // namespace font
