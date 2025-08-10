#include "ast.h"

using namespace ast;

// Literals
void Number::accept(Visitor &v) { v.visit(*this); }
void String::accept(Visitor &v) { v.visit(*this); }
void Boolean::accept(Visitor &v) { v.visit(*this); }

// Statements
void VariableDecl::accept(Visitor &v) { v.visit(*this); }
void Assignment::accept(Visitor &v) { v.visit(*this); }
void Block::accept(Visitor &v) { v.visit(*this); }
void If::accept(Visitor &v) { v.visit(*this); }
void While::accept(Visitor &v) { v.visit(*this); }
void Return::accept(Visitor &v) { v.visit(*this); }
void ExprStatement::accept(Visitor &v) { v.visit(*this); }

// Expressions
void Variable::accept(Visitor &v) { v.visit(*this); }
void CallExpr::accept(Visitor &v) { v.visit(*this); }
void BinaryOp::accept(Visitor &v) { v.visit(*this); }
void UnaryOp::accept(Visitor &v) { v.visit(*this); }

// Declarations
void Prototype::accept(Visitor &v) { v.visit(*this); }
void Definition::accept(Visitor &v) { v.visit(*this); }

void Parameter::accept(Visitor &v) { v.visit(*this); }
