#include "ast.h"
#include "parser.h"

using namespace ast;

void PrintVisitor::print_prefix(bool is_last)
{
    for (size_t i = 0; i + 1 < indent_stack.size(); ++i)
        out << (indent_stack[i] ? "    " : "│   ");

    out << (indent_stack.empty() ? "" : (is_last ? "└── " : "├── "));
}

void PrintVisitor::push_indent(bool is_last)
{
    indent_stack.push_back(is_last);
}

void PrintVisitor::pop_indent()
{
    if (!indent_stack.empty())
        indent_stack.pop_back();
}

// Literals
void PrintVisitor::visit(Number &node)
{
    print_prefix(true);
    out << "Number(" << node.value << "): " << type_to_str.at(node.type) << "\n";
}

void PrintVisitor::visit(String &node)
{
    print_prefix(true);
    out << "String(" << to_escaped_string(node.value) << "): " << type_to_str.at(node.type) << "\n";
}

void PrintVisitor::visit(Boolean &node)
{
    print_prefix(true);
    out << "Boolean(" << std::boolalpha << node.value << "): " << type_to_str.at(node.type) << "\n";
}

// Statements
void PrintVisitor::visit(VariableDecl &node)
{
    print_prefix(false);
    
    out << "VariableDecl(" << node.name << "): " << type_to_str.at(node.type) << "\n";

    push_indent(true);
    if (node.init)
        node.init->accept(*this);
    pop_indent();
}

void PrintVisitor::visit(Block &node)
{
    print_prefix(true);
    out << "Block\n";
    for (size_t i = 0; i < node.statements.size(); ++i)
    {
        push_indent(i == node.statements.size() - 1);
        node.statements[i]->accept(*this);
        pop_indent();
    }
}

void PrintVisitor::visit(If &node)
{
    print_prefix(true);
    out << "If\n";
    push_indent(false);
    node.cond->accept(*this);
    pop_indent();

    push_indent(false);
    node.then_branch->accept(*this);
    pop_indent();

    if (node.else_branch)
    {
        push_indent(true);
        node.else_branch->accept(*this);
        pop_indent();
    }
}

void PrintVisitor::visit(While &node)
{
    print_prefix(true);
    out << "While\n";
    push_indent(false);
    node.cond->accept(*this);
    pop_indent();

    push_indent(true);
    node.body->accept(*this);
    pop_indent();
}

void PrintVisitor::visit(Return &node)
{
    print_prefix(true);
    out << "Return\n";

    push_indent(true);
    if (node.value)
        node.value->accept(*this);
    pop_indent();
}

void PrintVisitor::visit(ExprStatement &node)
{
    print_prefix(true);
    out << "ExprStatement\n";
    push_indent(true);
    node.expression->accept(*this);
    pop_indent();
}

// Expressions
void PrintVisitor::visit(Variable &node)
{
    print_prefix(true);
    out << "Variable(" << node.name << "): " << type_to_str.at(node.type) << "\n";
}

void PrintVisitor::visit(Assignment &node)
{
    print_prefix(true);
    out << "Assignment\n";
    push_indent(false);
    node.lhs->accept(*this);
    pop_indent();

    push_indent(true);
    node.rhs->accept(*this);
    pop_indent();
}

void PrintVisitor::visit(CallExpr &node)
{
    print_prefix(true);
    out << "CallExpr(" << node.callee << "): " << type_to_str.at(node.type) << "\n";
    for (size_t i = 0; i < node.args.size(); ++i)
    {
        push_indent(i == node.args.size() - 1);
        node.args[i]->accept(*this);
        pop_indent();
    }
}

void PrintVisitor::visit(BinaryOp &node)
{
    print_prefix(true);
    out << "BinaryOp(" << binop_to_str.at(node.op) << ")" << ": " << type_to_str.at(node.type) << "\n";
    push_indent(false);
    node.lhs->accept(*this);
    pop_indent();

    push_indent(true);
    node.rhs->accept(*this);
    pop_indent();
}

void PrintVisitor::visit(UnaryOp &node)
{
    print_prefix(true);
    out << "UnaryOp(" << unary_to_str.at(node.op) << ")" << ": " << type_to_str.at(node.type) << "\n";
    push_indent(true);
    node.rhs->accept(*this);
    pop_indent();
}

// Declarations
void PrintVisitor::visit(Prototype &node)
{
    print_prefix(true);
    
    if (node.isExtern)
        out << "ExternFn(" << node.name << "): " << type_to_str.at(node.retType) << "\n";
    else
        out << "Fn(" << node.name << "): " << type_to_str.at(node.retType) << "\n";

    for (size_t i = 0; i < node.args.size(); ++i)
    {
        push_indent(i == node.args.size() - 1);
        node.args[i]->accept(*this);
        pop_indent();
    }
}

void PrintVisitor::visit(Definition &node)
{
    node.type->accept(*this);
    push_indent(true);
    node.body->accept(*this);
    pop_indent();
}

void PrintVisitor::visit(Parameter &node)
{
    print_prefix(false);
    
    out << "Arg(" << node.name << "): " << type_to_str.at(node.type) << "\n";

    if (node.init)
    {
        push_indent(true);
        node.init->accept(*this);
        pop_indent();
    }
}