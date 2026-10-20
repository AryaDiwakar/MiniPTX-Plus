#pragma once
#include "miniptx/ast.h"
#include <string>
#include <vector>
#include <memory>

namespace miniptx {
struct IRInstr {
  std::string op, dest, src1, src2;
  int line=0;
};
struct IRFunction {
  std::string name;
  std::vector<std::string> params;
  std::vector<IRInstr> instrs;
};
struct IRModule {
  std::vector<IRFunction> functions;
  std::string dump() const;
};
IRModule lowerToIR(const AST& ast);
}
