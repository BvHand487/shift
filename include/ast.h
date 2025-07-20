#ifndef AST_H
#define AST_H

#include <string>
#include <memory>
#include <vector>
#include <ostream>

#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Value.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/StandardInstrumentations.h"

#include "utils.h"

extern std::unique_ptr<llvm::IRBuilder<>> builder;
extern std::unique_ptr<llvm::LLVMContext> context;
extern std::unique_ptr<llvm::Module> module;

extern std::unique_ptr<llvm::FunctionPassManager> theFPM;
extern std::unique_ptr<llvm::LoopAnalysisManager> theLAM;
extern std::unique_ptr<llvm::FunctionAnalysisManager> theFAM;
extern std::unique_ptr<llvm::CGSCCAnalysisManager> theCGAM;
extern std::unique_ptr<llvm::ModuleAnalysisManager> theMAM;
extern std::unique_ptr<llvm::PassInstrumentationCallbacks> thePIC;
extern std::unique_ptr<llvm::StandardInstrumentations> theSI;


class ASTNode
{
public:
    virtual ~ASTNode() = default;

    virtual std::ostream& print(std::ostream&, size_t = 0) const = 0;

    friend std::ostream& operator<<(std::ostream& out, const ASTNode& obj)
    {
        return obj.print(out);
    }

    virtual llvm::Value *codegen() = 0;
    
};
class Expr : public ASTNode
{
public:
    virtual ~Expr() = default;
};

// --- VARIABLES --- //
class Variable : public Expr
{
    std::string name;

public:
    Variable(const std::string& name) : name(name) {}
    llvm::Value *codegen() override;

    std::ostream& print(std::ostream&, size_t) const;
    const std::string& getName() const { return name; }
};

// --- LITERALS  --- //
template <class T>
class Literal : public Expr
{
protected:
    T value;

public:
    explicit Literal(T value) : value(value) {}
};

class Number : public Literal<double>
{
private:
    std::ostream& print(std::ostream&, size_t) const;

public:
    using Literal::Literal;
    llvm::Value *codegen() override;
};

class Boolean : public Literal<bool>
{
private:
    std::ostream& print(std::ostream&, size_t) const;

public:
    using Literal::Literal;
    llvm::Value *codegen() override;
};

class String : public Literal<std::string>
{
private:
    std::ostream& print(std::ostream&, size_t) const;

public:
    using Literal::Literal;
    llvm::Value *codegen() override;
};

// --- BINARY OPERATORS --- //
enum class BinaryOps
{
    None = -1,

    // Arithmetic
    Addition,
    Subtraction,
    Multiplication,
    Division,
    Exponentiation,

    // Logical
    And,
    Or,

    // Bitwise
    BitXor,
    BitAnd,
    BitOr,

    // Comparison
    Greater,
    GreaterEqual,
    Less,
    LessEqual,
    Equal,
    NotEqual
};

class BinaryOp : public Expr
{
    BinaryOps op;
    std::unique_ptr<Expr> lhs, rhs;

    std::ostream& print(std::ostream&, size_t) const;

public:
    BinaryOp(
        BinaryOps op,
        std::unique_ptr<Expr> lhs,
        std::unique_ptr<Expr> rhs) : op(op),
        lhs(std::move(lhs)),
        rhs(std::move(rhs)) {
    }

    llvm::Value *codegen() override;
};

// --- UNARY OPERATORS --- //
enum class UnaryOps
{
    None = -1,
    Addition,
    Subtraction,
    Not,
    BitwiseNot
};

class UnaryOp : public Expr
{
    UnaryOps op;
    std::unique_ptr<Expr> rhs;

    std::ostream& print(std::ostream&, size_t) const;

public:
    UnaryOp(
        UnaryOps op,
        std::unique_ptr<Expr> rhs) : op(op), rhs(std::move(rhs)) {
    }

    llvm::Value *codegen() override;
};

class Statement : public ASTNode
{
public:
    virtual ~Statement() = default;
    virtual llvm::Value *codegen() = 0;
};

// --- ASSIGNMENT --- //
class Assignment : public Statement
{
    std::unique_ptr<Variable> lhs;
    std::unique_ptr<Expr> rhs;

    std::ostream& print(std::ostream&, size_t) const;

public:
    Assignment(
        std::unique_ptr<Variable> lhs,
        std::unique_ptr<Expr> rhs) : lhs(std::move(lhs)), rhs(std::move(rhs)) {
    }

    llvm::Value *codegen() override;
};

// --- FUNCTIONS --- //
class Function : public Statement
{
    std::string name;
    std::vector<std::unique_ptr<Variable>> args;
    std::vector<std::unique_ptr<Statement>> body;

    std::ostream& print(std::ostream&, size_t) const;

public:
    Function(
        const std::string& name,
        std::vector<std::unique_ptr<Variable>> args,
        std::vector<std::unique_ptr<Statement>> body) : name(name), args(std::move(args)), body(std::move(body)) {
    }

    llvm::Value *codegen() override;
};

class Return : public Statement
{
    std::unique_ptr<Expr> value;

    std::ostream& print(std::ostream&, size_t) const;

public:
    Return(std::unique_ptr<Expr> value) : value(std::move(value)) { }

    llvm::Value *codegen() override;
};

class Call : public Expr
{
    std::string callee;
    std::vector<std::unique_ptr<Expr>> args;

    std::ostream& print(std::ostream&, size_t) const;

public:
    Call(
        const std::string& callee,
        std::vector<std::unique_ptr<Expr>> args) : callee(callee), args(std::move(args)) {
    }

    llvm::Value *codegen() override;
};


class IfStatement : public Statement
{
    std::unique_ptr<Expr> cond;
    std::vector<std::unique_ptr<Statement>> then_branch, else_branch;

    std::ostream& print(std::ostream&, size_t) const;

public:
    IfStatement(
        std::unique_ptr<Expr> cond,
        std::vector<std::unique_ptr<Statement>> then_branch,
        std::vector<std::unique_ptr<Statement>> else_branch) : cond(std::move(cond)),
        then_branch(std::move(then_branch)),
        else_branch(std::move(else_branch)) {
    }

    llvm::Value *codegen() override;
};

class WhileStatement : public Statement
{
    std::unique_ptr<Expr> cond;
    std::vector<std::unique_ptr<Statement>> body;

    std::ostream& print(std::ostream&, size_t) const;

public:
    WhileStatement(
        std::unique_ptr<Expr> cond,
        std::vector<std::unique_ptr<Statement>> body) : cond(std::move(cond)),
        body(std::move(body)) {
    }

    llvm::Value *codegen() override;
};

#endif
