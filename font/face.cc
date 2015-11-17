#include "font/face.h"

#include "font/glyph.h"

namespace font {

Face::Face(const Library& library, filesystem::path filepathname,
           FT_Long face_index)
    : face_(FT_New_Face, library, filepathname.string().c_str(), face_index) {}

void Face::SetCharSize(FT_F26Dot6 char_width, FT_F26Dot6 char_height,
                       FT_UInt horz_resolution, FT_UInt vert_resolution) {
  ThrowOnError(FT_Set_Char_Size(face_, char_width, char_height, horz_resolution,
                                vert_resolution));
}

void Face::SetPixelSize(FT_UInt pixel_width, FT_UInt pixel_height) {
  ThrowOnError(FT_Set_Pixel_Sizes(face_, pixel_width, pixel_height));
}

FT_UInt Face::GetCharIndex(FT_ULong char_code) {
  return FT_Get_Char_Index(face_, char_code);
}

void Face::LoadChar(FT_ULong char_code, FT_Int32 load_flags) {
  ThrowOnError(FT_Load_Char(face_, char_code, load_flags));
}

void Face::LoadGlyph(FT_UInt glyph_index, FT_Int32 load_flags) {
  ThrowOnError(FT_Load_Glyph(face_, glyph_index, load_flags));
}

void Face::RenderGlyph(FT_Render_Mode render_mode) {
  ThrowOnError(FT_Render_Glyph(face_->glyph, render_mode));
}

FT_Vector Face::GetKerning(FT_UInt left_glyph, FT_UInt right_glyph,
                           FT_UInt kern_mode) {
  FT_Vector v;
  std::memset(&v, 0, sizeof(v));
  ThrowOnError(FT_Get_Kerning(face_, left_glyph, right_glyph, kern_mode, &v));
  return v;
}

Glyph Face::GetGlyph() { return {face_->glyph}; }

}  // namespace font
