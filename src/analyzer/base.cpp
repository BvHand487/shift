#include <format>

#include "analyzer/base.h"

void AnalyzerVisitor::visit(Parameter &node)
{
    if (node.init != nullptr)
    {
        node.init->accept(*this);

        if (node.type != Type::Unknown)
        {
            if (node.type != node.init->type)
                throw std::runtime_error("Type mismatch when initializing a parameter");
        }
        else
            node.type = node.init->type;
    }
}

// Declaration Nodes
void AnalyzerVisitor::visit(Prototype &node)
{
    std::vector<ParamSymbol> args;
    bool seenInit = false;

    for (auto &arg : node.args)
    {
        arg->accept(*this);

        if (arg->init != nullptr)
        {
            seenInit = true;
        }
        else if (seenInit)
            throw std::runtime_error(std::format("Non-default parameter '{}' cannot follow a parameter with a default value", arg->name));

        ParamSymbol paramSymbol;
        paramSymbol.name = arg->name;
        paramSymbol.type = arg->type;
        paramSymbol.hasInit = arg->init != nullptr;

        args.push_back(paramSymbol);
    }

    FuncSymbol funcSymbol;
    funcSymbol.name = node.name;
    funcSymbol.retType = node.retType;
    funcSymbol.args = std::move(args);
    funcSymbol.isExtern = node.isExtern;
    funcSymbol.isVarArg = node.isVarArg;
    funcSymbol.isDefined = false;

    symbols->addFunction(funcSymbol);
}

void AnalyzerVisitor::visit(Definition &node)
{
    node.type->accept(*this);

    FuncSymbol *funcSymPtr = symbols->lookupFunction(node.type->name);
    currentFuncReturnType = funcSymPtr->retType;
    
    // missing return statement at the end of a function
    if (dynamic_cast<Return*>(node.body->statements.back().get()) == nullptr)
    {
        if (currentFuncReturnType == Type::Void)
        {
            auto emptyReturnStmnt = std::make_unique<Return>(nullptr);
            node.body->statements.push_back(std::move(emptyReturnStmnt));
        }
        // missing return statement from a typed function
        else
        {
            // insert return 0 in the main function only
            if (node.type->name == "main")
            {
                auto returnStmnt = std::make_unique<Return>(std::make_unique<Number>(0));
                node.body->statements.push_back(std::move(returnStmnt));
            }
            else
                throw std::runtime_error("Missing return statement in a non-void function");
        }
    }
    
    symbols->enterScope();

    for (const auto &arg : funcSymPtr->args)
    {
        VarSymbol varSymbol;
        varSymbol.type = arg.type;
        varSymbol.name = arg.name;
        varSymbol.llvmValue = nullptr;
        varSymbol.isMutable = true;

        symbols->addVariable(varSymbol);
    }

    node.body->accept(*this);
    symbols->exitScope();

    funcSymPtr->isDefined = true;
}

// Statement Nodes
void AnalyzerVisitor::visit(VariableDecl &node)
{
    // no initializer and no type annotation
    if (node.init == nullptr && node.type == Type::Unknown)
        throw std::runtime_error("Missing type annotation in variable declaration");
    
    // has initializer
    if (node.init != nullptr)
    {
        node.init->accept(*this);

        // infer type if no annotation
        if (node.type == Type::Unknown)
            node.type = node.init->type;
    
        // check conflicting types if annotation is present
        if (node.type != node.init->type)
            throw std::runtime_error("Type mismatch when declaring a variable");
    }

    VarSymbol varSymbol;
    varSymbol.name = node.name;
    varSymbol.type = node.type;
    varSymbol.isMutable = true;
    varSymbol.llvmValue = nullptr;

    symbols->addVariable(varSymbol);
}

void AnalyzerVisitor::visit(Assignment &node)
{
    node.lhs->accept(*this);
    node.rhs->accept(*this);

    if (node.lhs->type != node.rhs->type)
        throw std::runtime_error("Type mismatch when assigning a variable");
}

void AnalyzerVisitor::visit(Block &node)
{
    for (auto &stmnt : node.statements)
        stmnt->accept(*this);
}

void AnalyzerVisitor::visit(If &node)
{
    node.cond->accept(*this);

    if (node.cond->type == Type::String)
        throw std::runtime_error("If condition must be int or bool");

    symbols->enterScope();
    node.then_branch->accept(*this);
    symbols->exitScope();

    if (node.else_branch != nullptr)
    {
        symbols->enterScope();
        node.else_branch->accept(*this);
        symbols->exitScope();
    }
}

void AnalyzerVisitor::visit(While &node)
{
    node.cond->accept(*this);

    if (node.cond->type == Type::String)
        throw std::runtime_error("If condition must be int or bool");

    symbols->enterScope();
    node.body->accept(*this);
    symbols->exitScope();
}

