#include "experimental/cppsrc/tokenizer.h"

#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <vector>

bool IsAlpha(int code_point);
bool IsDigit(int code_point);
bool IsOctDigit(int code_point);
bool IsHexDigit(int code_point);
bool IsSpace(int code_point);
bool IsValidCodePoint(int code_point);

int HexCharToValue(int c);
char ValueToHexChar(int c);
string HexDump(void* pdata, size_t nbytes);
inline string HexDump(const std::vector<uint8>& data) {
  return HexDump((void*)data.data(), data.size());
}

string EncodeUtf8(const std::vector<int>& input);
std::vector<int> DecodeUtf8(const string& input);

template <typename T>
inline std::vector<uint8> to_byte_vector(const T& t) {
  uint8* begin = (uint8*)&t;
  uint8* end = begin + sizeof(T);

  return {begin, end};
}

struct Utf8Decoder {
  std::vector<int> decode(int input);

 private:
  int x = 0;
  int y = 0;
};

std::u16string EncodeUtf16(const std::vector<int>& input);
std::vector<int> DecodeUtf16(const std::u16string& input);

bool IsAllowedIdentifierFirstCharacter(int c);
bool IsAllowedIdentifierBodyCharacter(int c);

bool IsSpace(int code_point) {
  if (code_point < 0 || code_point > 127) return false;

  return isspace(code_point);
}

bool IsDigit(int code_point) {
  if (code_point < 0 || code_point > 127) return false;

  return isdigit(code_point);
}

bool IsOctDigit(int code_point) {
  if (code_point < 0 || code_point > 127) return false;

  switch (code_point) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
      return true;
    default:
      return false;
  }
}

bool IsHexDigit(int code_point) {
  if (code_point < 0 || code_point > 127) return false;

  return isxdigit(code_point);
}

bool IsAlpha(int code_point) {
  if (code_point < 0 || code_point > 127) return false;

  return isalpha(code_point);
}

bool IsValidCodePoint(int code_point) {
  return (code_point >= 0 && code_point < 0xD800) ||
         (code_point >= 0xE000 && code_point < 0x110000);
}

int HexCharToValue(int c) {
  switch (c) {
    case '0':
      return 0;
    case '1':
      return 1;
    case '2':
      return 2;
    case '3':
      return 3;
    case '4':
      return 4;
    case '5':
      return 5;
    case '6':
      return 6;
    case '7':
      return 7;
    case '8':
      return 8;
    case '9':
      return 9;
    case 'A':
      return 10;
    case 'a':
      return 10;
    case 'B':
      return 11;
    case 'b':
      return 11;
    case 'C':
      return 12;
    case 'c':
      return 12;
    case 'D':
      return 13;
    case 'd':
      return 13;
    case 'E':
      return 14;
    case 'e':
      return 14;
    case 'F':
      return 15;
    case 'f':
      return 15;
    default:
      throw std::logic_error("HexCharToValue of nonhex char");
  }
}

char ValueToHexChar(int c) {
  switch (c) {
    case 0:
      return '0';
    case 1:
      return '1';
    case 2:
      return '2';
    case 3:
      return '3';
    case 4:
      return '4';
    case 5:
      return '5';
    case 6:
      return '6';
    case 7:
      return '7';
    case 8:
      return '8';
    case 9:
      return '9';
    case 10:
      return 'A';
    case 11:
      return 'B';
    case 12:
      return 'C';
    case 13:
      return 'D';
    case 14:
      return 'E';
    case 15:
      return 'F';
    default:
      throw std::logic_error("ValueToHexChar of nonhex value");
  }
}

string HexDump(void* pdata, size_t nbytes) {
  uint8* p = (uint8*)pdata;

  string s(nbytes * 2, '?');

  for (size_t i = 0; i < nbytes; i++) {
    s[2 * i + 0] = ValueToHexChar((p[i] & 0xF0) >> 4);
    s[2 * i + 1] = ValueToHexChar((p[i] & 0x0F) >> 0);
  }

  return s;
}

string EncodeUtf8(const std::vector<int>& input) {
  string output;

  for (int c : input) {
    if (c < 0 || c > 0x10FFFF) throw std::logic_error("invalid code point");

    if (c <= 0x7F)
      output += char(c);
    else if (c <= 0x7FF) {
      int b0 = (0xC0 | (c >> 6));
      int b1 = (0x80 | (c & ((1 << 6) - 1)));

      output += char(b0);
      output += char(b1);
    } else if (c <= 0xFFFF) {
      int b0 = (0xE0 | (c >> 12));
      int b1 = (0x80 | ((c >> 6) & ((1 << 6) - 1)));
      int b2 = (0x80 | (c & ((1 << 6) - 1)));

      output += char(b0);
      output += char(b1);
      output += char(b2);
    } else {
      int b0 = (0xF0 | (c >> 18));
      int b1 = (0x80 | ((c >> 12) & ((1 << 6) - 1)));
      int b2 = (0x80 | ((c >> 6) & ((1 << 6) - 1)));
      int b3 = (0x80 | (c & ((1 << 6) - 1)));

      output += char(b0);
      output += char(b1);
      output += char(b2);
      output += char(b3);
    }
  }

  return output;
}

