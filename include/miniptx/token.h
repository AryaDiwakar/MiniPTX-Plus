#pragma once
#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <optional>

namespace miniptx {

enum class TokenType {
  Int, Return, If, Else, While, For, Void,
  Ident, Number,
  Plus, Minus, Star, Slash, Percent,
  Eq, EqEq, Bang, BangEq, Lt, LtEq, Gt, GtEq,
  AndAnd, OrOr,
  LParen, RParen, LBrace, RBrace, LBracket, RBracket,
  Comma, Semicolon, Colon,
  EndOfFile, Unknown
};

struct Token {
  TokenType type;
  std::string text;
  int line = 1;
  int col = 1;
};

} // namespace
