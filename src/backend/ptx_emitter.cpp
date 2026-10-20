#include "miniptx/ir.h"
#include <sstream>

namespace miniptx {
std::string emitPTX(const IRModule& m){
  std::ostringstream ss;
  ss << ".version 7.0\n";
  ss << ".target sm_50\n";
  ss << ".address_size 64\n\n";
  for (auto& f: m.functions){
    ss << ".func " << f.name << "(\n";
    ss << ")\n";
    ss << "{\n";
    ss << "  .reg .s32 %r<64>;\n";
    for (auto& i: f.instrs){
      if (i.op=="const") ss << "  mov.s32 " << i.dest << ", " << i.src1 << ";\n";
      else if (i.op=="mov") ss << "  mov.s32 " << i.dest << ", " << i.src1 << ";\n";
      else if (i.op=="add") ss << "  add.s32 " << i.dest << ", " << i.src1 << ", " << i.src2 << ";\n";
      else if (i.op=="sub") ss << "  sub.s32 " << i.dest << ", " << i.src1 << ", " << i.src2 << ";\n";
      else if (i.op=="mul") ss << "  mul.lo.s32 " << i.dest << ", " << i.src1 << ", " << i.src2 << ";\n";
      else if (i.op=="div") ss << "  div.s32 " << i.dest << ", " << i.src1 << ", " << i.src2 << ";\n";
      else if (i.op=="mod") ss << "  rem.s32 " << i.dest << ", " << i.src1 << ", " << i.src2 << ";\n";
      else if (i.op=="label") ss << i.dest << ":\n";
      else if (i.op=="jmp") ss << "  bra " << i.dest << ";\n";
      else if (i.op=="brz") ss << "  setp.eq.s32 %p1, " << i.src1 << ", 0;\n  @%p1 bra " << i.dest << ";\n";
      else if (i.op=="ret") { if(!i.dest.empty()) ss << "  mov.s32 %retval, " << i.dest << ";\n"; ss << "  ret;\n"; }
      else ss << "  // " << i.op << "\n";
    }
    ss << "}\n\n";
  }
  return ss.str();
}
}