std::vector<int> Utf8Decoder::decode(int input) {
  if (y == 0) {
    if (input == -1)
      return {-1};

    else if (input & 1 << 7) {
      if (!(input & 1 << 6))
        throw std::logic_error("utf8 trailing code unit (10xxxxxx) at start");

      else if (!(input & 1 << 5)) {
        x = input & ((1 << 5) - 1);
        y = 1;
      } else if (!(input & 1 << 4)) {
        x = input & ((1 << 4) - 1);
        y = 2;
      } else if (!(input & 1 << 3)) {
        x = input & ((1 << 3) - 1);
        y = 3;
      } else
        throw std::logic_error("utf8 invalid unit (111111xx)");

      return {};
    } else
      return {input};
  } else {
    if ((input == -1) || !(input & 1 << 7) || (input & 1 << 6))
      throw std::logic_error("utf8 expected trailing byte (10xxxxxx)");

    x <<= 6;
    x |= (input & ((1 << 6) - 1));
    y--;
    if (y == 0)
      return {x};
    else
      return {};
  }
}

std::vector<int> DecodeUtf8(const string& input) {
  std::vector<int> output;

  Utf8Decoder decoder;

  for (char c : input) {
    unsigned char code_unit = c;

    for (int code_point : decoder.decode(code_unit))
      output.push_back(code_point);
  }

  return output;
}

std::u16string EncodeUtf16(const std::vector<int>& input) {
  std::u16string s;

  for (int x : input) {
    if (x < 0 || (x >= 0xD800 && x < 0xE000) || x >= 0x110000)
      throw std::logic_error("utf16 encoding error: " + x);

    if (x < 0x10000)
      s += char16_t(x);
    else {
      int U = x - 0x10000;
      int W1 = 0xD800;
      int W2 = 0xDC00;

      W1 |= (U >> 10);
      W2 |= (U & ((1 << 10) - 1));

      s += char16_t(W1);
      s += char16_t(W2);
    }
  }

  return s;
}

std::vector<int> DecodeUtf16(const std::u16string& input) {
  std::vector<int> output;

  int state = 0;

  int W1 = 0, W2;

  for (char16_t c : input) {
    int x = (unsigned short int)c;

    switch (state) {
      case 0:
        if (x >= 0 && (x < 0xD800 || x >= 0xE000) && x < 0x110000)
          output.push_back(x);
        else if (x >= 0xD800 && x < 0xDC00) {
          W1 = x;
          state = 1;
        } else
          throw std::logic_error("invalid UTF-16 code unit: " +
                                 std::to_string(x));

        break;

      case 1:
        if (x >= 0xDC00 && x < 0xE000) {
          W2 = x;

          int U = (W1 & ((1 << 10) - 1)) << 10 | (W2 & ((1 << 10) - 1));
          output.push_back(U + 0x10000);

          state = 0;
        } else
          throw std::logic_error("invalud UTF-16 code units: " +
                                 std::to_string(W1) + " " + std::to_string(x));

        break;

      default:
        throw std::logic_error("DecodeUtf16 unexpected state");
    }
  }

  if (state != 0) throw std::logic_error("unterminated UTF-16 sequence");

  return output;
}

