#pragma once

bool IsAlpha(int code_point);
bool IsDigit(int code_point);
bool IsOctDigit(int code_point);
bool IsHexDigit(int code_point);
bool IsSpace(int code_point);
bool IsValidCodePoint(int code_point);

int HexCharToValue(int c);
char ValueToHexChar(int c);
string HexDump(void* pdata, size_t nbytes);
inline string HexDump(const vector<byte>& data) {
  return HexDump((void*)data.data(), data.size());
}

string EncodeUtf8(const vector<int>& input);
vector<int> DecodeUtf8(const string& input);

template <typename T>
inline vector<byte> to_byte_vector(const T& t) {
  byte* begin = (byte*)&t;
  byte* end = begin + sizeof(T);

  return {begin, end};
}

struct Utf8Decoder {
  vector<int> decode(int input);

 private:
  int x = 0;
  int y = 0;
};

u16string EncodeUtf16(const vector<int>& input);
vector<int> DecodeUtf16(const u16string& input);

bool IsAllowedIdentifierFirstCharacter(int c);
bool IsAllowedIdentifierBodyCharacter(int c);
