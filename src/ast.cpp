#include <iostream>
#include <map>

#include "llvm/IR/Constants.h"
#include "llvm/ADT/APFloat.h"

#include "ast.h"

using llvm::BasicBlock;
using llvm::Function;
using llvm::IRBuilder;
using llvm::LLVMContext;
using llvm::Module;

extern std::unique_ptr<LLVMContext> context;
extern std::unique_ptr<Module> module;
extern std::unique_ptr<IRBuilder<>> builder;

static std::map<std::string, llvm::AllocaInst *> namedValues;

std::ostream &Variable::print(std::ostream &out, size_t indent = 0) const
{
    out << create_indent(indent) << "Variable(" << this->name << ")";
    return out;
}

llvm::Value *Variable::codegen()
{
    llvm::AllocaInst *allocated = namedValues[this->name];

    if (!allocated)
        return nullptr;

    return builder->CreateLoad(allocated->getAllocatedType(), allocated, this->name.c_str());
}

std::ostream &Number::print(std::ostream &out, size_t indent = 0) const
{
    out << create_indent(indent) << "Number(" << this->value << ")";
    return out;
}

llvm::Value *Number::codegen()
{
    return llvm::ConstantFP::get(*context, llvm::APFloat(this->value));
}

std::ostream &Boolean::print(std::ostream &out, size_t indent = 0) const
{
    out << create_indent(indent) << "Boolean(" << std::boolalpha << this->value << ")";
    return out;
}

llvm::Value *Boolean::codegen()
{
    return llvm::ConstantInt::getBool(*context, this->value);
}

std::ostream &String::print(std::ostream &out, size_t indent = 0) const
{
    out << create_indent(indent) << "String(" << this->value << ")";
    return out;
}

llvm::Value *String::codegen()
{
    return nullptr; // TODO
}

std::ostream &BinaryOp::print(std::ostream &out, size_t indent = 0) const
{
    switch (op)
    {
    case BinaryOps::Addition:
        out << create_indent(indent) << "Add(" << *lhs << ", " << *rhs << ")";
        break;
    case BinaryOps::Subtraction:
        out << create_indent(indent) << "Sub(" << *lhs << ", " << *rhs << ")";
        break;
    case BinaryOps::Multiplication:
        out << create_indent(indent) << "Mul(" << *lhs << ", " << *rhs << ")";
        break;
    case BinaryOps::Division:
        out << create_indent(indent) << "Div(" << *lhs << ", " << *rhs << ")";
        break;
    case BinaryOps::Exponentiation:
        out << create_indent(indent) << "Pow(" << *lhs << ", " << *rhs << ")";
        break;
    case BinaryOps::And:
        out << create_indent(indent) << "And(" << *lhs << ", " << *rhs << ")";
        break;
    case BinaryOps::Or:
        out << create_indent(indent) << "Or(" << *lhs << ", " << *rhs << ")";
        break;
    case BinaryOps::BitXor:
        out << create_indent(indent) << "BitXor(" << *lhs << ", " << *rhs << ")";
        break;
    case BinaryOps::BitAnd:
        out << create_indent(indent) << "BitAnd(" << *lhs << ", " << *rhs << ")";
        break;
    case BinaryOps::BitOr:
        out << create_indent(indent) << "BitOr(" << *lhs << ", " << *rhs << ")";
        break;
    case BinaryOps::Greater:
        out << create_indent(indent) << "GT(" << *lhs << ", " << *rhs << ")";
        break;
    case BinaryOps::GreaterEqual:
        out << create_indent(indent) << "GTE(" << *lhs << ", " << *rhs << ")";
        break;
    case BinaryOps::Less:
        out << create_indent(indent) << "LT(" << *lhs << ", " << *rhs << ")";
        break;
    case BinaryOps::LessEqual:
        out << create_indent(indent) << "LTE(" << *lhs << ", " << *rhs << ")";
        break;
    case BinaryOps::Equal:
        out << create_indent(indent) << "EQ(" << *lhs << ", " << *rhs << ")";
        break;
    case BinaryOps::NotEqual:
        out << create_indent(indent) << "NEQ(" << *lhs << ", " << *rhs << ")";
        break;
    }

    return out;
}

llvm::Value *BinaryOp::codegen()
{
    llvm::Value *l = this->lhs->codegen();
    llvm::Value *r = this->rhs->codegen();

    if (!l || !r)
        return nullptr;

    switch (this->op)
    {
    case BinaryOps::Addition:
        return builder->CreateFAdd(l, r, "addtmp");
    case BinaryOps::Subtraction:
        return builder->CreateFSub(l, r, "subtmp");
    case BinaryOps::Multiplication:
        return builder->CreateFMul(l, r, "multmp");
    case BinaryOps::Division:
        return builder->CreateFDiv(l, r, "divtmp");
    case BinaryOps::Exponentiation:
        // return builder->CreateF(l, r, "addtmp");
        break;
    case BinaryOps::And:
        return builder->CreateLogicalAnd(l, r, "andtmp");
    case BinaryOps::Or:
        return builder->CreateLogicalOr(l, r, "ortmp");
    case BinaryOps::BitXor:
        return builder->CreateXor(l, r, "bxortmp");
    case BinaryOps::BitAnd:
        return builder->CreateAnd(l, r, "baddtmp");
    case BinaryOps::BitOr:
        return builder->CreateOr(l, r, "bortmp");
    case BinaryOps::Greater:
        return builder->CreateFCmpUGT(l, r, "gttmp");
    case BinaryOps::GreaterEqual:
        return builder->CreateFCmpUGE(l, r, "getmp");
    case BinaryOps::Less:
        return builder->CreateFCmpOLT(l, r, "lttmp");
    case BinaryOps::LessEqual:
        return builder->CreateFCmpOLE(l, r, "letmp");
    case BinaryOps::Equal:
        return builder->CreateFCmpOEQ(l, r, "eqtmp");
    case BinaryOps::NotEqual:
        return builder->CreateFCmpONE(l, r, "neqtmp");
    }

    return nullptr;
}

