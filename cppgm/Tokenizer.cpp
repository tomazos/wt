#include "std.pch"

#include "Tokenizer.h"

#include "Tokens.h"
#include "Text.h"

enum EIntegerSuffix
{
	INTS_NONE = 0,
	INTS_U = 1,
	INTS_L = 2,
	INTS_UL = 3,
	INTS_LL = 4,
	INTS_ULL = 5
};

EIntegerSuffix StringToIntegerSuffix(const string& integer_suffix)
{
	static unordered_map<string, EIntegerSuffix> m =
	{
		{"", INTS_NONE},
		{"u", INTS_U},
		{"U", INTS_U},
		{"l", INTS_L},
		{"L", INTS_L},
		{"ll", INTS_LL},
		{"LL", INTS_LL},
		{"ul", INTS_UL},
		{"uL", INTS_UL},
		{"ull", INTS_ULL},
		{"uLL", INTS_ULL},
		{"lu", INTS_UL},
		{"Lu", INTS_UL},
		{"llu", INTS_ULL},
		{"LLu", INTS_ULL},
		{"Ul", INTS_UL},
		{"UL", INTS_UL},
		{"Ull", INTS_ULL},
		{"ULL", INTS_ULL},
		{"lU", INTS_UL},
		{"LU", INTS_UL},
		{"llU", INTS_ULL},
		{"LLU", INTS_ULL}
	};

	auto it = m.find(integer_suffix);

	if (it == m.end())
		throw logic_error("unknown integer suffix: " + integer_suffix);

	return it->second;
}

template<class T>
PToken MakeLiteralToken(SourceInfo src, T t)
{
	return make_shared<LiteralToken>(src, FundamentalTypeOf<T>(), &t, sizeof(T));
}

PToken DecodeZeroInteger(SourceInfo src, const string& integer_suffix)
{
	switch (StringToIntegerSuffix(integer_suffix))
	{
	case INTS_NONE: return MakeLiteralToken<int>(src, 0);
	case INTS_U: return MakeLiteralToken<unsigned int>(src, 0);
	case INTS_L: return MakeLiteralToken<long int>(src, 0);
	case INTS_UL: return MakeLiteralToken<unsigned long int>(src, 0);
	case INTS_LL: return MakeLiteralToken<long long int>(src, 0);
	case INTS_ULL: return MakeLiteralToken<unsigned long long int>(src, 0);
	default: throw logic_error("DecodeZeroInteger: " + integer_suffix);
	}
}

PToken DecodeDecimalInteger(SourceInfo src, const string& s, const string& integer_suffix)
{
	static string smax_ull = "18446744073709551615";

	if (s[0] == '0')
		throw logic_error("DecodeDecimalInteger with leading zero: " + s);

	if (s.size() > smax_ull.size())
		throw logic_error("decimal integer literal out of range (#1): " + s);

	string padded = string(smax_ull.size() - s.size(), '0') + s;

	if (padded > smax_ull)
		throw logic_error("decimal integer literal out of range(#2): " + s);

	unsigned long long v = 0;

	for (char c : s)
	{
		v *= 10;
		v += c - '0';
	}

	constexpr unsigned long long max_i = INT_MAX;
	constexpr unsigned long long max_u = UINT_MAX;
	constexpr unsigned long long max_l = LONG_MAX;
	constexpr unsigned long long max_ul = ULONG_MAX;
	constexpr unsigned long long max_ll = LLONG_MAX;
	constexpr unsigned long long max_ull = ULLONG_MAX;

	switch (StringToIntegerSuffix(integer_suffix))
	{
	case INTS_NONE:
		if (v <= max_i)
			return MakeLiteralToken(src, (int)v);

	case INTS_L:
		if (v <= max_l)
			return MakeLiteralToken(src, (long)v);

	case INTS_LL:
		if (v <= max_ll)
			return MakeLiteralToken(src, (long long)v);

		throw logic_error("decimal integer literal out of range(#4): " + s);

	case INTS_U:
		if (v <= max_u)
			return MakeLiteralToken(src, (unsigned int)v);

	case INTS_UL:
		if (v <= max_ul)
			return MakeLiteralToken(src, (unsigned long)v);

	case INTS_ULL:
		if (v <= max_ull)
			return MakeLiteralToken(src, (unsigned long long)v);

		throw logic_error("decimal integer literal out of range(#5): " + s);

	default:
		throw logic_error("DecodeDecimalInteger: " + s);
	}
}

