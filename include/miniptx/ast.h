#pragma once
#include "miniptx/token.h"
#include <string>
#include <vector>
#include <memory>

namespace miniptx {

struct SourceLoc { int line=1, col=1; };

enum class NodeType {
  Program, Function, Param, Block, VarDecl, Assign, If, While, Return, ExprStmt,
  Binary, Unary, Number, Variable, Call
};

struct ASTNode {
  NodeType type;
  SourceLoc loc;
  std::string value;
  std::vector<std::shared_ptr<ASTNode>> children;
  std::vector<std::shared_ptr<ASTNode>> params;
  std::string name;
  // for binary
  int op = 0;
};

using AST = std::shared_ptr<ASTNode>;

} // namespace
