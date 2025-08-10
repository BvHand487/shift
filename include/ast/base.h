#ifndef AST_BASE_H
#define AST_BASE_H

#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "operators.h"
#include "types.h"
#include "utils.h"

namespace ast
{
    class Visitor;

    class ASTNode
    {
    public:
        virtual void accept(Visitor &v) = 0;
        virtual ~ASTNode() = default;
    };

    class Expr : public ASTNode
    {
    public:
        Type type = Type::Unknown;
    };

    class Statement : public ASTNode
    {
    };

    class Declaration : public ASTNode
    {
    };

    class Parameter : public ASTNode
    {
    public:
        Type type;
        std::string name;
        std::unique_ptr<Expr> init;

        Parameter(
            const std::string& name,
            Type type = Type::Unknown,
            std::unique_ptr<Expr> init = nullptr);

        void accept(Visitor &v) override;
    };

    // - - - - - LITERALS - - - - - //
    template <class T>
    class Literal : public Expr
    {
    public:
        T value;
        explicit Literal(T value) : value(value) {}
    };

    class Number : public Literal<int>
    {
    public:
        Number(int value) : Literal<int>(value) {
            type = Type::Int;
        }

        void accept(Visitor &v) override;
    };

    class Boolean : public Literal<bool>
    {
    public:
        Boolean(bool value) : Literal<bool>(value) {
            type = Type::Bool;
        }

        void accept(Visitor &v) override;
    };

    class String : public Literal<std::string>
    {
    public:
        String(const std::string& value) : Literal<std::string>(value) {
            type = Type::String;
        }

        void accept(Visitor &v) override;
    };
    // - - - - - - - - - - - - - - - //

    // - - - - - EXPRESSIONS - - - - - //
    class Variable : public Expr
    {
    public:
        std::string name;

        Variable(const std::string &name);
        void accept(Visitor &v) override;
    };

    class CallExpr : public Expr
    {
    public:
        std::vector<std::unique_ptr<Expr>> args;
        std::string callee;

        CallExpr(
            const std::string &callee,
            std::vector<std::unique_ptr<Expr>> args);

        void accept(Visitor &v) override;
    };

    class BinaryOp : public Expr
    {
    public:
        BinaryOpType op;
        std::unique_ptr<Expr> lhs, rhs;

        BinaryOp(
            BinaryOpType op,
            std::unique_ptr<Expr> lhs,
            std::unique_ptr<Expr> rhs);

        void accept(Visitor &v) override;
    };

    class UnaryOp : public Expr
    {
    public:
        UnaryOpType op;
        std::unique_ptr<Expr> rhs;

        UnaryOp(
            UnaryOpType op,
            std::unique_ptr<Expr> rhs);

        void accept(Visitor &v) override;
    };
    // - - - - - - - - - - - - - - - - //

    // - - - - - STATEMENTS - - - - - //
    class VariableDecl : public Statement
    {
    public:
        std::string name;
        Type type;
        std::unique_ptr<Expr> init;

        VariableDecl(const std::string &name, Type type = Type::Unknown, std::unique_ptr<Expr> init = nullptr);

        void accept(Visitor &v) override;
    };

    class Assignment : public Statement
    {
    public:
        std::unique_ptr<Variable> lhs;
        std::unique_ptr<Expr> rhs;

        Assignment(
            std::unique_ptr<Variable> lhs,
            std::unique_ptr<Expr> rhs);

        void accept(Visitor &v) override;
    };
    
    class Block : public Statement
    {
    public:
        std::vector<std::unique_ptr<Statement>> statements;

        Block(std::vector<std::unique_ptr<Statement>> stmts);
        void accept(Visitor &v) override;
    };

    class If : public Statement
    {
    public:
        std::unique_ptr<Expr> cond;
        std::unique_ptr<Block> then_branch, else_branch;

        If(
            std::unique_ptr<Expr> cond,
            std::unique_ptr<Block> then_branch,
            std::unique_ptr<Block> else_branch);

        void accept(Visitor &v) override;
    };

    class While : public Statement
    {
    public:
        std::unique_ptr<Expr> cond;
        std::unique_ptr<Block> body;

        While(
            std::unique_ptr<Expr> cond,
            std::unique_ptr<Block> body);

        void accept(Visitor &v) override;
    };

    class Return : public Statement
    {
    public:
        std::unique_ptr<Expr> value;

        Return(std::unique_ptr<Expr> value);
        void accept(Visitor &v) override;
    };

    class ExprStatement : public Statement
    {
    public:
        std::unique_ptr<Expr> expression;

        ExprStatement(std::unique_ptr<Expr> expression);
        void accept(Visitor &v) override;
    };
    // - - - - - - - - - - - - - - - - //

    // - - - - - DECLARATIONS - - - - - //
    class Prototype : public Declaration
    {
    public:
        Type retType;
        std::vector<std::unique_ptr<Parameter>> args;
        std::string name;
        bool isExtern = false;
        bool isVarArg = false;

        Prototype(
            Type retType,
            const std::string &name,
            std::vector<std::unique_ptr<Parameter>> args,
            bool isExtern = false,
            bool isVarArg = false
        );

        void accept(Visitor &v) override;
    };

    class Definition : public Declaration
    {
    public:
        std::unique_ptr<Prototype> type;
        std::unique_ptr<Block> body;

        Definition(
            std::unique_ptr<Prototype> type,
            std::unique_ptr<Block> body);

        void accept(Visitor &v) override;
    };
    // - - - - - - - - - - - - -  - - - //
}

#endif