PToken DecodeOctalInteger(SourceInfo src, const string& s, const string& integer_suffix)
{
	static string smax_ull = "1777777777777777777777";

	if (s[0] == '0')
	{
		for (size_t i = 0; i < s.size(); i++)
			if (s[i] != '0')
				return DecodeOctalInteger(src, s.substr(i), integer_suffix);

		return DecodeZeroInteger(src, integer_suffix);
	}

	if (s.size() > smax_ull.size())
		throw logic_error("octal integer literal out of range (#1): " + s);

	string padded = string(smax_ull.size() - s.size(), '0') + s;

	if (padded > smax_ull)
		throw logic_error("octal integer literal out of range (#2): " + s);

	unsigned long long v = 0;

	for (char c : s)
	{
		v *= 8;
		v += c - '0';
	}

	constexpr unsigned long long max_i = INT_MAX;
	constexpr unsigned long long max_u = UINT_MAX;
	constexpr unsigned long long max_l = LONG_MAX;
	constexpr unsigned long long max_ul = ULONG_MAX;
	constexpr unsigned long long max_ll = LLONG_MAX;
	constexpr unsigned long long max_ull = ULLONG_MAX;

	switch (StringToIntegerSuffix(integer_suffix))
	{
	case INTS_NONE:
		if (v <= max_i)
			return MakeLiteralToken(src, (int)v);

		if (v <= max_u)
			return MakeLiteralToken(src, (unsigned int)v);

	case INTS_L:
		if (v <= max_l)
			return MakeLiteralToken(src, (long)v);

		if (v <= max_ul)
			return MakeLiteralToken(src, (unsigned long)v);

	case INTS_LL:
		if (v <= max_ll)
			return MakeLiteralToken(src, (long long)v);

		if (v <= max_ull)
			return MakeLiteralToken(src, (unsigned long long)v);

		throw logic_error("octal integer literal out of range(#4): " + s);

	case INTS_U:
		if (v <= max_u)
			return MakeLiteralToken(src, (unsigned int)v);

	case INTS_UL:
		if (v <= max_ul)
			return MakeLiteralToken(src, (unsigned long)v);

	case INTS_ULL:
		if (v <= max_ull)
			return MakeLiteralToken(src, (unsigned long long)v);

		throw logic_error("octal integer literal out of range(#5): " + s);

	default:
		throw logic_error("DecodeOctalInteger: " + s);
	}

}

PToken DecodeHexInteger(SourceInfo src, const string& smc, const string& integer_suffix)
{
	string s = string(smc.size(), '?');

	for (size_t i = 0; i < smc.size(); i++)
		s[i] = tolower(smc[i]);

	static string smax_ull = "ffffffffffffffff";

	if (s[0] == '0')
	{
		for (size_t i = 0; i < s.size(); i++)
			if (s[i] != '0')
				return DecodeHexInteger(src, s.substr(i), integer_suffix);

		return DecodeZeroInteger(src, integer_suffix);
	}

	if (s.size() > smax_ull.size())
		throw logic_error("hex integer literal out of range (#1): " + s);

	string padded = string(smax_ull.size() - s.size(), '0') + s;

	if (padded > smax_ull)
		throw logic_error("hex integer literal out of range (#2): " + s);

	unsigned long long v = 0;

	for (char c : s)
	{
		v *= 16;
		v += HexCharToValue(c);
	}

	constexpr unsigned long long max_i = INT_MAX;
	constexpr unsigned long long max_u = UINT_MAX;
	constexpr unsigned long long max_l = LONG_MAX;
	constexpr unsigned long long max_ul = ULONG_MAX;
	constexpr unsigned long long max_ll = LLONG_MAX;
	constexpr unsigned long long max_ull = ULLONG_MAX;

	switch (StringToIntegerSuffix(integer_suffix))
	{
	case INTS_NONE:
		if (v <= max_i)
			return MakeLiteralToken(src, (int)v);

		if (v <= max_u)
			return MakeLiteralToken(src, (unsigned int)v);

	case INTS_L:
		if (v <= max_l)
			return MakeLiteralToken(src, (long)v);

		if (v <= max_ul)
			return MakeLiteralToken(src, (unsigned long)v);

	case INTS_LL:
		if (v <= max_ll)
			return MakeLiteralToken(src, (long long)v);

		if (v <= max_ull)
			return MakeLiteralToken(src, (unsigned long long)v);

		throw logic_error("hex integer literal out of range(#4): " + s);

	case INTS_U:
		if (v <= max_u)
			return MakeLiteralToken(src, (unsigned int)v);

	case INTS_UL:
		if (v <= max_ul)
			return MakeLiteralToken(src, (unsigned long)v);

	case INTS_ULL:
		if (v <= max_ull)
			return MakeLiteralToken(src, (unsigned long long)v);

		throw logic_error("hex integer literal out of range(#5): " + s);

	default:
		throw logic_error("DecodeHexInteger: " + s);
	}
}

