#include "miniptx/lexer.h"
#include "miniptx/parser.h"
#include "miniptx/ir.h"
#include "miniptx/ptx_emitter.h"
#include "miniptx/constant_folding.h"
#include "miniptx/extra_opts.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

int main(int argc, char** argv) {
  std::string mode="compile";
  std::string input, output;
  bool dumpIR=false; bool noOpt=false;
  for (int i=1;i<argc;i++){
    std::string a=argv[i];
    if (a=="check"){ mode="check"; }
    else if (a=="ast"){ mode="ast"; }
    else if (a=="ir"){ mode="ir"; }
    else if (a=="compile"){ mode="compile"; }
    else if (a=="--dump-ir"){ dumpIR=true; }
    else if (a=="--no-opt"){ noOpt=true; }
    else if (a=="-o" && i+1<argc){ output=argv[++i]; }
    else input=a;
  }
  if (input.empty()){ std::cerr<<"usage: miniptx <cmd> file [-o out] [--dump-ir]\n"; return 1; }
  std::ifstream f(input); if(!f){ std::cerr<<"cannot open "<<input<<"\n"; return 1;}
  std::stringstream ss; ss<<f.rdbuf();
  miniptx::Lexer lex(ss.str()); auto toks=lex.tokenize();
  if(!lex.errors().empty()){ for(auto&e:lex.errors()) std::cerr<<"lex: "<<e<<"\n"; return 1;}
  miniptx::Parser p(toks); auto ast=p.parse();
  if(!p.errors().empty()){ for(auto&e:p.errors()) std::cerr<<"parse: "<<e<<"\n"; return 1;}
  if (mode=="check"){ std::cout<<"OK\n"; return 0; }
  if (mode=="ast"){ std::cout<<"AST parsed ("<<ast->children.size()<<" functions)\n"; return 0; }
  auto ir=miniptx::lowerToIR(ast);
  if (!noOpt){ miniptx::constantFolding(ir); miniptx::strengthReduction(ir); miniptx::deadCodeElimination(ir); }
  if (mode=="ir" || dumpIR){ std::cout<<ir.dump(); }
  if (mode=="compile"){
    std::string ptx=miniptx::emitPTX(ir);
    if (output.empty()) std::cout<<ptx;
    else { std::ofstream o(output); o<<ptx; }
  }
  return 0;
}
