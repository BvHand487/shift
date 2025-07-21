#ifndef PARSER_PRINTER_H
#define PARSER_PRINTER_H

#include <iostream>

#include "ast.h"

using namespace ast;

class PrintVisitor : public Visitor
{
private:
    std::ostream &out;
    std::string prefix;
    std::vector<bool> indent_stack;

    void print_prefix(bool is_last);
    void push_indent(bool is_last);
    void pop_indent();

public:
    PrintVisitor(std::ostream &out = std::cout) : out(out) {}

    // Declaration Nodes
    void visit(Prototype &node) override;
    void visit(Definition &node) override;

    // Statement Nodes
    void visit(Block &node) override;
    void visit(If &node) override;
    void visit(While &node) override;
    void visit(Assignment &node) override;
    void visit(Return &node) override;
    void visit(ExprStatement &node) override;

    // Expression Nodes
    void visit(Variable &node) override;
    void visit(CallExpr &node) override;
    void visit(BinaryOp &node) override;
    void visit(UnaryOp &node) override;

    // Literal Nodes
    void visit(Number &node) override;
    void visit(String &node) override;
    void visit(Boolean &node) override;
};

#endif