void CheckFloating(const string& s)
{
	size_t pos = 0;

	while (isdigit(s[pos]))
		pos++;

	bool front = (pos > 0);

	if (s[pos] == '.')
	{
		pos++;

		if (!front && !isdigit(s[pos]))
			throw logic_error("invalid floating literal (#3): " + s);

		while (isdigit(s[pos]))
			pos++;

		if (s[pos] == 0)
			return;
	}
	else if (!front && !isdigit(s[pos]))
		throw logic_error("invalid floating literal (#4): " + s);

	if (tolower(s[pos]) != 'e')
		throw logic_error("invalid floating literal (#1): " + s);

	pos++;

	if (s[pos] == '+' || s[pos] == '-')
		pos++;

	if (!isdigit(s[pos]))
		throw logic_error("invalid floating literal (#2): " + s);

	while (isdigit(s[pos]))
		pos++;

	if (s[pos] != 0)
		throw logic_error("invalid floating literal (#5): " + s);
}

PToken DecodeFloating(SourceInfo src, const string& s, const string& floating_suffix)
{
	istringstream iss(s);

	if (floating_suffix.empty())
	{
		double x;
		iss >> x;
		return MakeLiteralToken(src, x);
	}
	else if (floating_suffix == "f" || floating_suffix == "F")
	{
		float x;
		iss >> x;
		return MakeLiteralToken(src, x);
	}
	else if (floating_suffix == "l" || floating_suffix == "L")
	{
		long double x;
		iss >> x;
		return MakeLiteralToken(src, x);
	}

	throw logic_error("invalid floating suffix (#1): " + floating_suffix);
}

