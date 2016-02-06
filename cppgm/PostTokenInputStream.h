#pragma once

#include "IPPTokenStream.h"

#include "ETokenType.h"
#include "IPostTokenOutputStream.h"
#include "Tokens.h"
#include "Tokenizer.h"
#include "IPPTokenStream.h"

struct PostTokenInputStream : IPPTokenStream {
  vector<PPStringLiteral> string_literals;

  IPostTokenOutputStream& output;

  PostTokenInputStream(IPostTokenOutputStream& output) : output(output) {}

  void emit_whitespace_sequence() {}
  virtual void emit_new_line() {}

  virtual void emit_header_name(SourceInfo src) {
    terminate_string_literals();

    output.emit_invalid(src);
  }

  virtual void emit_non_whitespace_char(SourceInfo src) {
    terminate_string_literals();

    output.emit_invalid(src);
  }

  virtual void emit_identifier(SourceInfo src) {
    terminate_string_literals();

    ETokenType tt = PPTokenToSimpleTokenType(src.spelling);

    if (tt == TT_INVALID)
      output.emit_identifier(src);
    else
      output.emit_simple(src, tt);
  }

  virtual void emit_preprocessing_op_or_punc(SourceInfo src) {
    terminate_string_literals();

    ETokenType tt = PPTokenToSimpleTokenType(src.spelling);

    if (tt == TT_INVALID)
      output.emit_invalid(src);
    else
      output.emit_simple(src, tt);
  }

  virtual void emit_pp_number(SourceInfo src) {
    terminate_string_literals();

    try {
      PToken token = TokenizePPNumber(src);
      switch (token->token_type) {
        case TT_LITERAL: {
          auto p = dynamic_cast<LiteralToken*>(token.get());
          output.emit_literal(src, p->fundamental_type, p->data.data(),
                              p->data.size());
        } break;

        case TT_UDINTEGER: {
          auto p = dynamic_cast<UserDefinedIntegerLiteralToken*>(token.get());
          output.emit_user_defined_literal_integer(src, p->ud_suffix,
                                                   p->source);
        } break;

        case TT_UDFLOATING: {
          auto p = dynamic_cast<UserDefinedFloatingLiteralToken*>(token.get());
          output.emit_user_defined_literal_floating(src, p->ud_suffix,
                                                    p->source);
        } break;

        default:
          throw logic_error("unexpected pp-number: " + src.spelling);
      }
    } catch (exception& e) {
      output.emit_invalid(src);
      cerr << "ERROR: " << e.what() << endl;
    }
  }

  virtual void emit_character_literal(SourceInfo src) {
    terminate_string_literals();

    try {
      PToken token = TokenizeCharacterLiteral(src);
      auto p = dynamic_cast<LiteralToken*>(token.get());
      output.emit_literal(src, p->fundamental_type, p->data.data(),
                          p->data.size());
    } catch (exception& e) {
      output.emit_invalid(src);
      cerr << "ERROR: " << e.what() << endl;
    }
  }

  virtual void emit_user_defined_character_literal(SourceInfo src) {
    terminate_string_literals();

    try {
      PToken token = TokenizeUserDefinedCharacterLiteral(src);
      auto p = dynamic_cast<UserDefinedCharacterLiteralToken*>(token.get());
      output.emit_user_defined_literal_character(
          src, p->ud_suffix, p->fundamental_type, p->data.data(),
          p->data.size());
    } catch (exception& e) {
      output.emit_invalid(src);
      cerr << "ERROR: " << e.what() << endl;
    }
  }

  void terminate_string_literals() {
    if (string_literals.empty()) return;

    SourceInfo src;
    src.filename = string_literals[0].src.filename;
    src.linenum = string_literals[0].src.linenum;

    for (const PPStringLiteral& string_literal : string_literals) {
      src.spelling += string_literal.src.spelling;
      src.spelling += " ";
    }

    src.spelling.erase(src.spelling.end() - 1);

    try {
      PToken token = TokenizeStringLiteralSequence(src, string_literals);

      if (token->token_type == TT_UDSTRING) {
        auto p = dynamic_cast<UserDefinedStringLiteralToken*>(token.get());
        output.emit_user_defined_literal_string_array(
            src, p->ud_suffix, p->num_elements, p->fundamental_type,
            p->data.data(), p->data.size());
      } else if (token->token_type == TT_LITERAL) {
        auto p = dynamic_cast<LiteralToken*>(token.get());
        output.emit_literal_array(src, p->num_elements, p->fundamental_type,
                                  p->data.data(), p->data.size());
      } else
        throw("unexpected token_type from TokenizeStringLiterals: " +
              SimpleTokenTypeToString(token->token_type));
    } catch (exception& e) {
      output.emit_invalid(src);
      cerr << "ERROR: " << e.what() << endl;
    }

    string_literals.clear();
  }

  virtual void emit_string_literal(SourceInfo src) {
    string_literals.push_back(PPStringLiteral{false, src});

    //		try
    //		{
    //			PToken token = TokenizeStringLiteral(source);
    //			auto p = dynamic_cast<LiteralToken*>(token.get());
    //			output.emit_literal_array(source, p->num_elements,
    //p->fundamental_type, p->data.data(), p->data.size());
    //		}
    //		catch (exception& e)
    //		{
    //			output.emit_invalid(source);
    //			cerr << "ERROR: " << e.what() << endl;
    //		}
  }

  virtual void emit_user_defined_string_literal(SourceInfo src) {
    string_literals.push_back(PPStringLiteral{true, src});

    //		try
    //		{
    //			PToken token = TokenizeUserDefinedStringLiteral(source);
    //			auto p =
    //dynamic_cast<UserDefinedStringLiteralToken*>(token.get());
    //			output.emit_user_defined_literal_string_array(source, p->ud_suffix,
    //p->num_elements, p->fundamental_type, p->data.data(), p->data.size());
    //		}
    //		catch (exception& e)
    //		{
    //			output.emit_invalid(source);
    //			cerr << "ERROR: " << e.what() << endl;
    //		}
  }

  virtual void emit_eof() {
    terminate_string_literals();

    output.emit_eof();
  }
};
