#include "font/glyph.h"

#include "core/env.h"
#include "core/must.h"
#include "core/file_functions.h"
#include "font/face.h"
#include "gtest/gtest.h"

namespace font {

class GlyphTest : public testing::Test {
 public:
  GlyphTest()
      : face(library, filesystem::path(GetEnv("SOURCE_ROOT")) / "font" /
                          "testdata" / "FreeSerif.ttf",
             0) {}

  Library library;
  Face face;
};

TEST_F(GlyphTest, Smoke) {
  face.SetPixelSize(160, 160);
  face.LoadChar('A');
  Glyph glyph = face.GetGlyph();
  Glyph glyph2(glyph);
}

TEST_F(GlyphTest, Render) {
  face.SetPixelSize(160, 160);
  face.LoadChar('A', FT_LOAD_DEFAULT);
  Glyph glyph = face.GetGlyph();
  EXPECT_TRUE(glyph.IsOutline());
  EXPECT_FALSE(glyph.IsBitmap());
  Glyph glyph2(glyph);
  glyph.ConvertToBitmap();
  EXPECT_FALSE(glyph.IsOutline());
  EXPECT_TRUE(glyph.IsBitmap());
  EXPECT_TRUE(glyph2.IsOutline());
  EXPECT_FALSE(glyph2.IsBitmap());
}

}  // namespace font
