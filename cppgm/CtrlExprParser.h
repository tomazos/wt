#pragma once

#include "Tokens.h"
#include "PreprocessingToken.h"

vector<PToken> CtrlExpr_ConvertPreprocessingTokens(
    const vector<PreprocessingToken>& input);

struct CtrlExprVal {
  bool is_error;
  bool is_signed;

  union {
    intmax_t s;
    uintmax_t u;
  };
  //
  //	CtrlExprVal error() const
  //	{
  //		CtrlExprVal e(intmax_t(0));
  //		e.is_signed = is_signed;
  //		e.is_error = true;
  //		return e;
  //	}

  bool eval_bool() const {
    if (is_error) throw logic_error("erroneous value evaluated eval_bool");
    if (is_signed) {
      if (s)
        return true;
      else
        return false;
    } else {
      if (u)
        return true;
      else
        return false;
    }
  }

  bool is_ok_shift() const {
    if (is_error) throw logic_error("erroneous value evaluated is_ok_shift");
    if (is_signed)
      return (s >= 0 && s < 64);
    else
      return (u < 64);
  }

  //	explicit CtrlExprVal(bool b)
  //		: is_signed(true)
  //		, s(b ? 1 : 0)
  //	{
  //
  //	}
  //
  CtrlExprVal(intmax_t x, bool is_error)
      : is_error(is_error), is_signed(true), s(x) {}

  CtrlExprVal(uintmax_t x, bool is_error)
      : is_error(is_error), is_signed(false), u(x) {}

  void set_signed(intmax_t x) {
    is_signed = true;
    s = x;
  }

  void set_unsigned(uintmax_t x) {
    is_signed = false;
    u = x;
  }

  CtrlExprVal(const LiteralToken& token) : is_error(false) {
    if (token.token_type != TT_LITERAL)
      throw logic_error("user defined literal in controlling expression");

    if (token.is_array) throw logic_error("array in controlling expression");

    switch (token.fundamental_type) {
      case FT_SIGNED_CHAR:
        set_signed(token.get<signed char>());
        return;
      case FT_SHORT_INT:
        set_signed(token.get<short int>());
        return;
      case FT_INT:
        set_signed(token.get<int>());
        return;
      case FT_LONG_INT:
        set_signed(token.get<long int>());
        return;
      case FT_LONG_LONG_INT:
        set_signed(token.get<long long int>());
        return;
      case FT_UNSIGNED_CHAR:
        set_unsigned(token.get<unsigned char>());
        return;
      case FT_UNSIGNED_SHORT_INT:
        set_unsigned(token.get<unsigned short int>());
        return;
      case FT_UNSIGNED_INT:
        set_unsigned(token.get<unsigned int>());
        return;
      case FT_UNSIGNED_LONG_INT:
        set_unsigned(token.get<unsigned long int>());
        return;
      case FT_UNSIGNED_LONG_LONG_INT:
        set_unsigned(token.get<unsigned long long int>());
        return;

      case FT_WCHAR_T:
        set_signed(token.get<wchar_t>());
        return;
      case FT_CHAR:
        set_signed(token.get<char>());
        return;
      case FT_CHAR16_T:
        set_unsigned(token.get<char16_t>());
        return;
      case FT_CHAR32_T:
        set_unsigned(token.get<char32_t>());
        return;

      case FT_BOOL:
      case FT_VOID:
      case FT_NULLPTR_T:
        throw logic_error(
            "internal error: unexpected literal type passed as LiteralToken");

      case FT_FLOAT:
      case FT_DOUBLE:
      case FT_LONG_DOUBLE:
        throw logic_error("floating literal in controlling expression");

      default:
        throw logic_error("unknown fundamental type (#1)");
    }
  }