std::ostream &UnaryOp::print(std::ostream &out, size_t indent = 0) const
{
    switch (op)
    {
    case UnaryOps::Addition:
        out << create_indent(indent) << "Plus(" << *rhs << ")";
        break;
    case UnaryOps::Subtraction:
        out << create_indent(indent) << "Minus(" << *rhs << ")";
        break;
    case UnaryOps::Not:
        out << create_indent(indent) << "Not(" << *rhs << ")";
        break;
    case UnaryOps::BitwiseNot:
        out << create_indent(indent) << "BitNot(" << *rhs << ")";
        break;
    }

    return out;
}

llvm::Value *UnaryOp::codegen()
{
    llvm::Value *r = this->rhs->codegen();

    if (!r)
        return nullptr;

    switch (this->op)
    {
    case UnaryOps::Addition:
        return r;
    case UnaryOps::Subtraction:
        return builder->CreateFNeg(r, "negtmp");
    case UnaryOps::Not:
        return builder->CreateFCmpOEQ(r, llvm::ConstantInt::get(*context, llvm::APInt()), "nottmp"); // llvm::APInt() defaults to a value of 0
    case UnaryOps::BitwiseNot:
        return builder->CreateNot(r, "bnottmp");
    }

    return nullptr;
}

std::ostream &Assignment::print(std::ostream &out, size_t indent = 0) const
{
    out << create_indent(indent) << "Assignment(" << *lhs << ", " << *rhs << ")" << std::endl;
    return out;
}

llvm::Value *Assignment::codegen()
{
    llvm::Value *r = this->rhs->codegen();

    if (!r)
        return nullptr;

    llvm::Value *var = namedValues[this->lhs->getName()];

    // if the variable doesnt exist yet - declare it.
    if (!var)
    {
        llvm::AllocaInst* alloca = builder->CreateAlloca(llvm::Type::getDoubleTy(*context), nullptr, this->lhs->getName());
        var = namedValues[this->lhs->getName()] = alloca;
    }

    builder->CreateStore(r, var);

    return r;
}

std::ostream &IfStatement::print(std::ostream &out, size_t indent = 0) const
{
    out << create_indent(indent) << "If(" << *cond << ")" << std::endl;

    for (auto &s : then_branch)
        s->print(out, indent + 1);

    if (else_branch.size() > 0)
    {
        out << create_indent(indent) << "Else" << std::endl;

        for (auto &s : else_branch)
            s->print(out, indent + 1);
    }

    return out;
}

llvm::Value *IfStatement::codegen()
{
    llvm::Value *cond = this->cond->codegen();

    if (!cond)
        return nullptr;

    cond = builder->CreateFCmpONE(cond, llvm::ConstantFP::get(*context, llvm::APFloat(0.0)), "condtmp");

    Function *func = builder->GetInsertBlock()->getParent();

    BasicBlock *thenBB = BasicBlock::Create(*context, "then", func);
    BasicBlock *elseBB = BasicBlock::Create(*context, "else");
    BasicBlock *mergedBB = BasicBlock::Create(*context, "merged");

    builder->CreateCondBr(cond, thenBB, elseBB);

    // - - - THEN BLOCK - - - //
    builder->SetInsertPoint(thenBB);
    llvm::Value *thenVal = nullptr;
    for (auto &statement : this->then_branch)
    {
        thenVal = statement->codegen();
        if (!thenVal)
            return nullptr;
    }
    builder->CreateBr(mergedBB);
    thenBB = builder->GetInsertBlock();

    // - - - ELSE BLOCK - - - //
    func->insert(func->end(), elseBB);

    builder->SetInsertPoint(elseBB);
    llvm::Value *elseVal = nullptr;
    for (auto &statement : this->else_branch)
    {
        elseVal = statement->codegen();
        if (!elseVal)
            return nullptr;
    }
    builder->CreateBr(mergedBB);
    elseBB = builder->GetInsertBlock();

    // - - - MERGED BLOCK - - - //
    func->insert(func->end(), mergedBB);
    builder->SetInsertPoint(mergedBB);

    llvm::PHINode *phi = builder->CreatePHI(llvm::Type::getDoubleTy(*context), 2, "iftmp");

    phi->addIncoming(thenVal, thenBB);
    phi->addIncoming(elseVal, elseBB);

    return phi;
}

std::ostream &WhileStatement::print(std::ostream &out, size_t indent = 0) const
{
    out << create_indent(indent) << "While(" << *cond << ")" << std::endl;

    for (auto &s : body)
        s->print(out, indent + 1);

    return out;
}

llvm::Value *WhileStatement::codegen()
{
    return nullptr;
}