#include "font/library.h"

namespace font {

Library::Library() : library_(FT_Init_FreeType) {}

}  // namespace font
