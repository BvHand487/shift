#include "ast.h"

using namespace ast;

Parameter::Parameter(
    const std::string &name,
    Type type,
    std::unique_ptr<Expr> init) : name(name),
                                  type(type),
                                  init(std::move(init)) {}

// - - - - - DECLARATIONS - - - - - //
Prototype::Prototype(
    Type retType,
    const std::string &name,
    std::vector<std::unique_ptr<Parameter>> args,
    bool isExtern,
    bool isVarArg) : retType(retType),
                     name(name),
                     args(std::move(args)),
                     isExtern(isExtern),
                     isVarArg(isVarArg) {}

Definition::Definition(
    std::unique_ptr<Prototype> type,
    std::unique_ptr<Block> body) : type(std::move(type)),
                                   body(std::move(body)) {}

// - - - - - STATEMENTS - - - - - //
VariableDecl::VariableDecl(
    const std::string &name,
    Type type,
    std::unique_ptr<Expr> init) : name(name),
                                  type(type),
                                  init(std::move(init)) {}

Assignment::Assignment(
    std::unique_ptr<Variable> lhs,
    std::unique_ptr<Expr> rhs) : lhs(std::move(lhs)),
                                 rhs(std::move(rhs)) {}

// constructor for a list of statements
Block::Block(std::vector<std::unique_ptr<Statement>> stmts) : statements(std::move(stmts)) {}

// constructor for single-statement bodies
Block::Block(std::unique_ptr<Statement> stmt)
{
    statements.push_back(std::move(stmt));
}


If::If(
    std::unique_ptr<Expr> cond,
    std::unique_ptr<Block> then_branch,
    std::unique_ptr<Block> else_branch) : cond(std::move(cond)),
                                          then_branch(std::move(then_branch)),
                                          else_branch(std::move(else_branch)) {}

While::While(
    std::unique_ptr<Expr> cond,
    std::unique_ptr<Block> body) : cond(std::move(cond)),
                                   body(std::move(body)) {}

Return::Return(std::unique_ptr<Expr> value) : value(std::move(value)) {}

ExprStatement::ExprStatement(std::unique_ptr<Expr> expression) : expression(std::move(expression)) {}

// - - - - - EXPRESSIONS - - - - - //
Variable::Variable(const std::string &name) : name(name) {}

CallExpr::CallExpr(
    const std::string &callee,
    std::vector<std::unique_ptr<Expr>> args) : callee(callee),
                                               args(std::move(args)) {}

BinaryOp::BinaryOp(
    BinaryOpType op,
    std::unique_ptr<Expr> lhs,
    std::unique_ptr<Expr> rhs) : op(op),
                                 lhs(std::move(lhs)),
                                 rhs(std::move(rhs)) {}

UnaryOp::UnaryOp(
    UnaryOpType op,
    std::unique_ptr<Expr> rhs) : op(op),
                                 rhs(std::move(rhs)) {}