void AnalyzerVisitor::visit(Return &node)
{
    // void return from void function
    if(currentFuncReturnType == Type::Void && node.value == nullptr)
        return;

    // attempted return from void function
    if (currentFuncReturnType == Type::Void && node.value != nullptr)
        throw std::runtime_error("Tried to return a value from a void function");
    
    // void return from non-void function
    if (currentFuncReturnType != Type::Void && node.value == nullptr)
        throw std::runtime_error("No return value from a non-void function");

    // value return from typed function
    node.value->accept(*this);
    if (currentFuncReturnType != node.value->type)
        throw std::runtime_error("Return type mismatch");
}

void AnalyzerVisitor::visit(ExprStatement &node)
{
    node.expression->accept(*this);
}

// Expression Nodes
void AnalyzerVisitor::visit(Variable &node)
{
    const VarSymbol *varSymPtr = symbols->lookupVariable(node.name);

    if (varSymPtr == nullptr)
        throw std::runtime_error("Referenced variable is undeclared");

    node.type = varSymPtr->type;
}

void AnalyzerVisitor::visit(CallExpr &node)
{
    const FuncSymbol *funcSymPtr = symbols->lookupFunction(node.callee);

    if (funcSymPtr == nullptr)
        throw std::runtime_error("Referenced function is undefined");

    size_t minRequiredArgs = 0;
    for (auto &arg : funcSymPtr->args)
        if (!arg.hasInit)
            minRequiredArgs++;

    if (node.args.size() < minRequiredArgs)
    {
        throw std::runtime_error(std::format("Too few arguments in call to '{}'", node.callee));
    }

    if (!funcSymPtr->isVarArg && node.args.size() > funcSymPtr->args.size())
    {
        throw std::runtime_error(std::format("Too many arguments in call to '{}'", node.callee));
    }

    // determine type of arguments in callexpr
    for (size_t i = 0; i < node.args.size(); ++i)
    {
        node.args[i]->accept(*this);
    }

    for (size_t i = 0; i < std::min(node.args.size(), funcSymPtr->args.size()); ++i)
    {
        const auto &param = funcSymPtr->args[i];
        if (param.type != node.args[i]->type)
            throw std::runtime_error(std::format("Type mismatch for parameter '{}' in call to '{}'", param.name, node.callee));
    }

    for (size_t i = node.args.size(); i < funcSymPtr->args.size(); ++i)
    {
        const auto &arg = funcSymPtr->args[i];

        if (!arg.hasInit)
            throw std::runtime_error(std::format("Missing argument '{}'", arg.name));
    }

    node.type = funcSymPtr->retType;
}

void AnalyzerVisitor::visit(BinaryOp &node)
{
    node.lhs->accept(*this);
    node.rhs->accept(*this);

    Type lt = node.lhs->type;
    Type rt = node.rhs->type;

    if (lt != rt)
    {
        throw std::runtime_error(std::format("Type mismatch in binary operation: {} vs {}", type_to_str.at(lt), type_to_str.at(rt)));
    }

    switch (node.op)
    {
    case binop_add:
    case binop_sub:
    case binop_mul:
    case binop_div:
    case binop_mod:
        if (lt == Type::String)
        {
            throw std::runtime_error("Arithmetic operators require numeric operands");
        }

        node.type = lt;
        break;

    case binop_and:
    case binop_or:
        if (lt != Type::Bool)
        {
            throw std::runtime_error("Logical operators require boolean operands");
        }

        node.type = Type::Bool;
        break;

    case binop_bit_and:
    case binop_bit_or:
    case binop_bit_xor:
        if (lt == Type::String)
        {
            throw std::runtime_error("Bitwise operators require numeric operands");
        }

        node.type = Type::Int;
        break;

    case binop_eq:
    case binop_neq:
    case binop_lt:
    case binop_lte:
    case binop_gt:
    case binop_gte:
        if (lt != Type::Int && lt != Type::Bool)
        {
            throw std::runtime_error("Comparison operators require comparable operands");
        }

        node.type = Type::Bool;
        break;

    default:
        throw std::runtime_error("Unknown binary operator");
    }
}

void AnalyzerVisitor::visit(UnaryOp &node)
{
    node.rhs->accept(*this);
    Type operandType = node.rhs->type;

    switch (node.op)
    {
    case unary_sub:
        if (operandType == Type::String)
            throw std::runtime_error("Unary '-' requires an int or bool operand");

        node.type = Type::Int;
        break;

    case unary_not:
        if (operandType == Type::String)
            throw std::runtime_error("Unary '!' requires an int or bool operand");

        node.type = Type::Bool;
        break;

    case unary_bit_not:
        if (operandType == Type::String)
            throw std::runtime_error("Unary '~' requires int operand");

        node.type = Type::Int;
        break;

    default:
        throw std::runtime_error("Unknown unary operator");
    }
}

// Literal Nodes
void AnalyzerVisitor::visit(Number &node)
{
    node.type = Type::Int;
}

void AnalyzerVisitor::visit(String &node)
{
    node.type = Type::String;
}

void AnalyzerVisitor::visit(Boolean &node)
{
    node.type = Type::Bool;
}
