#include "miniptx/ir.h"
#include <sstream>

namespace miniptx {
static int tmpCounter=0;
static std::string newTmp(){ return "%t"+std::to_string(tmpCounter++); }

static std::string lowerExpr(const AST& e, IRFunction& f){
  if (e->type==NodeType::Number) { std::string t=newTmp(); f.instrs.push_back({"const",t,e->value,""}); return t; }
  if (e->type==NodeType::Variable){ return e->name; }
  if (e->type==NodeType::Binary){
    std::string a=lowerExpr(e->children[0],f);
    std::string b=lowerExpr(e->children[1],f);
    std::string t=newTmp();
    std::string op;
    int o=e->op;
    if (o==(int)TokenType::Plus) op="add";
    else if (o==(int)TokenType::Minus) op="sub";
    else if (o==(int)TokenType::Star) op="mul";
    else if (o==(int)TokenType::Slash) op="div";
    else if (o==(int)TokenType::Percent) op="mod";
    else if (o==(int)TokenType::EqEq) op="eq";
    else if (o==(int)TokenType::BangEq) op="ne";
    else if (o==(int)TokenType::Lt) op="lt";
    else if (o==(int)TokenType::LtEq) op="le";
    else if (o==(int)TokenType::Gt) op="gt";
    else if (o==(int)TokenType::GtEq) op="ge";
    else op="add";
    f.instrs.push_back({op,t,a,b});
    return t;
  }
  if (e->type==NodeType::Call){
    std::vector<std::string> args;
    for (auto& c: e->children) args.push_back(lowerExpr(c,f));
    std::string t=newTmp();
    std::string a;
    for (size_t i=0;i<args.size();i++){ if(i) a+=","; a+=args[i]; }
    f.instrs.push_back({"call",t,e->name,a});
    return t;
  }
  if (e->type==NodeType::Unary){
    std::string a=lowerExpr(e->children[0],f);
    std::string t=newTmp();
    f.instrs.push_back({"neg",t,a,""});
    return t;
  }
  return "0";
}

static void lowerStmt(const AST& s, IRFunction& f){
  if (s->type==NodeType::VarDecl){
    if (!s->children.empty()){
      std::string v=lowerExpr(s->children[0],f);
      f.instrs.push_back({"mov",s->name,v,""});
    }
    return;
  }
  if (s->type==NodeType::Assign){
    std::string v=lowerExpr(s->children[0],f);
    f.instrs.push_back({"mov",s->name,v,""});
    return;
  }
  if (s->type==NodeType::Return){
    if (!s->children.empty()){
      std::string v=lowerExpr(s->children[0],f);
      f.instrs.push_back({"ret",v,"",""});
    } else f.instrs.push_back({"ret","","",""});
    return;
  }
  if (s->type==NodeType::If){
    std::string c=lowerExpr(s->children[0],f);
    int id=tmpCounter++;
    f.instrs.push_back({"brz",c,"Lelse"+std::to_string(id),""});
    // then block
    if (s->children[1]->type==NodeType::Block) for (auto& x: s->children[1]->children) lowerStmt(x,f);
    else lowerStmt(s->children[1],f);
    f.instrs.push_back({"jmp","Lend"+std::to_string(id),"" ,""});
    f.instrs.push_back({"label","Lelse"+std::to_string(id),"" ,""});
    if (s->children.size()>2){
      if (s->children[2]->type==NodeType::Block) for (auto& x: s->children[2]->children) lowerStmt(x,f);
      else lowerStmt(s->children[2],f);
    }
    f.instrs.push_back({"label","Lend"+std::to_string(id),"" ,""});
    return;
  }
  if (s->type==NodeType::While){
    int id=tmpCounter++;
    f.instrs.push_back({"label","Lwhile"+std::to_string(id),"" ,""});
    std::string c=lowerExpr(s->children[0],f);
    f.instrs.push_back({"brz",c,"Lend"+std::to_string(id),""});
    if (s->children[1]->type==NodeType::Block) for (auto& x: s->children[1]->children) lowerStmt(x,f);
    else lowerStmt(s->children[1],f);
    f.instrs.push_back({"jmp","Lwhile"+std::to_string(id),"" ,""});
    f.instrs.push_back({"label","Lend"+std::to_string(id),"" ,""});
    return;
  }
  if (s->type==NodeType::Block){
    for (auto& x: s->children) lowerStmt(x,f);
    return;
  }
  if (s->type==NodeType::ExprStmt){
    lowerExpr(s->children[0],f);
    return;
  }
}

IRModule lowerToIR(const AST& ast){
  IRModule m;
  for (auto& fn: ast->children){
    IRFunction f; f.name=fn->name;
    // params
    for (auto& p: fn->params) f.params.push_back(p->name);
    if (!fn->children.empty() && fn->children[0]->type==NodeType::Block){
      for (auto& s: fn->children[0]->children) lowerStmt(s,f);
    }
    m.functions.push_back(f);
  }
  return m;
}

std::string IRModule::dump() const {
  std::ostringstream ss;
  for (auto& f: functions){
    ss << "function " << f.name << "(";
    for (size_t i=0;i<f.params.size();i++){ if(i) ss<<","; ss<<f.params[i]; }
    ss << ")\n";
    for (auto& i: f.instrs){
      ss << "  " << i.op << " " << i.dest;
      if (!i.src1.empty()) ss << " " << i.src1;
      if (!i.src2.empty()) ss << " " << i.src2;
      ss << "\n";
    }
  }
  return ss.str();
}
}