// See C++ Standard Annex E: Universal character names for indentifier
// characters
bool IsCharacterAllowed_E1(int c) {
  static const std::vector<std::pair<int, int>> ranges = {{0xA8, 0xA8},
                                                          {0xAA, 0xAA},
                                                          {0xAD, 0xAD},
                                                          {0xAF, 0xAF},
                                                          {0xB2, 0xB5},
                                                          {0xB7, 0xBA},
                                                          {0xBC, 0xBE},
                                                          {0xC0, 0xD6},
                                                          {0xD8, 0xF6},
                                                          {0xF8, 0xFF},
                                                          {0x100, 0x167F},
                                                          {0x1681, 0x180D},
                                                          {0x180F, 0x1FFF},
                                                          {0x200B, 0x200D},
                                                          {0x202A, 0x202E},
                                                          {0x203F, 0x2040},
                                                          {0x2054, 0x2054},
                                                          {0x2060, 0x206F},
                                                          {0x2070, 0x218F},
                                                          {0x2460, 0x24FF},
                                                          {0x2776, 0x2793},
                                                          {0x2C00, 0x2DFF},
                                                          {0x2E80, 0x2FFF},
                                                          {0x3004, 0x3007},
                                                          {0x3021, 0x302F},
                                                          {0x3031, 0x303F},
                                                          {0x3040, 0xD7FF},
                                                          {0xF900, 0xFD3D},
                                                          {0xFD40, 0xFDCF},
                                                          {0xFDF0, 0xFE44},
                                                          {0xFE47, 0xFFFD},
                                                          {0x10000, 0x1FFFD},
                                                          {0x20000, 0x2FFFD},
                                                          {0x30000, 0x3FFFD},
                                                          {0x40000, 0x4FFFD},
                                                          {0x50000, 0x5FFFD},
                                                          {0x60000, 0x6FFFD},
                                                          {0x70000, 0x7FFFD},
                                                          {0x80000, 0x8FFFD},
                                                          {0x90000, 0x9FFFD},
                                                          {0xA0000, 0xAFFFD},
                                                          {0xB0000, 0xBFFFD},
                                                          {0xC0000, 0xCFFFD},
                                                          {0xD0000, 0xDFFFD},
                                                          {0xE0000, 0xEFFFD}};

  auto it = std::upper_bound(
      ranges.begin(), ranges.end(), c,
      [](int a, const std::pair<int, int>& b) { return a < b.first; });
  if (it == ranges.begin())
    return false;
  else
    return (it - 1)->second >= c;
}

bool IsCharacterDisallowedInitially_E2(int c) {
  static const std::vector<std::pair<int, int>> ranges = {
      {0x300, 0x36F}, {0x1DC0, 0x1DFF}, {0x20D0, 0x20FF}, {0xFE20, 0xFE2F}};

  auto it = std::upper_bound(
      ranges.begin(), ranges.end(), c,
      [](int a, const std::pair<int, int>& b) { return a < b.first; });
  if (it == ranges.begin())
    return false;
  else
    return (it - 1)->second >= c;
}

bool IsAllowedIdentifierFirstCharacter(int c) {
  if (c <= 0)
    return false;
  else if (c <= 127)
    return IsAlpha(c) || c == '_';
  else
    return (IsCharacterAllowed_E1(c) && !IsCharacterDisallowedInitially_E2(c));
}

bool IsAllowedIdentifierBodyCharacter(int c) {
  if (c <= 0)
    return false;
  else if (c <= 127)
    return IsAlpha(c) || IsDigit(c) || c == '_';
  else
    return IsCharacterAllowed_E1(c);
}

bool IsCharacterLiteralEncodingPrefix(const string& s) {
  static std::unordered_set<string> S = {"u", "U", "L"};

  return S.count(s);
}

bool IsStringLiteralEncodingPrefix(const string& s) {
  static std::unordered_set<string> S = {"u8", "u", "U", "L"};

  return S.count(s);
}

bool IsRawStringLiteralPrefix(const string& s) {
  static std::unordered_set<string> S = {"u8R", "uR", "UR", "LR", "R"};

  return S.count(s);
}

bool IsDigraphKeyword(const string& s) {
  static std::unordered_set<string> S = {
      "new", "delete", "and", "and_eq", "bitand", "bitor", "compl",
      "not", "not_eq", "or",  "or_eq",  "xor",    "xor_eq"};

  return S.count(s);
}

bool IsSimpleEscapeChar(int c) {
  static std::unordered_set<int> simple_escape_chars = {
      '\'', '"', '?', '\\', 'a', 'b', 'f', 'n', 'r', 't', 'v'};

  return simple_escape_chars.count(c);
}

struct UniversalCharacterNameDecoder {
  std::vector<int> decode(int input) {
    switch (state) {
      case 0:
        if (input == '\\') {
          acc.clear();
          acc.push_back(input);
          state = 1;
          return {};
        } else
          return {input};

      case 1:
        acc.push_back(input);

        if (input == 'u') {
          digits = 4;
          state = 2;
          code_point = 0;
          return {};
        } else if (input == 'U') {
          digits = 8;
          state = 2;
          code_point = 0;
          return {};
        } else if (input == '\\') {
          state = 1;
          acc.clear();
          acc.push_back('\\');
          return {'\\'};
        } else {
          state = 0;
          return {'\\', input};
        }

      case 2:
        acc.push_back(input);

        if (!IsHexDigit(input)) {
          state = 0;
          return acc;
        } else {
          code_point = (code_point << 4) + HexCharToValue(input);
          digits--;

          if (digits == 0) {
            state = 0;

            return {code_point};
          } else
            return {};
        }

      default:
        throw std::logic_error("invalid state");
    }
  }

 private:
  int digits = 0;
  int state = 0;
  int code_point = 0;

  std::vector<int> acc;
};

