#include "miniptx/extra_opts.h"
#include <string>
namespace miniptx {
void strengthReduction(IRModule& m){
  for (auto& f: m.functions){
    for (auto& i: f.instrs){
      if (i.op=="mul" && i.src2=="2"){ i.op="add"; i.src2=i.src1; }
    }
  }
}
void deadCodeElimination(IRModule& m){
  // remove mov to unused? simple: remove const that never used (naive)
  for (auto& f: m.functions){
    // placeholder: no-op safe
  }
}
}
