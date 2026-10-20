#include "miniptx/lexer.h"
#include <cctype>

namespace miniptx {

char Lexer::advance() {
  char c = src_[i_++];
  if (c == '\n') { line_++; col_ = 1; } else col_++;
  return c;
}

void Lexer::skipWs() {
  while (i_ < src_.size()) {
    char c = peek();
    if (c == ' ' || c == '\t' || c == '\r' || c == '\n') advance();
    else if (c == '/' && i_+1 < src_.size() && src_[i_+1] == '/') {
      while (i_ < src_.size() && src_[i_] != '\n') advance();
    } else if (c == '/' && i_+1 < src_.size() && src_[i_+1] == '*') {
      advance(); advance();
      while (i_ < src_.size() && !(src_[i_] == '*' && i_+1 < src_.size() && src_[i_+1] == '/')) advance();
      if (i_ < src_.size()) { advance(); advance(); }
    } else break;
  }
}

Token Lexer::readToken() {
  skipWs();
  Token t; t.line = line_; t.col = col_;
  if (i_ >= src_.size()) { t.type = TokenType::EndOfFile; t.text = ""; return t; }
  char c = peek();
  if (std::isalpha((unsigned char)c) || c == '_') {
    std::string s;
    while (i_ < src_.size() && (std::isalnum((unsigned char)peek()) || peek() == '_')) s += advance();
    // keywords
    if (s == "int") t.type = TokenType::Int;
    else if (s == "return") t.type = TokenType::Return;
    else if (s == "if") t.type = TokenType::If;
    else if (s == "else") t.type = TokenType::Else;
    else if (s == "while") t.type = TokenType::While;
    else if (s == "void") t.type = TokenType::Void;
    else { t.type = TokenType::Ident; }
    t.text = s; return t;
  }
  if (std::isdigit((unsigned char)c)) {
    std::string s;
    while (i_ < src_.size() && std::isdigit((unsigned char)peek())) s += advance();
    t.type = TokenType::Number; t.text = s; return t;
  }
  // two-char
  auto two = [&](const std::string& op, TokenType ty) {
    if (i_+1 < src_.size() && src_[i_] == op[0] && src_[i_+1] == op[1]) {
      advance(); advance(); t.type = ty; t.text = op; return true;
    } return false;
  };
  if (two("==", TokenType::EqEq)) return t;
  if (two("!=", TokenType::BangEq)) return t;
  if (two("<=", TokenType::LtEq)) return t;
  if (two(">=", TokenType::GtEq)) return t;
  if (two("&&", TokenType::AndAnd)) return t;
  if (two("||", TokenType::OrOr)) return t;
  // single
  char ch = advance();
  switch (ch) {
    case '+': t.type=TokenType::Plus; break;
    case '-': t.type=TokenType::Minus; break;
    case '*': t.type=TokenType::Star; break;
    case '/': t.type=TokenType::Slash; break;
    case '%': t.type=TokenType::Percent; break;
    case '=': t.type=TokenType::Eq; break;
    case '!': t.type=TokenType::Bang; break;
    case '<': t.type=TokenType::Lt; break;
    case '>': t.type=TokenType::Gt; break;
    case '(': t.type=TokenType::LParen; break;
    case ')': t.type=TokenType::RParen; break;
    case '{': t.type=TokenType::LBrace; break;
    case '}': t.type=TokenType::RBrace; break;
    case '[': t.type=TokenType::LBracket; break;
    case ']': t.type=TokenType::RBracket; break;
    case ',': t.type=TokenType::Comma; break;
    case ';': t.type=TokenType::Semicolon; break;
    case ':': t.type=TokenType::Colon; break;
    default: t.type=TokenType::Unknown; errors_.push_back("Unknown char: " + std::string(1,ch));
  }
  t.text = std::string(1,ch);
  return t;
}

std::vector<Token> Lexer::tokenize() {
  std::vector<Token> toks;
  while (true) {
    Token t = readToken();
    toks.push_back(t);
    if (t.type == TokenType::EndOfFile) break;
  }
  return toks;
}

} // namespace