struct TrigraphDecoder {
  std::vector<int> decode(int input) {
    static std::unordered_map<int, int> trigraphs = {{'=', '#'},
                                                     {'/', '\\'},
                                                     {'\'', '^'},
                                                     {'(', '['},
                                                     {')', ']'},
                                                     {'!', '|'},
                                                     {'<', '{'},
                                                     {'>', '}'},
                                                     {'-', '~'}};

    switch (state) {
      case 0:
        if (input == '?') {
          state = 1;
          return {};
        } else
          return {input};

      case 1:
        if (input == '?') {
          state = 2;
          return {};
        } else {
          state = 0;
          return {'?', input};
        }

      case 2:
        if (input == '?') {
          state = 2;
          return {'?'};
        } else {
          state = 0;

          auto it = trigraphs.find(input);
          if (it == trigraphs.end()) {
            return {'?', '?', input};
          } else
            return {it->second};
        }

      default:
        throw std::logic_error("invalid state");
    }
  }

 private:
  int state = 0;
};

struct LineSplicer {
  std::vector<int> decode(int input) {
    switch (state) {
      case 0:
        if (input == '\\') {
          state = 1;
          return {};
        } else
          return {input};

      case 1:
        if (input == '\n') {
          state = 0;
          return {};
        } else if (input == '\\')
          return {'\\'};
        else {
          state = 0;
          return {'\\', input};
        }

      default:
        throw std::logic_error("invalid state");
    }
  }

 private:
  int state = 0;
};

struct NewlineEnder {
  std::vector<int> decode(int input) {
    switch (state) {
      case 0:
        if (input == -1)
          return {-1};
        else if (input == '\n') {
          state = 2;
          return {input};
        } else {
          state = 1;
          return {input};
        }

      case 1:
        if (input == '\n') {
          state = 2;
          return {input};
        } else if (input == -1) {
          return {'\n', -1};
        } else
          return {input};

      case 2:
        if (input == '\n')
          return {input};
        else if (input == -1) {
          return {-1};
        } else {
          state = 1;
          return {input};
        }

      default:
        throw std::logic_error("invalid state");
    }
  }

 private:
  int state = 0;
};

struct PPTokenizer {
  IPPTokenStream& output;

  int header_name_state = 1;

  void emit_whitespace_sequence() { output.emit_whitespace_sequence(); }

  void emit_new_line() {
    header_name_state = 1;

    output.emit_new_line();
  }

  void emit_header_name(const string& data) {
    header_name_state = 0;

    output.emit_header_name(SourceInfo{data, filename, linenum});
  }

  void emit_identifier(const string& data) {
    if (header_name_state == 2 && data == "include")
      header_name_state = 3;
    else
      header_name_state = 0;

    output.emit_identifier(SourceInfo{data, filename, linenum});
  }

  void emit_pp_number(const string& data) {
    header_name_state = 0;

    output.emit_pp_number(SourceInfo{data, filename, linenum});
  }

  void emit_character_literal(const string& data) {
    header_name_state = 0;

    output.emit_character_literal(SourceInfo{data, filename, linenum});
  }

  void emit_user_defined_character_literal(const string& data) {
    header_name_state = 0;

    output.emit_user_defined_character_literal(
        SourceInfo{data, filename, linenum});
  }

  void emit_string_literal(const string& data) {
    header_name_state = 0;

    output.emit_string_literal(SourceInfo{data, filename, linenum});
  }

  void emit_user_defined_string_literal(const string& data) {
    header_name_state = 0;

    output.emit_user_defined_string_literal(
        SourceInfo{data, filename, linenum});
  }

  void emit_preprocessing_op_or_punc(const string& data) {
    if (header_name_state == 1 && (data == "#" || data == "%:"))
      header_name_state = 2;
    else
      header_name_state = 0;

    output.emit_preprocessing_op_or_punc(SourceInfo{data, filename, linenum});
  }

  void emit_non_whitespace_char(const string& data) {
    header_name_state = 0;

    output.emit_non_whitespace_char(SourceInfo{data, filename, linenum});
  }

  void emit_eof() {
    header_name_state = 0;

    output.emit_eof();
  }

  PPTokenizer(IPPTokenStream& output) : output(output) {}

  enum states {
    start,
    equals,
    colon,
    hash,
    langle,
    langle2,
    langle_colon,
    langle_colon2,
    rangle,
    rangle2,
    percent,
    percent_colon,
    percent_colon_percent,
    asterisk,
    plus,
    dash,
    dash_rangle,
    hat,
    ampersand,
    bar,
    exclamation,
    dot,
    dot2,
    pp_number,
    pp_number_e,
    identifier,
    whitespace,
    forward_slash,
    whitespace_forward_slash,
    inline_comment,
    inline_comment_ending,
    single_line_comment,
    character_literal,
    character_literal_backslash,
    character_literal_hex,
    character_literal_suffix,
    user_defined_character_literal,
    string_literal,
    string_literal_backslash,
    string_literal_hex,
    string_literal_suffix,
    user_defined_string_literal,
    raw_string_literal,
    raw_string_body,
    header_name_h,
    header_name_q,
    done
  };

