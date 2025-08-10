#ifndef ANALYZER_BASE_H
#define ANALYZER_BASE_H

#include <vector>
#include <string>
#include <memory>
#include <map>

#include "analyzer/symbols.h"
#include "ast.h"

using namespace ast;


class AnalyzerVisitor : public Visitor
{
    std::unique_ptr<SymbolTable> symbols = std::make_unique<SymbolTable>();
    Type currentFuncReturnType = Type::Int;

public:
    AnalyzerVisitor() {}

    void visit(Parameter &node) override;

    // Declaration Nodes
    void visit(Prototype &node) override;
    void visit(Definition &node) override;

    // Statement Nodes
    void visit(VariableDecl &node) override;
    void visit(Assignment &node) override;
    void visit(Block &node) override;
    void visit(If &node) override;
    void visit(While &node) override;
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