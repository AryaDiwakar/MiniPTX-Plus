#pragma once
#include "miniptx/token.h"
#include "miniptx/ast.h"
#include <vector>
#include <string>

namespace miniptx {
class Parser {
public:
  Parser(const std::vector<Token>& toks);
  AST parse();
  const std::vector<std::string>& errors() const { return errors_; }
private:
  std::vector<Token> toks_;
  size_t i_=0;
  std::vector<std::string> errors_;
  Token peek(int off=0);
  Token advance();
  bool match(TokenType t);
  AST parseProgram();
  AST parseFunction();
  AST parseBlock();
  AST parseStmt();
  AST parseExpr();
  AST parseAssign();
  AST parseEquality();
  AST parseRelational();
  AST parseAdditive();
  AST parseMultiplicative();
  AST parseUnary();
  AST parsePrimary();
};
}