  int state = start;

  bool raw_mode = false;
  Utf8Decoder utf8_decoder;
  UniversalCharacterNameDecoder ucn_decoder;
  TrigraphDecoder trigraph_decoder;
  LineSplicer line_splicer;
  NewlineEnder line_ender;

  string filename = "<stdin>";
  size_t linenum = 1;

  void process(int c0) {
    if (c0 == '\n') linenum++;

    if (!raw_mode) {
      for (int c1 : utf8_decoder.decode(c0))
        for (int c2 : trigraph_decoder.decode(c1))
          for (int c3 : ucn_decoder.decode(c2))
            for (int c4 : line_splicer.decode(c3))
              for (int c5 : line_ender.decode(c4)) {
                lookahead = c5;
                next_state();
              }
    } else {
      for (int c1 : utf8_decoder.decode(c0)) {
        lookahead = c1;
        next_state();
      }
    }
  }

  int lookahead;
  std::vector<int> accumulator;

  string accumulator_utf8() { return EncodeUtf8(accumulator); }

  struct action_t {};

  // add lookahead to accumulator, wait for next character
  action_t keep_wait(states s) {
    accumulator.push_back(lookahead);
    state = s;
    return action_t();
  }

  // discard accumulator and lookahead, wait for next character
  action_t clear_wait(states s) {
    accumulator.clear();
    state = s;
    return action_t();
  }

  // goto state s, do not touch accumulator or lookahead
  action_t keep_redirect(states s) {
    state = s;
    return next_state();
  }

  // discard accumulator, goto state s
  action_t clear_redirect(states s) {
    accumulator.clear();
    state = s;
    return next_state();
  }

  std::vector<int> raw_string_delim;
  size_t raw_string_match;