PToken TokenizePPNumber(SourceInfo src)
{
	string s_full = src.spelling;

	auto us_pos = s_full.find('_');

	string s, ud_suffix, number_suffix;

	if (us_pos == string::npos)
	{
		s = s_full;
	}
	else
	{
		s = s_full.substr(0, us_pos);
		ud_suffix = s_full.substr(us_pos);

		auto code_points = DecodeUtf8(ud_suffix);

		for (auto code_point : code_points)
			if (!IsAllowedIdentifierBodyCharacter(code_point))
				throw logic_error("invalid character in ud-suffix: " + s_full);
	}

	if (s.empty())
		throw logic_error("TokenizePPNumber tokenization failed on " + s_full);

	bool hexlit = ((s.find('x') != string::npos) || (s.find('X') != string::npos));

	bool floating =
	(
		!hexlit &&
		(s.find('.') != string::npos ||
			s.find('e') != string::npos ||
			s.find('E') != string::npos)
	);

	static unordered_set<string> floating_suffixes =
	{
		"", "f", "F", "l", "L"
	};

	static unordered_set<string> integer_suffixes =
	{
		"", "u", "U", "l", "L", "ll", "LL",
		"ul", "uL", "ull", "uLL",
		"lu", "Lu", "llu", "LLu",
		"Ul", "UL", "Ull", "ULL",
		"lU", "LU", "llU", "LLU",
	};

	auto it = s.end() - 1;

	while (true)
	{
		if (it == s.begin())
			break;

		char c = tolower(*it);

		if (floating)
		{
			if (c != 'f' && c != 'l')
				break;
		}
		else
		{
			if (c != 'u' && c != 'l')
				break;
		}

		it--;
	}

	it++;

	number_suffix = string(it, s.end());
	s = string(s.begin(), it);

	if (floating && floating_suffixes.count(number_suffix) == 0)
		throw logic_error("invalid floating suffix (#2): " + number_suffix);

	if (!floating && integer_suffixes.count(number_suffix) == 0)
		throw logic_error("invalid integer suffix " + number_suffix);

	if (s.empty())
		throw logic_error("malformed number (#1): " + s_full);

	if (!number_suffix.empty() && !ud_suffix.empty())
		throw logic_error("malformed number (#2): " + s_full);

	if (!floating)
	{
		if (s[0] == '0' && s.size() >= 3 && tolower(s[1]) == 'x')
		{
			for (size_t i = 2; i < s.size(); i++)
				if (!isxdigit(s[i]))
					throw logic_error("malformed number (#3): " + s_full);

			if (!ud_suffix.empty())
			{
				return make_shared<UserDefinedIntegerLiteralToken>(src, ud_suffix, s);
			}
			else
			{
				return DecodeHexInteger(src, s.substr(2), number_suffix);
			}
		}
		else if (s[0] == '0')
		{
			if (s.size() == 1)
			{
				if (!ud_suffix.empty())
				{
					return make_shared<UserDefinedIntegerLiteralToken>(src, ud_suffix, "0");
				}
				else
				{
					return DecodeZeroInteger(src, number_suffix);
				}
			}

			for (size_t i = 1; i < s.size(); i++)
				if (!IsOctDigit(s[i]))
					throw logic_error("malformed number (#4): " + s_full);

			if (!ud_suffix.empty())
			{
				return make_shared<UserDefinedIntegerLiteralToken>(src, ud_suffix, s);
			}
			else
			{
				return DecodeOctalInteger(src, s.substr(1), number_suffix);
			}
		}
		else
		{
			for (size_t i = 0; i < s.size(); i++)
				if (!isdigit(s[i]))
					throw logic_error("malformed number (#5): " + s_full);

			if (!ud_suffix.empty())
			{
				return make_shared<UserDefinedIntegerLiteralToken>(src, ud_suffix, s);
			}
			else
			{
				return DecodeDecimalInteger(src, s, number_suffix);
			}
		}
	}
	else
	{
		CheckFloating(s);

		if (!ud_suffix.empty())
			return make_shared<UserDefinedFloatingLiteralToken>(src, ud_suffix, s);
		else
			return DecodeFloating(src, s, number_suffix);
	}
}

struct CharacterLiteralInfo
{
	int code_point;
	char encoding_prefix;
};

