#include "miniptx/parser.h"

namespace miniptx {

Parser::Parser(const std::vector<Token>& toks): toks_(toks) {}

Token Parser::peek(int off){ size_t idx=i_+off; if (idx>=toks_.size()) return toks_.back(); return toks_[idx]; }
Token Parser::advance(){ if (i_<toks_.size()) return toks_[i_++]; return toks_.back(); }
bool Parser::match(TokenType t){ if (peek().type==t){ advance(); return true;} return false;}

AST Parser::parse(){ return parseProgram(); }

AST Parser::parseProgram(){
  auto n=std::make_shared<ASTNode>(); n->type=NodeType::Program; n->loc={peek().line,peek().col};
  while (peek().type!=TokenType::EndOfFile){
    if (peek().type==TokenType::Int || peek().type==TokenType::Void){
      n->children.push_back(parseFunction());
    } else { errors_.push_back("Expected function at top level"); advance(); }
  }
  return n;
}

AST Parser::parseFunction(){
  auto n=std::make_shared<ASTNode>(); n->type=NodeType::Function; n->loc={peek().line,peek().col};
  // type
  advance(); // int/void
  if (peek().type!=TokenType::Ident){ errors_.push_back("Expected function name"); }
  n->name=peek().text; advance();
  if (!match(TokenType::LParen)){ errors_.push_back("Expected ("); }
  while (peek().type!=TokenType::RParen && peek().type!=TokenType::EndOfFile){
    auto p=std::make_shared<ASTNode>(); p->type=NodeType::Param; p->name=peek().text; 
    // type
    // assume int param: if ident type? skip
    advance();
    n->params.push_back(p);
    if (peek().type==TokenType::Comma) advance();
  }
  match(TokenType::RParen);
  n->children.push_back(parseBlock());
  return n;
}

AST Parser::parseBlock(){
  auto n=std::make_shared<ASTNode>(); n->type=NodeType::Block; n->loc={peek().line,peek().col};
  if (!match(TokenType::LBrace)){ errors_.push_back("Expected {"); }
  while (peek().type!=TokenType::RBrace && peek().type!=TokenType::EndOfFile){
    n->children.push_back(parseStmt());
  }
  match(TokenType::RBrace);
  return n;
}

AST Parser::parseStmt(){
  if (peek().type==TokenType::Int){
    auto n=std::make_shared<ASTNode>(); n->type=NodeType::VarDecl; n->loc={peek().line,peek().col};
    advance();
    n->name=peek().text; advance();
    if (match(TokenType::Eq)) n->children.push_back(parseExpr());
    match(TokenType::Semicolon);
    return n;
  }
  if (peek().type==TokenType::If){
    auto n=std::make_shared<ASTNode>(); n->type=NodeType::If; n->loc={peek().line,peek().col};
    advance();
    match(TokenType::LParen);
    n->children.push_back(parseExpr());
    match(TokenType::RParen);
    n->children.push_back(parseBlock());
    if (peek().type==TokenType::Else){ advance(); n->children.push_back(parseBlock()); }
    return n;
  }
  if (peek().type==TokenType::While){
    auto n=std::make_shared<ASTNode>(); n->type=NodeType::While; n->loc={peek().line,peek().col};
    advance();
    match(TokenType::LParen);
    n->children.push_back(parseExpr());
    match(TokenType::RParen);
    n->children.push_back(parseBlock());
    return n;
  }
  if (peek().type==TokenType::Return){
    auto n=std::make_shared<ASTNode>(); n->type=NodeType::Return; n->loc={peek().line,peek().col};
    advance();
    if (peek().type!=TokenType::Semicolon) n->children.push_back(parseExpr());
    match(TokenType::Semicolon);
    return n;
  }
  // assignment or expr
  if (peek().type==TokenType::Ident && peek(1).type==TokenType::Eq){
    auto n=std::make_shared<ASTNode>(); n->type=NodeType::Assign; n->loc={peek().line,peek().col};
    n->name=peek().text; advance(); advance();
    n->children.push_back(parseExpr());
    match(TokenType::Semicolon);
    return n;
  }
  auto e=parseExpr();
  match(TokenType::Semicolon);
  auto n=std::make_shared<ASTNode>(); n->type=NodeType::ExprStmt; n->loc=e->loc; n->children.push_back(e);
  return n;
}

AST Parser::parseExpr(){ return parseAssign(); }
AST Parser::parseAssign(){ return parseEquality(); }

AST Parser::parseEquality(){
  auto node=parseRelational();
  while (peek().type==TokenType::EqEq || peek().type==TokenType::BangEq){
    auto op=advance().type;
    auto r=parseRelational();
    auto b=std::make_shared<ASTNode>(); b->type=NodeType::Binary; b->loc=node->loc; b->op=(int)op; b->children={node,r}; node=b;
  }
  return node;
}
AST Parser::parseRelational(){
  auto node=parseAdditive();
  while (peek().type==TokenType::Lt || peek().type==TokenType::LtEq || peek().type==TokenType::Gt || peek().type==TokenType::GtEq){
    auto op=advance().type;
    auto r=parseAdditive();
    auto b=std::make_shared<ASTNode>(); b->type=NodeType::Binary; b->loc=node->loc; b->op=(int)op; b->children={node,r}; node=b;
  }
  return node;
}
AST Parser::parseAdditive(){
  auto node=parseMultiplicative();
  while (peek().type==TokenType::Plus || peek().type==TokenType::Minus){
    auto op=advance().type;
    auto r=parseMultiplicative();
    auto b=std::make_shared<ASTNode>(); b->type=NodeType::Binary; b->loc=node->loc; b->op=(int)op; b->children={node,r}; node=b;
  }
  return node;
}
AST Parser::parseMultiplicative(){
  auto node=parseUnary();
  while (peek().type==TokenType::Star || peek().type==TokenType::Slash || peek().type==TokenType::Percent){
    auto op=advance().type;
    auto r=parseUnary();
    auto b=std::make_shared<ASTNode>(); b->type=NodeType::Binary; b->loc=node->loc; b->op=(int)op; b->children={node,r}; node=b;
  }
  return node;
}
AST Parser::parseUnary(){
  if (peek().type==TokenType::Minus || peek().type==TokenType::Bang){
    auto op=advance().type;
    auto u=std::make_shared<ASTNode>(); u->type=NodeType::Unary; u->loc={peek().line,peek().col}; u->op=(int)op; u->children={parseUnary()}; return u;
  }
  return parsePrimary();
}
AST Parser::parsePrimary(){
  if (peek().type==TokenType::Number){
    auto n=std::make_shared<ASTNode>(); n->type=NodeType::Number; n->loc={peek().line,peek().col}; n->value=peek().text; advance(); return n;
  }
  if (peek().type==TokenType::Ident){
    if (peek(1).type==TokenType::LParen){
      auto c=std::make_shared<ASTNode>(); c->type=NodeType::Call; c->loc={peek().line,peek().col}; c->name=peek().text; advance(); advance();
      while (peek().type!=TokenType::RParen){
        c->children.push_back(parseExpr());
        if (peek().type==TokenType::Comma) advance();
      }
      match(TokenType::RParen);
      return c;
    }
    auto v=std::make_shared<ASTNode>(); v->type=NodeType::Variable; v->loc={peek().line,peek().col}; v->name=peek().text; advance(); return v;
  }
  if (match(TokenType::LParen)){
    auto e=parseExpr();
    match(TokenType::RParen);
    return e;
  }
  errors_.push_back("Unexpected token in expression");
  advance();
  auto n=std::make_shared<ASTNode>(); n->type=NodeType::Number; n->value="0"; return n;
}
}
