#ifndef AST_VISITOR_H
#define AST_VISITOR_H

#include "ast.h"

namespace ast
{
    class Visitor
    {
    public:
        // Literals
        virtual void visit(Number &node) = 0;
        virtual void visit(String &node) = 0;
        virtual void visit(Boolean &node) = 0;

        // Statements
        virtual void visit(Block &node) = 0;
        virtual void visit(If &node) = 0;
        virtual void visit(While &node) = 0;
        virtual void visit(Assignment &node) = 0;
        virtual void visit(Return &node) = 0;
        virtual void visit(ExprStatement &node) = 0;

        // Expressions
        virtual void visit(Variable &node) = 0;
        virtual void visit(CallExpr &node) = 0;
        virtual void visit(BinaryOp &node) = 0;
        virtual void visit(UnaryOp &node) = 0;

        // Declarations
        virtual void visit(Prototype &node) = 0;
        virtual void visit(Definition &node) = 0;
    };
}

#endif