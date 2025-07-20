#ifndef BASE_H
#define BASE_H

#include <string>
#include <vector>
#include <ostream>

#include "llvm/IR/Value.h"

#include "utils.h"
#include "operators.h"

namespace ast
{
    class ASTNode
    {
    public:
        virtual ~ASTNode() = default;

        virtual std::ostream &print(std::ostream &, size_t = 0) const = 0;

        friend std::ostream &operator<<(std::ostream &out, const ASTNode &obj)
        {
            return obj.print(out);
        }

        virtual llvm::Value *codegen() = 0;
    };

    class Expr : public ASTNode
    {
    };
    class Statement : public ASTNode
    {
    };
    class Declaration : public ASTNode
    {
    };

    class Variable : public Expr
    {
    public:
        std::string name;

        Variable(const std::string &name) : name(name) {}

        std::ostream &print(std::ostream &, size_t) const;
        llvm::Value *codegen() override;
    };

    class BinaryOp : public Expr
    {
        BinaryOpType op;
        std::unique_ptr<Expr> lhs, rhs;

    public:
        BinaryOp(
            BinaryOpType op,
            std::unique_ptr<Expr> lhs,
            std::unique_ptr<Expr> rhs) : op(op),
                                         lhs(std::move(lhs)),
                                         rhs(std::move(rhs))
        {
        }

        std::ostream &print(std::ostream &, size_t) const;
        llvm::Value *codegen() override;
    };

    class UnaryOp : public Expr
    {
        UnaryOpType op;
        std::unique_ptr<Expr> rhs;

    public:
        UnaryOp(
            UnaryOpType op,
            std::unique_ptr<Expr> rhs) : op(op), rhs(std::move(rhs))
        {
        }

        std::ostream &print(std::ostream &, size_t) const;
        llvm::Value *codegen() override;
    };

    class Assignment : public Statement
    {
        std::unique_ptr<Variable> lhs;
        std::unique_ptr<Expr> rhs;

    public:
        Assignment(
            std::unique_ptr<Variable> lhs,
            std::unique_ptr<Expr> rhs) : lhs(std::move(lhs)), rhs(std::move(rhs))
        {
        }

        std::ostream &print(std::ostream &, size_t) const;
        llvm::Value *codegen() override;
    };

    /*
        For expressions that are treated as a statement, e.g:
        - function(1, 2);
        - 1 + 2;
    */
    class ExprStatement : public Statement
    {
        std::unique_ptr<Expr> expression;

    public:
        ExprStatement(std::unique_ptr<Expr> expression) : expression(std::move(expression))
        {
        }

        std::ostream &print(std::ostream &, size_t) const;
        llvm::Value *codegen() override;
    };
};

#endif
