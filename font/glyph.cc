#include "font/glyph.h"

#include "core/must.h"

namespace font {

Glyph::Glyph(const Glyph& original) : glyph_(FT_Glyph_Copy, original.glyph_) {}

Glyph& Glyph::operator=(const Glyph& original) {
  Glyph temp(original);
  *this = std::move(temp);
  return *this;
}

void Glyph::ConvertToBitmap(FT_Render_Mode render_mode, FT_Vector origin) {
  ThrowOnError(FT_Glyph_To_Bitmap(glyph_.ptr(), render_mode, &origin,
                                  true /* destroy */));
}

bool Glyph::IsBitmap() const {
  return glyph_->format == FT_GLYPH_FORMAT_BITMAP;
}

bool Glyph::IsOutline() const {
  return glyph_->format == FT_GLYPH_FORMAT_OUTLINE;
}

FT_BitmapGlyphRec& Glyph::bitmap() {
  MUST(IsBitmap());
  FT_Glyph glyph = glyph_;
  FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
  MUST(bitmap_glyph);
  return *bitmap_glyph;
}

const FT_BitmapGlyphRec& Glyph::bitmap() const {
  MUST(IsBitmap());
  const FT_GlyphRec* glyph = glyph_;
  const FT_BitmapGlyphRec* bitmap_glyph = (const FT_BitmapGlyphRec*)glyph;
  MUST(bitmap_glyph);
  return *bitmap_glyph;
}

FT_OutlineGlyphRec& Glyph::outline() {
  MUST(IsOutline());
  FT_Glyph glyph = glyph_;
  FT_OutlineGlyph outline_glyph = (FT_OutlineGlyph)glyph;
  MUST(outline_glyph);
  return *outline_glyph;
}

const FT_OutlineGlyphRec& Glyph::outline() const {
  MUST(IsOutline());
  const FT_GlyphRec* glyph = glyph_;
  const FT_OutlineGlyphRec* outline_glyph = (const FT_OutlineGlyphRec*)glyph;
  MUST(outline_glyph);
  return *outline_glyph;
}

Glyph::Glyph(FT_GlyphSlot slot) : glyph_(FT_Get_Glyph, slot) {}

FT_Error Glyph::DoneGlyph(FT_Glyph glyph) {
  FT_Done_Glyph(glyph);
  return 0;
}

}  // namespace font