  CtrlExprVal operator+() const {
    return is_signed ? CtrlExprVal(+s, is_error) : CtrlExprVal(+u, is_error);
  }
  CtrlExprVal operator-() const {
    return is_signed ? CtrlExprVal(-s, is_error) : CtrlExprVal(-u, is_error);
  }
  CtrlExprVal operator!() const {
    return is_signed ? CtrlExprVal(intmax_t(!s), is_error)
                     : CtrlExprVal(intmax_t(!u), is_error);
  }
  CtrlExprVal operator~() const {
    return is_signed ? CtrlExprVal(~s, is_error) : CtrlExprVal(~u, is_error);
  }

#define DefineCtrlExprBinOp(op)                                            \
  CtrlExprVal operator op(CtrlExprVal x) const {                           \
    bool e = is_error || x.is_error;                                       \
    if (is_signed && x.is_signed) return CtrlExprVal(s op x.s, e);         \
    if (is_signed && !x.is_signed) return CtrlExprVal(s op x.u, e);        \
    if (!is_signed && x.is_signed) return CtrlExprVal(u op x.s, e);        \
    /* if (!is_signed && !x.is_signed) */ return CtrlExprVal(u op x.u, e); \
  }

#define DefineCtrlExprBinOpL(op)                                              \
  CtrlExprVal operator op(CtrlExprVal x) const {                              \
    bool e = is_error || x.is_error;                                          \
    if (is_signed && x.is_signed) return CtrlExprVal(intmax_t(s op x.s), e);  \
    if (is_signed && !x.is_signed) return CtrlExprVal(intmax_t(s op x.u), e); \
    if (!is_signed && x.is_signed) return CtrlExprVal(intmax_t(u op x.s), e); \
    /* if (!is_signed && !x.is_signed) */ return CtrlExprVal(                 \
        intmax_t(u op x.u), e);                                               \
  }

  DefineCtrlExprBinOp(*);
  DefineCtrlExprBinOp(/ );
  DefineCtrlExprBinOp(% );

  DefineCtrlExprBinOp(+);
  DefineCtrlExprBinOp(-);

  DefineCtrlExprBinOp(<< );
  DefineCtrlExprBinOp(>> );

  DefineCtrlExprBinOpL(< );
  DefineCtrlExprBinOpL(> );
  DefineCtrlExprBinOpL(<= );
  DefineCtrlExprBinOpL(>= );
  DefineCtrlExprBinOpL(== );
  DefineCtrlExprBinOpL(!= );

  DefineCtrlExprBinOp(&);
  DefineCtrlExprBinOp (^);
  DefineCtrlExprBinOp(| );

  CtrlExprVal operator&&(CtrlExprVal x) const {
    CtrlExprVal e(intmax_t(false), true);

    if (is_error) return e;

    if (!eval_bool()) return CtrlExprVal(intmax_t(false), false);

    if (x.is_error) return e;

    if (x.eval_bool())
      return CtrlExprVal(intmax_t(true), false);
    else
      return CtrlExprVal(intmax_t(false), false);
  }

  CtrlExprVal operator||(CtrlExprVal x) const {
    CtrlExprVal e(intmax_t(false), true);

    if (is_error) return e;

    if (eval_bool()) return CtrlExprVal(intmax_t(true), false);

    if (x.is_error) return e;

    if (x.eval_bool())
      return CtrlExprVal(intmax_t(true), false);
    else
      return CtrlExprVal(intmax_t(false), false);
  }

  void dump() {
    if (is_error) {
      cerr << "ERROR: evaluation error in controlling expression" << endl;
      cout << "error" << endl;
    } else if (is_signed)
      cout << s << endl;
    else
      cout << u << "u" << endl;
  }
};

struct CtrlExprParser {
  const vector<PToken> tokens;
  vector<PToken>::const_iterator cur;
  function<bool(const string&)> is_defined_f;

  CtrlExprParser(const vector<PToken>& tokens_in,
                 function<bool(const string&)> is_defined_f_in)
      : tokens(tokens_in), cur(tokens.begin()), is_defined_f(is_defined_f_in) {}

  CtrlExprVal parse() {
    CtrlExprVal result = parse_conditional_expression();
    if (cur != tokens.end())
      throw logic_error("left over tokens at end of controlling expression");
    return result;
  }