vector<int> DescapeStringLiteral(const vector<int>& input)
{
	vector<int> neg1_input = input;
	neg1_input.push_back(-1);

	vector<int> output;

	int state = 0;

	static unordered_map<int, int> simple_escapes =
	{
		{'n', '\n'},
		{'t', '\t'},
		{'v', '\v'},
		{'b', '\b'},
		{'r', '\r'},
		{'f', '\f'},
		{'a', '\a'},
		{'\\', '\\'},
		{'?', '\?'},
		{'\'', '\''},
		{'"', '"'}
	};

	int y = 0;
	int z = 0;

	for (auto it = neg1_input.begin(); it != neg1_input.end(); ++it)
	{
		int c = *it;

		switch (state)
		{
		case 0:
			switch (c)
			{
			case '\\':
				state = 1;
				break;

			case -1:
				break;

			default:
				output.push_back(c);
			}
			break;

		case 1:
			{
				auto jt = simple_escapes.find(c);
				if (jt != simple_escapes.end())
				{
					output.push_back(jt->second);
					state = 0;
				}
				else if (c == 'x')
				{
					y = 0;
					z = 1;
					state = 2;
				}
				else if (IsOctDigit(c))
				{
					y = HexCharToValue(c);
					z = 1;
					state = 3;
				}
				else
				{
					throw logic_error("invalid escape: " + EncodeUtf8(input));
				}
			}
			break;

		case 2:
			if (IsHexDigit(c))
			{
				z++;
				y = y*16 + HexCharToValue(c);
				if (y >= 0x110000)
					throw logic_error("hex escape out of range: " + to_string(y) + " " + EncodeUtf8(input));
			}
			else
			{
				if (z == 0)
					throw logic_error("invalid hex escape: " + EncodeUtf8(input));
				output.push_back(y);
				state = 0;
				it--;
			}
			break;

		case 3:
			if (IsOctDigit(c) && z < 3)
			{
				z++;
				y = y*8 + HexCharToValue(c);
			}
			else
			{
				output.push_back(y);
				state = 0;
				it--;
			}
			break;

		default:
			throw logic_error("DescapeStringLiteral: unknown state " + to_string(state));
		}
	}

	return output;
}

void CharacterLiteralToCodePoint(const string& s, CharacterLiteralInfo& info)
{
	if (s.size() < 3)
		throw logic_error("malformed character literal (#1): " + s);

	string inner;

	if (s[0] == '\'')
	{
		inner = s.substr(1, s.size()-2);
		info.encoding_prefix = 0;
	}
	else
	{
		inner = s.substr(2, s.size()-3);
		info.encoding_prefix = s[0];
	}

	if (inner.empty())
		throw logic_error("malformed character literal (#2): " + s);

	vector<int> escaped_code_points = DecodeUtf8(inner);

	vector<int> code_points = DescapeStringLiteral(escaped_code_points);

	if (code_points.size() > 1)
		throw logic_error("multi code point character literals not supported: " + s);

	info.code_point = code_points[0];
}

CharacterLiteralInfo CharacterLiteralToCodePoint(const string& s)
{
	CharacterLiteralInfo info;
	CharacterLiteralToCodePoint(s, info);
	return info;
}

struct UserDefinedCharaterLiteralInfo : CharacterLiteralInfo
{
	string ud_suffix;
};

size_t FindStartOfUdSuffix_CharAndStr(const string& s)
{
	for (size_t i = s.size(); i > 0; i--)
		if (s[i-1] == '\'' || s[i-1] == '"')
		{
			if (s[i] != '_')
				throw logic_error("ud_suffix does not start with _: " + s);

			return i;
		}

	throw logic_error("malformed user-defined-literal (ud_suffix): " + s);
}

UserDefinedCharaterLiteralInfo UserDefinedCharacterLiteralToCodePoint(const string& s)
{
	auto ud_suffix_start = FindStartOfUdSuffix_CharAndStr(s);

	string character_literal = s.substr(0, ud_suffix_start);

	UserDefinedCharaterLiteralInfo info;
	CharacterLiteralToCodePoint(character_literal, info);

	info.ud_suffix = s.substr(ud_suffix_start);

	return info;
}

struct StringLiteralInfo
{
	vector<int> code_points;
	string encoding_prefix;
};

