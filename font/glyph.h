#pragma once

#include "font/object.h"

namespace font {

class Glyph {
 public:
  Glyph(const Glyph& original);

  Glyph& operator=(const Glyph& original);

  void ConvertToBitmap(FT_Render_Mode render_mode = FT_RENDER_MODE_NORMAL,
                       FT_Vector origin = {0, 0});

  bool IsBitmap() const;
  bool IsOutline() const;

  FT_Glyph_Format format() const { return glyph_->format; }
  FT_Vector advance() const { return glyph_->advance; }

  FT_BitmapGlyphRec& bitmap();
  const FT_BitmapGlyphRec& bitmap() const;
  FT_OutlineGlyphRec& outline();
  const FT_OutlineGlyphRec& outline() const;

 private:
  Glyph(FT_GlyphSlot slot);

  static FT_Error DoneGlyph(FT_Glyph glyph);

  Object<FT_Glyph, DoneGlyph> glyph_;

  friend class Face;
};

}  // namespace font