  PToken peek_lookahead() {
    static PToken invalid_token =
        make_shared<Token>(SourceInfo{"", "", 0}, TT_INVALID);

    if (cur == tokens.end())
      return invalid_token;
    else
      return *cur;
  }

  PToken pop_lookahead() {
    static PToken invalid_token =
        make_shared<Token>(SourceInfo{"", "", 0}, TT_INVALID);

    if (cur == tokens.end())
      return invalid_token;
    else
      return *cur++;
  }

  CtrlExprVal parse_conditional_expression() {
    auto condition = parse_logical_or_expression();

    if (peek_lookahead()->token_type == OP_QMARK) {
      pop_lookahead();

      auto on_true = parse_conditional_expression();

      if (pop_lookahead()->token_type != OP_COLON)
        throw logic_error("expected colon");

      auto on_false = parse_conditional_expression();

      if (!on_true.is_signed) on_false.is_signed = false;

      if (!on_false.is_signed) on_true.is_signed = false;

      if (condition.is_error)
        return condition;
      else if (condition.eval_bool())
        return on_true;
      else
        return on_false;
    } else
      return condition;
  }

  CtrlExprVal parse_logical_or_expression() {
    auto val = parse_logical_and_expression();

    while (peek_lookahead()->token_type == OP_LOR) {
      pop_lookahead();

      auto next = parse_logical_and_expression();

      val = (val || next);
    }

    return val;
  }

  CtrlExprVal parse_logical_and_expression() {
    auto val = parse_inclusive_or_expression();

    while (peek_lookahead()->token_type == OP_LAND) {
      pop_lookahead();

      auto next = parse_inclusive_or_expression();

      val = (val && next);
    }

    return val;
  }

  CtrlExprVal parse_inclusive_or_expression() {
    auto val = parse_exclusive_or_expression();

    while (peek_lookahead()->token_type == OP_BOR) {
      pop_lookahead();

      auto next = parse_exclusive_or_expression();

      val = (val | next);
    }

    return val;
  }

  CtrlExprVal parse_exclusive_or_expression() {
    auto val = parse_and_expression();

    while (peek_lookahead()->token_type == OP_XOR) {
      pop_lookahead();

      auto next = parse_and_expression();

      val = (val ^ next);
    }

    return val;
  }

  CtrlExprVal parse_and_expression() {
    auto val = parse_equality_expression();

    while (peek_lookahead()->token_type == OP_AMP) {
      pop_lookahead();

      auto next = parse_equality_expression();

      val = (val & next);
    }

    return val;
  }

  CtrlExprVal parse_equality_expression() {
    auto val = parse_relational_expression();

    while (peek_lookahead()->token_type == OP_EQ ||
           peek_lookahead()->token_type == OP_NE) {
      auto op = pop_lookahead();

      auto next = parse_relational_expression();

      if (op->token_type == OP_EQ)
        val = (val == next);
      else if (op->token_type == OP_NE)
        val = (val != next);
      else
        throw logic_error("internal error");
    }

    return val;
  }

  CtrlExprVal parse_relational_expression() {
    auto val = parse_shift_expression();

    while (peek_lookahead()->token_type == OP_LT ||
           peek_lookahead()->token_type == OP_GT ||
           peek_lookahead()->token_type == OP_LE ||
           peek_lookahead()->token_type == OP_GE) {
      auto op = pop_lookahead();

      auto next = parse_shift_expression();

      switch (op->token_type) {
        case OP_LT:
          val = (val < next);
          break;
        case OP_GT:
          val = (val > next);
          break;
        case OP_LE:
          val = (val <= next);
          break;
        case OP_GE:
          val = (val >= next);
          break;
        default:
          throw logic_error("internal error");
      }
    }

    return val;
  }

  CtrlExprVal parse_shift_expression() {
    auto val = parse_additive_expression();

    while (peek_lookahead()->token_type == OP_LSHIFT ||
           peek_lookahead()->token_type == OP_RSHIFT) {
      auto op = pop_lookahead();

      auto next = parse_additive_expression();

      if (next.is_error || !next.is_ok_shift()) {
        next.is_error = true;
        next.u = 0;
      }

      if (op->token_type == OP_LSHIFT)
        val = (val << next);
      else if (op->token_type == OP_RSHIFT)
        val = (val >> next);
      else
        throw logic_error("internal error");
    }

    return val;
  }