  action_t next_state() {
    switch (state) {
      case start:
        if (IsDigit(lookahead)) return keep_wait(pp_number);

        if (IsAllowedIdentifierFirstCharacter(lookahead))
          return keep_wait(identifier);

        switch (lookahead) {
          case ' ':
          case '\t':
          case '\v':
          case '\f':
            return keep_wait(whitespace);

          case '\n':
            emit_new_line();
            return clear_wait(start);

          case '"':
            if (header_name_state == 3)
              return keep_wait(header_name_q);
            else
              return keep_wait(string_literal);

          case '\'':
            return keep_wait(character_literal);

          case '/':
            return keep_wait(forward_slash);

          case '.':
            return keep_wait(dot);

          case '{':
          case '}':
          case '[':
          case ']':
          case '(':
          case ')':
          case ';':
          case '?':
          case ',':
          case '~':
            emit_preprocessing_op_or_punc(string(1, char(lookahead)));
            return clear_wait(start);

          case '=':
            return keep_wait(equals);

          case ':':
            return keep_wait(colon);

          case '#':
            return keep_wait(hash);

          case '<':
            if (header_name_state == 3)
              return keep_wait(header_name_h);
            else
              return keep_wait(langle);

          case '>':
            return keep_wait(rangle);

          case '%':
            return keep_wait(percent);

          case '*':
            return keep_wait(asterisk);

          case '+':
            return keep_wait(plus);

          case '-':
            return keep_wait(dash);

          case '^':
            return keep_wait(hat);

          case '&':
            return keep_wait(ampersand);

          case '|':
            return keep_wait(bar);

          case '!':
            return keep_wait(exclamation);

          case -1:
            emit_eof();
            return clear_wait(done);

          default:
            emit_non_whitespace_char(EncodeUtf8({lookahead}));
            return clear_wait(start);
        }

      case equals:
        switch (lookahead) {
          case '=':
            emit_preprocessing_op_or_punc("==");
            return clear_wait(start);

          default:
            emit_preprocessing_op_or_punc("=");
            return clear_redirect(start);
        }

      case colon:
        switch (lookahead) {
          case '>':
            emit_preprocessing_op_or_punc(":>");
            return clear_wait(start);

          case ':':
            emit_preprocessing_op_or_punc("::");
            return clear_wait(start);

          default:
            emit_preprocessing_op_or_punc(":");
            return clear_redirect(start);
        }

      case hash:
        switch (lookahead) {
          case '#':
            emit_preprocessing_op_or_punc("##");
            return clear_wait(start);

          default:
            emit_preprocessing_op_or_punc("#");
            return clear_redirect(start);
        }

      case langle:
        switch (lookahead) {
          case '<':
            return keep_wait(langle2);

          case ':':
            return keep_wait(langle_colon);

          case '%':
            emit_preprocessing_op_or_punc("<%");
            return clear_wait(start);

          case '=':
            emit_preprocessing_op_or_punc("<=");
            return clear_wait(start);

          default:
            emit_preprocessing_op_or_punc("<");
            return clear_redirect(start);
        }

      case langle2:
        switch (lookahead) {
          case '=':
            emit_preprocessing_op_or_punc("<<=");
            return clear_wait(start);

          default:
            emit_preprocessing_op_or_punc("<<");
            return clear_redirect(start);
        }

      case langle_colon:
        switch (lookahead) {
          case ':':
            return keep_wait(langle_colon2);

          default:
            emit_preprocessing_op_or_punc("<:");
            return clear_redirect(start);
        }

      case langle_colon2:
        switch (lookahead) {
          case ':':
            emit_preprocessing_op_or_punc("<:");
            emit_preprocessing_op_or_punc("::");
            return clear_wait(start);

          case '>':
            emit_preprocessing_op_or_punc("<:");
            emit_preprocessing_op_or_punc(":>");
            return clear_wait(start);

          default:
            emit_preprocessing_op_or_punc("<");
            emit_preprocessing_op_or_punc("::");
            return clear_redirect(start);
        }

      case rangle:
        switch (lookahead) {
          case '>':
            return keep_wait(rangle2);

          case '=':
            emit_preprocessing_op_or_punc(">=");
            return clear_wait(start);

          default:
            emit_preprocessing_op_or_punc(">");
            return clear_redirect(start);
        }

      case rangle2:
        switch (lookahead) {
          case '=':
            emit_preprocessing_op_or_punc(">>=");
            return clear_wait(start);

          default:
            emit_preprocessing_op_or_punc(">>");
            return clear_redirect(start);
        }

      case percent:
        switch (lookahead) {
          case '>':
            emit_preprocessing_op_or_punc("%>");
            return clear_wait(start);

          case ':':
            return keep_wait(percent_colon);

          case '=':
            emit_preprocessing_op_or_punc("%=");
            return clear_wait(start);

          default:
            emit_preprocessing_op_or_punc("%");
            return clear_redirect(start);
        }

      case percent_colon:
        switch (lookahead) {
          case '%':
            return keep_wait(percent_colon_percent);

          default:
            emit_preprocessing_op_or_punc("%:");
            return clear_redirect(start);
        }

      case percent_colon_percent:
        switch (lookahead) {
          case '>':
            emit_preprocessing_op_or_punc("%:");
            emit_preprocessing_op_or_punc("%>");
            return clear_wait(start);

          case ':':
            emit_preprocessing_op_or_punc("%:%:");
            return clear_wait(start);

          case '=':
            emit_preprocessing_op_or_punc("%:");
            emit_preprocessing_op_or_punc("%=");
            return clear_wait(start);

          default:
            emit_preprocessing_op_or_punc("%:");
            emit_preprocessing_op_or_punc("%");
            return clear_redirect(start);
        }

      case asterisk:
        switch (lookahead) {
          case '=':
            emit_preprocessing_op_or_punc("*=");
            return clear_wait(start);

          default:
            emit_preprocessing_op_or_punc("*");
            return clear_redirect(start);
        }

      case plus:
        switch (lookahead) {
          case '=':
            emit_preprocessing_op_or_punc("+=");
            return clear_wait(start);

          case '+':
            emit_preprocessing_op_or_punc("++");
            return clear_wait(start);

          default:
            emit_preprocessing_op_or_punc("+");
            return clear_redirect(start);
        }

      case dash:
        switch (lookahead) {
          case '=':
            emit_preprocessing_op_or_punc("-=");
            return clear_wait(start);

          case '-':
            emit_preprocessing_op_or_punc("--");
            return clear_wait(start);

          case '>':
            return keep_wait(dash_rangle);

          default:
            emit_preprocessing_op_or_punc("-");
            return clear_redirect(start);
        }

      case dash_rangle:
        switch (lookahead) {
          case '*':
            emit_preprocessing_op_or_punc("->*");
            return clear_wait(start);

          default:
            emit_preprocessing_op_or_punc("->");
            return clear_redirect(start);
        }

      case hat:
        switch (lookahead) {
          case '=':
            emit_preprocessing_op_or_punc("^=");
            return clear_wait(start);

          default:
            emit_preprocessing_op_or_punc("^");
            return clear_redirect(start);
        }

      case ampersand:
        switch (lookahead) {
          case '=':
            emit_preprocessing_op_or_punc("&=");
            return clear_wait(start);

          case '&':
            emit_preprocessing_op_or_punc("&&");
            return clear_wait(start);

          default:
            emit_preprocessing_op_or_punc("&");
            return clear_redirect(start);
        }

      case bar:
        switch (lookahead) {
          case '=':
            emit_preprocessing_op_or_punc("|=");
            return clear_wait(start);

          case '|':
            emit_preprocessing_op_or_punc("||");
            return clear_wait(start);

          default:
            emit_preprocessing_op_or_punc("|");
            return clear_redirect(start);
        }

      case exclamation:
        switch (lookahead) {
          case '=':
            emit_preprocessing_op_or_punc("!=");
            return clear_wait(start);

          default:
            emit_preprocessing_op_or_punc("!");
            return clear_redirect(start);
        }

      case dot:
        if (IsDigit(lookahead)) return keep_wait(pp_number);

        switch (lookahead) {
          case '.':
            return keep_wait(dot2);

          case '*':
            emit_preprocessing_op_or_punc(".*");
            return clear_wait(start);

          default:
            emit_preprocessing_op_or_punc(".");
            return clear_redirect(start);
        }

      case dot2:
        if (IsDigit(lookahead)) {
          emit_preprocessing_op_or_punc(".");
          accumulator.clear();
          accumulator.push_back('.');
          return keep_wait(pp_number);
        }

        switch (lookahead) {
          case '.':
            emit_preprocessing_op_or_punc("...");
            return clear_wait(start);

          case '*':
            emit_preprocessing_op_or_punc(".");
            emit_preprocessing_op_or_punc(".*");
            return clear_wait(start);

          default:
            emit_preprocessing_op_or_punc(".");
            emit_preprocessing_op_or_punc(".");
            return clear_redirect(start);
        }

      case pp_number:
        if (lookahead == 'E' || lookahead == 'e')
          return keep_wait(pp_number_e);
        else if (IsAllowedIdentifierBodyCharacter(lookahead) ||
                 lookahead == '.')
          return keep_wait(pp_number);
        else {
          emit_pp_number(accumulator_utf8());
          return clear_redirect(start);
        }

      case pp_number_e:
        if (lookahead == '+' || lookahead == '-')
          return keep_wait(pp_number);
        else
          return keep_redirect(pp_number);

      case identifier:
        if (IsAllowedIdentifierBodyCharacter(lookahead))
          return keep_wait(identifier);
        else if (lookahead == '\'' &&
                 IsCharacterLiteralEncodingPrefix(accumulator_utf8()))
          return keep_wait(character_literal);
        else if (lookahead == '"' &&
                 IsStringLiteralEncodingPrefix(accumulator_utf8()))
          return keep_wait(string_literal);
        else if (lookahead == '"' &&
                 IsRawStringLiteralPrefix(accumulator_utf8())) {
          raw_mode = true;
          raw_string_delim.clear();
          raw_string_delim.push_back(')');
          return keep_wait(raw_string_literal);
        } else if (IsDigraphKeyword(accumulator_utf8())) {
          emit_preprocessing_op_or_punc(accumulator_utf8());
          return clear_redirect(start);
        } else {
          emit_identifier(accumulator_utf8());
          return clear_redirect(start);
        }

      case whitespace:
        if (IsSpace(lookahead) && lookahead != '\n')
          return keep_wait(whitespace);
        else if (lookahead == '/')
          return keep_wait(whitespace_forward_slash);
        else {
          emit_whitespace_sequence();
          return clear_redirect(start);
        }

      case forward_slash:
        switch (lookahead) {
          case '=':
            emit_preprocessing_op_or_punc("/=");
            return clear_wait(start);

          case '*':
            return keep_wait(inline_comment);

          case '/':
            return keep_wait(single_line_comment);

          default:
            emit_preprocessing_op_or_punc("/");
            return clear_redirect(start);
        }

      case whitespace_forward_slash:
        switch (lookahead) {
          case '=':
            emit_whitespace_sequence();
            emit_preprocessing_op_or_punc("/=");
            return clear_wait(start);

          case '*':
            return keep_wait(inline_comment);

          case '/':
            return keep_wait(single_line_comment);

          default:
            emit_whitespace_sequence();
            emit_preprocessing_op_or_punc("/");
            return clear_redirect(start);
        }

      case inline_comment:
        if (lookahead == '*')
          return keep_wait(inline_comment_ending);
        else if (lookahead == -1)
          throw std::logic_error("partial comment");
        else
          return keep_wait(inline_comment);

      case inline_comment_ending:
        if (lookahead == '*')
          return keep_wait(inline_comment_ending);
        else if (lookahead == '/')
          return keep_wait(whitespace);
        else
          return keep_wait(inline_comment);

      case single_line_comment:
        if (lookahead == '\n') {
          emit_whitespace_sequence();
          return clear_redirect(start);
        } else
          return keep_wait(single_line_comment);

      case character_literal:
        if (lookahead == '\'')
          return keep_wait(character_literal_suffix);
        else if (lookahead == '\\')
          return keep_wait(character_literal_backslash);
        else if (lookahead == '\n' || lookahead == -1)
          throw std::logic_error("unterminated character literal");
        else
          return keep_wait(character_literal);

      case character_literal_backslash:
        if (IsSimpleEscapeChar(lookahead) ||
            (lookahead >= '0' && lookahead <= '7'))
          return keep_wait(character_literal);
        else if (lookahead == 'x')
          return keep_wait(character_literal_hex);
        else
          throw std::logic_error("invalid escape sequence");

      case character_literal_hex:
        if (IsHexDigit(lookahead))
          return keep_wait(character_literal);
        else
          throw std::logic_error("invalid hex escape sequence");

      case character_literal_suffix:
        if (IsAllowedIdentifierFirstCharacter(lookahead))
          return keep_wait(user_defined_character_literal);
        else {
          emit_character_literal(accumulator_utf8());
          return clear_redirect(start);
        }

      case user_defined_character_literal:
        if (IsAllowedIdentifierBodyCharacter(lookahead))
          return keep_wait(user_defined_character_literal);
        else {
          emit_user_defined_character_literal(accumulator_utf8());
          return clear_redirect(start);
        }

      case string_literal:
        if (lookahead == '"')
          return keep_wait(string_literal_suffix);
        else if (lookahead == '\\')
          return keep_wait(string_literal_backslash);
        else if (lookahead == '\n' || lookahead == -1)
          throw std::logic_error("unterminated string literal");
        else
          return keep_wait(string_literal);

      case string_literal_backslash:
        if (IsSimpleEscapeChar(lookahead) ||
            (lookahead >= '0' && lookahead <= '7'))
          return keep_wait(string_literal);
        else if (lookahead == 'x')
          return keep_wait(string_literal_hex);
        else
          throw std::logic_error("invalid escape sequence");

      case string_literal_hex:
        if (IsHexDigit(lookahead))
          return keep_wait(string_literal);
        else
          throw std::logic_error("invalid hex escape sequence");

      case string_literal_suffix:
        if (IsAllowedIdentifierFirstCharacter(lookahead))
          return keep_wait(user_defined_string_literal);
        else {
          emit_string_literal(accumulator_utf8());
          return clear_redirect(start);
        }

      case user_defined_string_literal:
        if (IsAllowedIdentifierBodyCharacter(lookahead))
          return keep_wait(user_defined_string_literal);
        else {
          emit_user_defined_string_literal(accumulator_utf8());
          return clear_redirect(start);
        }

      case raw_string_literal:
        if (lookahead == '(') {
          raw_string_delim.push_back('"');
          raw_string_match = 0;

          if (raw_string_delim.size() > 18)
            throw std::logic_error("raw string delimiter too long");

          return keep_wait(raw_string_body);
        } else if (lookahead == ')' || lookahead == '\\' || IsSpace(lookahead))
          throw std::logic_error("invalid characters in raw string delimiter");
        else if (lookahead == -1)
          throw std::logic_error("unterminated raw string literal");
        else {
          raw_string_delim.push_back(lookahead);
          return keep_wait(raw_string_literal);
        }

      case raw_string_body:
        if (lookahead == -1)
          throw std::logic_error("unterminated raw string literal");
        else if (lookahead == raw_string_delim[raw_string_match]) {
          raw_string_match++;

          if (raw_string_match == raw_string_delim.size()) {
            raw_mode = false;
            return keep_wait(string_literal_suffix);
          } else
            return keep_wait(raw_string_body);
        } else if (lookahead == raw_string_delim[0]) {
          raw_string_match = 1;
          return keep_wait(raw_string_body);
        } else {
          raw_string_match = 0;
          return keep_wait(raw_string_body);
        }

      case header_name_h:
        switch (lookahead) {
          case '>':
            accumulator.push_back('>');
            emit_header_name(accumulator_utf8());
            return clear_wait(start);

          case -1:
          case '\n':
            throw std::logic_error("unterminated header name");

          default:
            return keep_wait(header_name_h);
        }

      case header_name_q:
        switch (lookahead) {
          case '"':
            accumulator.push_back('"');
            emit_header_name(accumulator_utf8());
            return clear_wait(start);

          case -1:
          case '\n':
            throw std::logic_error("unterminated header name");

          default:
            return keep_wait(header_name_q);
        }

      case done:
        throw std::logic_error("already done");

      default:
        throw std::logic_error("unknown state");
    }
  }
};

void PPTokenize(string_view input, IPPTokenStream& output) {
  PPTokenizer tokenizer(output);

  for (char c : input) {
    unsigned char code_unit = c;

    tokenizer.process(code_unit);
  }

  tokenizer.process(-1);
}
