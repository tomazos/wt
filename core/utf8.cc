#include "core/utf8.h"

#include "core/must.h"

enum Utf8DecodingState {
  START,
  STATE_1_OF_1,
  STATE_2_OF_2,
  STATE_1_OF_2,
  STATE_3_OF_3,
  STATE_2_OF_3,
  STATE_1_OF_3
};

bool IsValidUtf8(string_view utf8) {
  const char* cursor = utf8.data();
  const char* end = utf8.data() + utf8.size();

  Utf8DecodingState state = START;

  while (cursor != end) {
    const uint8 code_unit = (uint8)*cursor;
    cursor++;
    switch (state) {
      case START:
        if (code_unit >= 0b1000'0000) {
          if ((code_unit & 0b1110'0000) == 0b1100'0000)
            state = STATE_1_OF_1;
          else if ((code_unit & 0b1111'0000) == 0b1110'0000)
            state = STATE_2_OF_2;
          else if ((code_unit & 0b1111'1000) == 0b1111'0000)
            state = STATE_3_OF_3;
          else
            return false;
        }
        break;
      case STATE_1_OF_1:
      case STATE_1_OF_2:
      case STATE_1_OF_3:
        if ((code_unit & 0b1100'0000) != 0b1000'0000) return false;
        state = START;
        break;
      case STATE_2_OF_2:
        if ((code_unit & 0b1100'0000) != 0b1000'0000) return false;
        state = STATE_1_OF_2;
        break;
      case STATE_3_OF_3:
        if ((code_unit & 0b1100'0000) != 0b1000'0000) return false;
        state = STATE_2_OF_3;
        break;
      case STATE_2_OF_3:
        if ((code_unit & 0b1100'0000) != 0b1000'0000) return false;
        state = STATE_1_OF_3;
        break;
      default:
        FAIL();
    }
  }
  return state == START;
}

string ConvertLatin1ToUtf8(string_view latin1) {
  std::ostringstream oss;
  for (char c : latin1) {
    uint8 i = c;
    if (i < 0b1000'0000)
      oss << c;
    else
      oss << (char)((i >> 6) | 0b1100'0000)
          << (char)((i & 0b0011'1111) | 0b1000'0000);
  }
  return oss.str();
}
