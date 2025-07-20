#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <ostream>

#include "base.h"

namespace ast::literals
{
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
    public:
        using Literal::Literal;
        std::ostream &print(std::ostream &, size_t) const;
        llvm::Value *codegen() override;
    };

    class Boolean : public Literal<bool>
    {
    public:
        using Literal::Literal;
        std::ostream &print(std::ostream &, size_t) const;
        llvm::Value *codegen() override;
    };

    class String : public Literal<std::string>
    {
    public:
        using Literal::Literal;
        std::ostream &print(std::ostream &, size_t) const;
        llvm::Value *codegen() override;
    };
};

#endif