void StringLiteralToCodePoints(const string& s, StringLiteralInfo& info)
{
	if (s.size() < 2)
		throw logic_error("malformed string literal (#1): " + s);

	switch (s[0])
	{
	case '"':
	case 'R':
		info.encoding_prefix = "";
		break;

	case 'u':
		if (s[1] == '8')
			info.encoding_prefix = "u8";
		else
			info.encoding_prefix = "u";
		break;

	case 'U':
		info.encoding_prefix = "U";
		break;

	case 'L':
		info.encoding_prefix = "L";
		break;

	default:
		throw logic_error("malformed string-literal (#2): " + s);
	}

	size_t prefix_len = info.encoding_prefix.size();

	if (s.size() + prefix_len < 2)
		throw logic_error("malformed string literal (#3): " + s);

	if (s[prefix_len] == 'R')
	{
		size_t open_bracket = s.find('(');

		if (open_bracket == string::npos)
			throw logic_error("malformed raw string literal (#1): " + s);

		size_t dchar_len = open_bracket - prefix_len; // includes "...(

		size_t start_pos = open_bracket+1;

		if (start_pos + dchar_len > s.size())
			throw logic_error("malformed raw string literal (#2): " + s);

		size_t end_pos = s.size() - dchar_len;

		if (s.substr(prefix_len+2, dchar_len-2) != s.substr(end_pos+1, dchar_len-2))
			throw logic_error("malformed raw string literal (#3): " + s);

		if (s[prefix_len+1] != '"' || s[open_bracket] != '(' || s[end_pos] != ')' || s[s.size()-1] != '"')
			throw logic_error("malformed raw string literal (#4): " + s);

		string inner = s.substr(start_pos, end_pos - start_pos);
		info.code_points = DecodeUtf8(inner);
	}
	else
	{
		if (s[prefix_len] != '"' || s[s.size()-1] != '"')
			throw logic_error("malformed string literal (#4): " + s);

		string inner = s.substr(prefix_len+1, s.size()-prefix_len-2);

		vector<int> escaped_code_points = DecodeUtf8(inner);
		info.code_points = DescapeStringLiteral(escaped_code_points);
	}
}

StringLiteralInfo StringLiteralToCodePoints(const string& s)
{
	StringLiteralInfo info;
	StringLiteralToCodePoints(s, info);
	return info;
}

struct UserDefinedStringLiteralInfo : StringLiteralInfo
{
	string ud_suffix;
};

UserDefinedStringLiteralInfo UserDefinedStringLiteralToCodePoints(const string& s)
{
	auto ud_suffix_start = FindStartOfUdSuffix_CharAndStr(s);

	string string_literal = s.substr(0, ud_suffix_start);

	UserDefinedStringLiteralInfo info;
	StringLiteralToCodePoints(string_literal, info);

	info.ud_suffix = s.substr(ud_suffix_start);

	return info;
}

PToken MakeCharacterLiteralToken(SourceInfo src, const CharacterLiteralInfo& info)
{
	int c = info.code_point;

	switch (info.encoding_prefix)
	{
	case 0:
		if (c >= 0 && c < 128)
			return MakeLiteralToken(src, (char) c);
		else if (IsValidCodePoint(c))
			return MakeLiteralToken(src, (int) c);
		else
			throw logic_error("char literal code point out of range " + src.spelling + " " + to_string(c));

	case 'u':
		if (IsValidCodePoint(c) && c < 0x10000)
			return MakeLiteralToken(src, (char16_t) c);
		else
			throw logic_error("UTF-16 char literal out of range: " + src.spelling + " " + to_string(c));

	case 'U':
		if (IsValidCodePoint(c))
			return MakeLiteralToken(src, (char32_t) c);
		else
			throw logic_error("UTF-32 char literal out of range: " + src.spelling + " " + to_string(c));

	case 'L':
		if (IsValidCodePoint(c))
			return MakeLiteralToken(src, (wchar_t) c);
		else
			throw logic_error("wchar_t char literal out of range: " + src.spelling + " " + to_string(c));

	default:
		throw logic_error("character literal encoding prefix invalid: " + src.spelling);
	}

}

