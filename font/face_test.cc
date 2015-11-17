#include "font/face.h"

#include "core/env.h"
#include "core/must.h"
#include "core/file_functions.h"
#include "gtest/gtest.h"

namespace font {

TEST(FaceTest, Open) {
  filesystem::path source_root = GetEnv("SOURCE_ROOT");
  filesystem::path free_serif_ttf =
      source_root / "font" / "testdata" / "FreeSerif.ttf";
  Library library;
  Face face(library, free_serif_ttf, 0);
  LOGEXPR(face->num_glyphs);
}

}  // namespace font