  CtrlExprVal parse_additive_expression() {
    auto val = parse_multiplicative_expression();

    while (peek_lookahead()->token_type == OP_PLUS ||
           peek_lookahead()->token_type == OP_MINUS) {
      auto op = pop_lookahead();

      auto next = parse_multiplicative_expression();

      if (op->token_type == OP_PLUS)
        val = (val + next);
      else if (op->token_type == OP_MINUS)
        val = (val - next);
      else
        throw logic_error("internal error");
    }

    return val;
  }

  CtrlExprVal parse_multiplicative_expression() {
    auto val = parse_unary_expression();

    while (peek_lookahead()->token_type == OP_STAR ||
           peek_lookahead()->token_type == OP_DIV ||
           peek_lookahead()->token_type == OP_MOD) {
      auto op = pop_lookahead();

      auto next = parse_unary_expression();

      if (op->token_type == OP_STAR)
        val = (val * next);
      else if (op->token_type == OP_DIV) {
        if (next.is_error || !next.eval_bool()) {
          next.is_error = true;
          next.u = 1;
        }

        val = (val / next);
      } else if (op->token_type == OP_MOD) {
        if (next.is_error || !next.eval_bool()) {
          next.is_error = true;
          next.u = 1;
        }

        val = (val % next);
      } else
        throw logic_error("internal error");
    }

    return val;
  }

  CtrlExprVal parse_unary_expression() {
    if (peek_lookahead()->token_type == OP_PLUS ||
        peek_lookahead()->token_type == OP_MINUS ||
        peek_lookahead()->token_type == OP_LNOT ||
        peek_lookahead()->token_type == OP_COMPL) {
      auto op = pop_lookahead();

      auto val = parse_unary_expression();

      switch (op->token_type) {
        case OP_PLUS:
          return +val;
        case OP_MINUS:
          return -val;
        case OP_LNOT:
          return !val;
        case OP_COMPL:
          return ~val;
        default:
          throw logic_error("internal error");
      }
    } else
      return parse_primary_expression();
  }

  CtrlExprVal parse_primary_expression() {
    if (peek_lookahead()->token_type == OP_LPAREN) {
      pop_lookahead();

      auto val = parse_conditional_expression();

      auto rparen = pop_lookahead();

      if (rparen->token_type != OP_RPAREN)
        throw logic_error("closing bracket expected in controlling expression");

      return val;
    }

    auto tok = pop_lookahead();

    if (tok->token_type == TT_LITERAL) {
      auto p = dynamic_cast<LiteralToken*>(tok.get());
      assert(p);
      CtrlExprVal val(*p);
      return val;
    } else if (tok->token_type == TT_IDENTIFIER) {
      auto p = dynamic_cast<IdentifierToken*>(tok.get());
      assert(p);
      string idname = p->src.spelling;

      if (idname == "true")
        return CtrlExprVal(intmax_t(true), false);
      else if (idname == "defined") {
        auto target = pop_lookahead();

        if (target->token_type == OP_LPAREN) {
          target = pop_lookahead();
          if (pop_lookahead()->token_type != OP_RPAREN)
            throw logic_error(
                "no closing paren on defined(identifier) in controlling "
                "expression");
        }

        if (target->token_type != TT_IDENTIFIER)
          throw logic_error(
              "non-identifier after defined in controlling expression");

        auto q = dynamic_cast<IdentifierToken*>(target.get());
        assert(q);
        string targetname = q->src.spelling;

        return CtrlExprVal(intmax_t(is_defined_f(targetname)), false);
      } else
        return CtrlExprVal(intmax_t(false), false);
    } else
      throw logic_error(
          "expected identifier or literal in controlling expression");
  }
};

CtrlExprVal ValFromPpNumber(const PreprocessingToken& pptoken);
string StrFromStringLiteral(SourceInfo src);
