#ifndef AST_H
#define AST_H


#include <string>
#include <memory>
#include <vector>
#include <ostream>


class ASTNode
{
    virtual std::ostream& print(std::ostream& out) const = 0;

public:
    virtual ~ASTNode() = default;

    friend std::ostream& operator<< (std::ostream& out, const ASTNode& obj)
    {
        return obj.print(out);
    }
};


class Expr : public ASTNode
{
    virtual std::ostream& print(std::ostream& out) const = 0;

public:
    virtual ~Expr() = default;
};

// --- VARIABLES --- //
class Variable : public Expr
{
    std::string name;

    std::ostream& print(std::ostream& out) const
    {
        out << "Variable(" << this->name << ")";
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
    std::ostream& print(std::ostream& out) const
    {
        out << "Number(" << this->value << ")";
        return out;
    }

public:
    using Literal::Literal;
};

class Boolean : public Literal<bool> {
private:
    std::ostream& print(std::ostream& out) const
    {
        out << "Boolean(" << std::boolalpha << this->value << ")";
        return out;
    }
public:
    using Literal::Literal;
};

class String : public Literal<std::string> {
private:
    std::ostream& print(std::ostream& out) const
    {
        out << "String(" << this->value << ")";
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

    std::ostream& print(std::ostream& out) const
    {
        switch(op)
        {
            case BinaryOps::Addition: out << "Add(" << *lhs << ", " << *rhs << ")"; break;
            case BinaryOps::Subtraction: out << "Sub(" << *lhs << ", " << *rhs << ")"; break;
            case BinaryOps::Multiplication: out << "Mul(" << *lhs << ", " << *rhs << ")"; break;
            case BinaryOps::Division: out << "Div(" << *lhs << ", " << *rhs << ")"; break;
            case BinaryOps::Exponentiation: out << "Pow(" << *lhs << ", " << *rhs << ")"; break;
            case BinaryOps::And: out << "And(" << *lhs << ", " << *rhs << ")"; break;
            case BinaryOps::Or: out << "Or(" << *lhs << ", " << *rhs << ")"; break;
            case BinaryOps::BitXor: out << "BitXor(" << *lhs << ", " << *rhs << ")"; break;
            case BinaryOps::BitAnd: out << "BitAnd(" << *lhs << ", " << *rhs << ")"; break;
            case BinaryOps::BitOr: out << "BitOr(" << *lhs << ", " << *rhs << ")"; break;
            case BinaryOps::Greater: out << "GT(" << *lhs << ", " << *rhs << ")"; break;
            case BinaryOps::GreaterEqual: out << "GTE(" << *lhs << ", " << *rhs << ")"; break;
            case BinaryOps::Less: out << "LT(" << *lhs << ", " << *rhs << ")"; break;
            case BinaryOps::LessEqual: out << "LTE(" << *lhs << ", " << *rhs << ")"; break;
            case BinaryOps::Equal: out << "EQ(" << *lhs << ", " << *rhs << ")"; break;
            case BinaryOps::NotEqual: out << "NEQ(" << *lhs << ", " << *rhs << ")"; break;
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

    std::ostream& print(std::ostream& out) const
    {
        switch(op)
        {
            case UnaryOps::Addition: out << "Plus(" << *rhs << ")"; break;
            case UnaryOps::Subtraction: out << "Minus(" << *rhs << ")"; break;
            case UnaryOps::Not: out << "Not(" << *rhs << ")"; break;
            case UnaryOps::BitwiseNot: out << "BitNot(" << *rhs << ")"; break;
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
    virtual std::ostream& print(std::ostream& out) const = 0;

public:
    virtual ~Statement() = default;
};


// --- ASSIGNMENT --- //
class Assignment : public Statement
{
    std::unique_ptr<Variable> lhs;
    std::unique_ptr<Expr> rhs;

    std::ostream& print(std::ostream& out) const
    {
        out << "Assignment(" << *lhs << ", " << *rhs << ")";
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
    std::unique_ptr<Statement> then_branch, else_branch;

public:
    IfStatement(
        std::unique_ptr<Expr> cond,
        std::unique_ptr<Statement> then_branch,
        std::unique_ptr<Statement> else_branch
    ) :
        cond(std::move(cond)),
        then_branch(std::move(then_branch)),
        else_branch(std::move(else_branch)) {}

};


class WhileStatement : public Statement
{
    std::unique_ptr<Expr> cond;
    std::unique_ptr<Statement> body;

public:
WhileStatement(
        std::unique_ptr<Expr> cond,
        std::unique_ptr<Statement> body
    ) :
        cond(std::move(cond)),
        body(std::move(body)) {}

};

#endif