PToken MakeStringLiteralToken(SourceInfo src, const StringLiteralInfo& info)
{
	if (info.encoding_prefix == "" || info.encoding_prefix == "u8")
	{
		string s = EncodeUtf8(info.code_points);

		return make_shared<LiteralToken>(src, s.size()+1, FT_CHAR, s.data(), s.size() + 1);
	}
	else if (info.encoding_prefix == "u")
	{
		u16string s = EncodeUtf16(info.code_points);

		return make_shared<LiteralToken>(src, s.size()+1, FT_CHAR16_T, s.data(), s.size() * 2 + 2);
	}
	else if (info.encoding_prefix == "U")
	{
		u32string s;

		for (size_t i = 0; i < info.code_points.size(); i++)
		{
			int code_point = info.code_points[i];

			if (!IsValidCodePoint(code_point))
				throw logic_error("MakeStringLiteralToken: invalid code points: " + src.spelling);

			s += (char32_t) code_point;
		}
		return make_shared<LiteralToken>(src, s.size()+1, FT_CHAR32_T, s.data(), s.size() * 4 + 4);
	}
	else if (info.encoding_prefix == "L")
	{
		wstring s;

		for (size_t i = 0; i < info.code_points.size(); i++)
		{
			int code_point = info.code_points[i];

			if (!IsValidCodePoint(code_point))
				throw logic_error("MakeStringLiteralToken: invalid code points: " + src.spelling);

			s += (wchar_t) code_point;
		}
		return make_shared<LiteralToken>(src, s.size()+1, FT_WCHAR_T, s.data(), s.size() * 4 + 4);
	}
	else
		throw logic_error("invalid encoding prefix: " + src.spelling);
}

PToken TokenizeCharacterLiteral(SourceInfo src)
{
	CharacterLiteralInfo info = CharacterLiteralToCodePoint(src.spelling);

	return MakeCharacterLiteralToken(src, info);
}

PToken TokenizeUserDefinedCharacterLiteral(SourceInfo src)
{
	auto info = UserDefinedCharacterLiteralToCodePoint(src.spelling);
	PToken character_literal = MakeCharacterLiteralToken(src, info);
	LiteralToken* literal_token = dynamic_cast<LiteralToken*>(character_literal.get());
	assert(literal_token);
	assert(!literal_token->is_array);

	return make_shared<UserDefinedCharacterLiteralToken>(src, info.ud_suffix, literal_token->fundamental_type, literal_token->data.data(), literal_token->data.size());
}

PToken TokenizeStringLiteral(SourceInfo src)
{
	StringLiteralInfo info = StringLiteralToCodePoints(src.spelling);

	return MakeStringLiteralToken(src, info);
}

PToken MakeUserDefinedStringLiteralToken(SourceInfo src, const UserDefinedStringLiteralInfo& info)
{
	PToken string_literal = MakeStringLiteralToken(src, info);
	LiteralToken* literal_token = dynamic_cast<LiteralToken*>(string_literal.get());
	assert(literal_token);
	assert(literal_token->is_array);
	return make_shared<UserDefinedStringLiteralToken>(src, info.ud_suffix, literal_token->num_elements, literal_token->fundamental_type, literal_token->data.data(), literal_token->data.size());
}

PToken TokenizeUserDefinedStringLiteral(SourceInfo src)
{
	auto info = UserDefinedStringLiteralToCodePoints(src.spelling);
	return MakeUserDefinedStringLiteralToken(src, info);
}

PToken TokenizeStringLiteralSequence(SourceInfo src, const vector<PPStringLiteral>& literals)
{
	UserDefinedStringLiteralInfo head;

	for (const PPStringLiteral& literal : literals)
	{
		UserDefinedStringLiteralInfo tail;

		if (literal.is_ud)
			tail = UserDefinedStringLiteralToCodePoints(literal.src.spelling);
		else
			StringLiteralToCodePoints(literal.src.spelling, tail);

		if (!tail.encoding_prefix.empty())
		{
			if (head.encoding_prefix.empty())
				head.encoding_prefix = tail.encoding_prefix;
			else if (head.encoding_prefix != tail.encoding_prefix)
				throw logic_error("mismatched encoding prefix in string literal sequence");
		}

		if (!tail.ud_suffix.empty())
		{
			if (head.ud_suffix.empty())
				head.ud_suffix = tail.ud_suffix;
			else if (head.ud_suffix != tail.ud_suffix)
				throw logic_error("mismatched ud_suffix in string literal sequence");
		}

		head.code_points.insert(head.code_points.end(), tail.code_points.begin(), tail.code_points.end());
	}

	if (head.ud_suffix.empty())
		return MakeStringLiteralToken(src, head);
	else
		return MakeUserDefinedStringLiteralToken(src, head);
}
