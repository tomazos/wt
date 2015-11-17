#pragma once

#include <boost/filesystem.hpp>

#include "font/library.h"
#include "font/object.h"

namespace font {

class Glyph;

class Face {
 public:
  Face(const Library& library, filesystem::path filepathname,
       FT_Long face_index);

  const FT_FaceRec* operator->() { return face_; }

  void SetCharSize(FT_F26Dot6 char_width, FT_F26Dot6 char_height,
                   FT_UInt horz_resolution, FT_UInt vert_resolution);

  void SetPixelSize(FT_UInt pixel_width, FT_UInt pixel_height);

  void LoadChar(FT_ULong char_code, FT_Int32 load_flags = FT_LOAD_RENDER);
  FT_UInt GetCharIndex(FT_ULong char_code);

  void LoadGlyph(FT_UInt glyph_index, FT_Int32 load_flags = FT_LOAD_DEFAULT);
  void RenderGlyph(FT_Render_Mode render_mode);

  FT_Vector GetKerning(FT_UInt left_glyph, FT_UInt right_glyph,
                       FT_UInt kern_mode = FT_KERNING_DEFAULT);

  Glyph GetGlyph();

 private:
  Object<FT_Face, FT_Done_Face> face_;
};

}  // namespace font
