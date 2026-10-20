#include "miniptx/constant_folding.h"
#include <string>
namespace miniptx {
void constantFolding(IRModule& m){
  for (auto& f: m.functions){
    for (auto& i: f.instrs){
      if ((i.op=="add"||i.op=="sub"||i.op=="mul") ){
        // if both operands numeric, fold
        char* e1; char* e2;
        long v1=strtol(i.src1.c_str(),&e1,10);
        long v2=strtol(i.src2.c_str(),&e2,10);
        if (*e1=='\0' && *e2=='\0'){
          long r=0;
          if (i.op=="add") r=v1+v2;
          else if (i.op=="sub") r=v1-v2;
          else if (i.op=="mul") r=v1*v2;
          i.op="const"; i.src1=std::to_string(r); i.src2="";
        }
      }
    }
  }
}
}
