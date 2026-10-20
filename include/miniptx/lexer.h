#pragma once
#include "miniptx/token.h"
#include <string>
#include <vector>

namespace miniptx {

class Lexer {
public:
  explicit Lexer(const std::string& src): src_(src) {}
  std::vector<Token> tokenize();
  const std::vector<std::string>& errors() const { return errors_; }
private:
  std::string src_;
  size_t i_ = 0;
  int line_ = 1, col_ = 1;
  std::vector<std::string> errors_;
  void skipWs();
  Token readToken();
  char peek() const { return i_ < src_.size() ? src_[i_] : '\0'; }
  char advance();
};

} // namespace
