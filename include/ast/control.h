#ifndef CONTROL_H
#define CONTROL_H

#include <vector>
#include <ostream>

#include "base.h"

namespace ast::control
{
    class Block : public Statement
    {
        std::vector<std::unique_ptr<Statement>> statements;

    public:
        Block(std::vector<std::unique_ptr<Statement>> stmts) : statements(std::move(stmts)) {}

        std::ostream &print(std::ostream &, size_t) const;
        llvm::Value *codegen() override;
    };

    class If : public Statement
    {
        std::unique_ptr<Expr> cond;
        std::unique_ptr<Block> then_branch, else_branch;

    public:
        If(
            std::unique_ptr<Expr> cond,
            std::unique_ptr<Block> then_branch,
            std::unique_ptr<Block> else_branch) : cond(std::move(cond)), then_branch(std::move(then_branch)), else_branch(std::move(else_branch))
        {
        }
        
        std::ostream &print(std::ostream &, size_t) const;
        llvm::Value *codegen() override;
    };

    class While : public Statement
    {
        std::unique_ptr<Expr> cond;
        std::unique_ptr<Block> body;

    public:
        While(
            std::unique_ptr<Expr> cond,
            std::unique_ptr<Block> body) : cond(std::move(cond)), body(std::move(body))
        {
        }

        std::ostream &print(std::ostream &, size_t) const;
        llvm::Value *codegen() override;
    };
};

#endif
