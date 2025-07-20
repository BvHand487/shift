#include <iostream>
#include <map>

#include "llvm/IR/Constants.h"
#include "llvm/IR/Verifier.h"
#include "llvm/ADT/APFloat.h"

#include "ast.h"

using llvm::BasicBlock;
using llvm::IRBuilder;
using llvm::LLVMContext;
using llvm::Module;

extern std::unique_ptr<LLVMContext> context;
extern std::unique_ptr<Module> module;
extern std::unique_ptr<IRBuilder<>> builder;

static std::map<std::string, llvm::AllocaInst *> namedValues;

extern std::unique_ptr<llvm::FunctionPassManager> theFPM;
extern std::unique_ptr<llvm::LoopAnalysisManager> theLAM;
extern std::unique_ptr<llvm::FunctionAnalysisManager> theFAM;
extern std::unique_ptr<llvm::CGSCCAnalysisManager> theCGAM;
extern std::unique_ptr<llvm::ModuleAnalysisManager> theMAM;
extern std::unique_ptr<llvm::PassInstrumentationCallbacks> thePIC;
extern std::unique_ptr<llvm::StandardInstrumentations> theSI;


std::ostream &Variable::print(std::ostream &out, size_t indent = 0) const
{
    return out << create_indent(indent) << "Variable(" << this->name << ")";
    ;
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
    return out << create_indent(indent) << "Number(" << this->value << ")";
}

llvm::Value *Number::codegen()
{
    return llvm::ConstantFP::get(*context, llvm::APFloat(this->value));
}

std::ostream &Boolean::print(std::ostream &out, size_t indent = 0) const
{
    return out << create_indent(indent) << "Boolean(" << std::boolalpha << this->value << ")";
}

llvm::Value *Boolean::codegen()
{
    return llvm::ConstantInt::getBool(*context, this->value);
}

std::ostream &String::print(std::ostream &out, size_t indent = 0) const
{
    return out << create_indent(indent) << "String(" << this->value << ")";
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
    return out << create_indent(indent) << "Assignment(" << *lhs << ", " << *rhs << ")" << std::endl;
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
        llvm::Function *function = builder->GetInsertBlock()->getParent();
        llvm::AllocaInst *alloca = builder->CreateAlloca(llvm::Type::getDoubleTy(*context), nullptr, this->lhs->getName());
        var = namedValues[this->lhs->getName()] = alloca;
    }

    builder->CreateStore(r, var);

    return r;
}

std::ostream &Function::print(std::ostream &out, size_t indent = 0) const
{
    out << create_indent(indent) << "fn " << this->name << "(";
    if (!this->args.empty())
        out << std::endl;
    
    for (auto &arg : this->args)
        arg->print(out, indent + 1) << '\n';

    out << create_indent(indent) << ")" << std::endl;

    for (auto &stmnt : this->body)
        stmnt->print(out, indent + 1);

    return out;
}

llvm::Value *Function::codegen()
{
    llvm::Type* type;
    std::vector<llvm::Type *> params;
    if (this->name == "main")
    {
        type = llvm::Type::getInt32Ty(*context);
    }
    else
    {
        type = llvm::Type::getDoubleTy(*context);
        params.insert(params.end(), this->args.size(), type);
    }
    
    llvm::FunctionType *functionType = llvm::FunctionType::get(type, params, false);
    llvm::Function *function = llvm::Function::Create(functionType, llvm::Function::ExternalLinkage, this->name, *module);
    
    size_t idx = 0;
    for (auto &arg : function->args())
        arg.setName(this->args[idx++]->getName());

    BasicBlock *block = BasicBlock::Create(*context, "entry", function);
    builder->SetInsertPoint(block);

    namedValues.clear();
    for (auto &arg : function->args())
    {
        llvm::AllocaInst *alloca = builder->CreateAlloca(llvm::Type::getDoubleTy(*context), nullptr, arg.getName());

        builder->CreateStore(&arg, alloca);

        namedValues[std::string(arg.getName())] = alloca;
    }

    llvm::Value *retVal = nullptr;
    for (auto &statement : this->body)
        retVal = statement->codegen();

    if (!llvm::verifyFunction(*function))
    {
        theFPM->run(*function, *theFAM);
        return function;
    }

    return nullptr;    
}

std::ostream &Return::print(std::ostream &out, size_t indent = 0) const
{
    return out << create_indent(indent) << "Return(" << *this->value << ")";
}

llvm::Value *Return::codegen()
{
    if (this->value == nullptr) {
        return builder->CreateRetVoid();
    }

    llvm::Value* retVal = this->value->codegen();
    if (!retVal)
        return nullptr;

    if (builder->GetInsertBlock()->getParent()->getName() == "main")
    {
        if (retVal->getType()->isDoubleTy())
        {
            retVal = builder->CreateFPToSI(retVal, llvm::Type::getInt32Ty(*context), "retcast");
        }
        // is bool
        else if (retVal->getType()->isIntegerTy(1))
        {
            retVal = builder->CreateZExt(retVal, llvm::Type::getInt32Ty(*context), "zextbool");
        }
        else
        {
            return nullptr;
        }
    }

    return builder->CreateRet(retVal);
}

std::ostream &Call::print(std::ostream &out, size_t indent = 0) const
{
    out << create_indent(indent) << this->callee << "(";
    if (!this->args.empty())
        out << std::endl;

    for (auto &arg : this->args)
        arg->print(out, indent + 1) << '\n';

    return out << create_indent(indent) << ")" << std::endl;
}

llvm::Value *Call::codegen()
{
    llvm::Function *callee = module->getFunction(this->callee);

    if (!callee)
        return nullptr;

    if (callee->arg_size() != this->args.size())
        return nullptr;

    std::vector<llvm::Value*> argValues;
    for (int i = 0; i < this->args.size(); ++i)
    {
        argValues.push_back(this->args[i]->codegen());

        if (!argValues.back())
            return nullptr;
    }

    return builder->CreateCall(callee, argValues, "calltmp");
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

    llvm::Function *func = builder->GetInsertBlock()->getParent();

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