#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string>
#include <memory>
#include <vector>
#include <ostream>

#include "base.h"
#include "control.h"
#include "utils.h"

namespace ast::functions
{
    class Prototype : public Declaration
    {
        std::string name;
        std::vector<std::unique_ptr<Variable>> args;
        
    public:
        Prototype(
            const std::string &name,
            std::vector<std::unique_ptr<Variable>> args) : name(name), args(std::move(args))
            {
            }
            
        std::ostream &print(std::ostream &, size_t) const;
        llvm::Value *codegen() override;
    };

    class Definition : public Declaration
    {
        std::unique_ptr<Prototype> type;
        std::unique_ptr<control::Block> body;

    public:
        Definition(
            std::unique_ptr<Prototype> type,
            std::unique_ptr<control::Block> body) : type(std::move(type)), body(std::move(body))
        {
        }

        std::ostream &print(std::ostream &, size_t) const;
        llvm::Value *codegen() override;
    };

    class Return : public Statement
    {
        std::unique_ptr<Expr> value;

    public:
        Return(std::unique_ptr<Expr> value) : value(std::move(value)) {}
    
        std::ostream &print(std::ostream &, size_t) const;
        llvm::Value *codegen() override;
    };

    class CallExpr : public Expr
    {
        std::string callee;
        std::vector<std::unique_ptr<Expr>> args;

    public:
        CallExpr(
            const std::string &callee,
            std::vector<std::unique_ptr<Expr>> args) : callee(callee), args(std::move(args))
        {
        }

        std::ostream &print(std::ostream &, size_t) const;
        llvm::Value *codegen() override;
    };
};

#endif
