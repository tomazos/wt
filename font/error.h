#pragma once

#include "font/freetype.h"

namespace font {

const char* ErrorToString(FT_Error);

void ThrowOnError(FT_Error);

}  // namespace font
