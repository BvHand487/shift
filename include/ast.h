#ifndef AST_H
#define AST_H


#include <string>
#include <memory>
#include <vector>
#include <ostream>


#include "utils.h"


class ASTNode
{
public:
    virtual ~ASTNode() = default;

    virtual std::ostream& print(std::ostream& out, size_t indent=0) const = 0;

    friend std::ostream& operator<< (std::ostream& out, const ASTNode& obj)
    {
        return obj.print(out);
    }
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

    std::ostream& print(std::ostream& out, size_t indent=0) const
    {
        out << create_indent(indent) << "Variable(" << this->name << ")";
        return out;
    }

public:
    Variable(const std::string& name) : name(name) {}
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

class Number : public Literal<double> {
private:
    std::ostream& print(std::ostream& out, size_t indent=0) const
    {
        out << create_indent(indent) << "Number(" << this->value << ")";
        return out;
    }

public:
    using Literal::Literal;
};

class Boolean : public Literal<bool> {
private:
    std::ostream& print(std::ostream& out, size_t indent=0) const
    {
        out << create_indent(indent) << "Boolean(" << std::boolalpha << this->value << ")";
        return out;
    }
public:
    using Literal::Literal;
};

class String : public Literal<std::string> {
private:
    std::ostream& print(std::ostream& out, size_t indent=0) const
    {
        out << create_indent(indent) << "String(" << this->value << ")";
        return out;
    }

public:
    using Literal::Literal;
};


// --- BINARY OPERATORS --- //
enum class BinaryOps
{
    None = -1,

    // Arithmetic
    Addition, Subtraction, Multiplication, Division, Exponentiation,

    // Logical
    And, Or,

    // Bitwise
    BitXor, BitAnd, BitOr,

    // Comparison
    Greater, GreaterEqual, Less, LessEqual, Equal, NotEqual
};

class BinaryOp : public Expr
{
    BinaryOps op;
    std::unique_ptr<Expr> lhs, rhs;

    std::ostream& print(std::ostream& out, size_t indent=0) const
    {
        switch(op)
        {
            case BinaryOps::Addition: out << create_indent(indent) << "Add(" << *lhs << ", " << *rhs << ")"; break;
            case BinaryOps::Subtraction: out << create_indent(indent) << "Sub(" << *lhs << ", " << *rhs << ")"; break;
            case BinaryOps::Multiplication: out << create_indent(indent) << "Mul(" << *lhs << ", " << *rhs << ")"; break;
            case BinaryOps::Division: out << create_indent(indent) << "Div(" << *lhs << ", " << *rhs << ")"; break;
            case BinaryOps::Exponentiation: out << create_indent(indent) << "Pow(" << *lhs << ", " << *rhs << ")"; break;
            case BinaryOps::And: out << create_indent(indent) << "And(" << *lhs << ", " << *rhs << ")"; break;
            case BinaryOps::Or: out << create_indent(indent) << "Or(" << *lhs << ", " << *rhs << ")"; break;
            case BinaryOps::BitXor: out << create_indent(indent) << "BitXor(" << *lhs << ", " << *rhs << ")"; break;
            case BinaryOps::BitAnd: out << create_indent(indent) << "BitAnd(" << *lhs << ", " << *rhs << ")"; break;
            case BinaryOps::BitOr: out << create_indent(indent) << "BitOr(" << *lhs << ", " << *rhs << ")"; break;
            case BinaryOps::Greater: out << create_indent(indent) << "GT(" << *lhs << ", " << *rhs << ")"; break;
            case BinaryOps::GreaterEqual: out << create_indent(indent) << "GTE(" << *lhs << ", " << *rhs << ")"; break;
            case BinaryOps::Less: out << create_indent(indent) << "LT(" << *lhs << ", " << *rhs << ")"; break;
            case BinaryOps::LessEqual: out << create_indent(indent) << "LTE(" << *lhs << ", " << *rhs << ")"; break;
            case BinaryOps::Equal: out << create_indent(indent) << "EQ(" << *lhs << ", " << *rhs << ")"; break;
            case BinaryOps::NotEqual: out << create_indent(indent) << "NEQ(" << *lhs << ", " << *rhs << ")"; break;
        }

        return out;
    }

public:
    BinaryOp(
        BinaryOps op,
        std::unique_ptr<Expr> lhs,
        std::unique_ptr<Expr> rhs
    ) : 
        op(op),
        lhs(std::move(lhs)),
        rhs(std::move(rhs)) {}
};


// --- UNARY OPERATORS --- //
enum class UnaryOps
{
    None = -1,
    Addition, Subtraction, Not, BitwiseNot
};

class UnaryOp : public Expr
{
    UnaryOps op;
    std::unique_ptr<Expr> rhs;

    std::ostream& print(std::ostream& out, size_t indent=0) const
    {
        switch(op)
        {
            case UnaryOps::Addition: out << create_indent(indent) << "Plus(" << *rhs << ")"; break;
            case UnaryOps::Subtraction: out << create_indent(indent) << "Minus(" << *rhs << ")"; break;
            case UnaryOps::Not: out << create_indent(indent) << "Not(" << *rhs << ")"; break;
            case UnaryOps::BitwiseNot: out << create_indent(indent) << "BitNot(" << *rhs << ")"; break;
        }

        return out;
    }

public:
    UnaryOp(
        UnaryOps op,
        std::unique_ptr<Expr> rhs
    ) :
        op(op), rhs(std::move(rhs)) {}
};



class Statement : public ASTNode
{
public:
    virtual ~Statement() = default;
};


// --- ASSIGNMENT --- //
class Assignment : public Statement
{
    std::unique_ptr<Variable> lhs;
    std::unique_ptr<Expr> rhs;

    std::ostream& print(std::ostream& out, size_t indent=0) const
    {
        out << create_indent(indent) << "Assignment(" << *lhs << ", " << *rhs << ")" << std::endl;
        return out;
    }

public:
    Assignment(
        std::unique_ptr<Variable> lhs,
        std::unique_ptr<Expr> rhs
    ) : 
        lhs(std::move(lhs)), rhs(std::move(rhs)) {}
};


class IfStatement : public Statement
{
    std::unique_ptr<Expr> cond;
    std::vector<std::unique_ptr<Statement>> then_branch, else_branch;

    std::ostream& print(std::ostream& out, size_t indent=0) const
    {
        out << create_indent(indent) << "If(" << *cond << ")" << std::endl;

        for (auto& s : then_branch)
            s->print(out, indent + 1); 

        if (else_branch.size() >  0)
        {
            out << create_indent(indent) << "Else" << std::endl;
            
            for (auto& s : else_branch)
                s->print(out, indent + 1); 
        }

        return out;
    }

public:
    IfStatement(
        std::unique_ptr<Expr> cond,
        std::vector<std::unique_ptr<Statement>> then_branch,
        std::vector<std::unique_ptr<Statement>> else_branch
    ) :
        cond(std::move(cond)),
        then_branch(std::move(then_branch)),
        else_branch(std::move(else_branch)) {}

};


class WhileStatement : public Statement
{
    std::unique_ptr<Expr> cond;
    std::vector<std::unique_ptr<Statement>> body;

    std::ostream& print(std::ostream& out, size_t indent=0) const
    {
        out << create_indent(indent) << "While(" << *cond << ")" << std::endl;

        for (auto& s : body)
            s->print(out, indent + 1); 

        return out;
    }

public:
WhileStatement(
        std::unique_ptr<Expr> cond,
        std::vector<std::unique_ptr<Statement>> body
    ) :
        cond(std::move(cond)),
        body(std::move(body)) {}

};


#endif
