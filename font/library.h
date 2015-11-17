#pragma once

#include "font/object.h"

namespace font {

class Library {
 public:
  Library();

  operator FT_Library() const { return library_; }

 private:
  Object<FT_Library, FT_Done_FreeType> library_;
};

}  // namespace font
