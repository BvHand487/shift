#include <iostream>
#include <map>

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Scalar/Reassociate.h"
#include "llvm/Transforms/Scalar/SimplifyCFG.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/StandardInstrumentations.h"

#include "ast/ast.h"
#include "operators.h"

using namespace ast;
using namespace ast::control;
using namespace ast::literals;
using namespace ast::functions;


extern std::unique_ptr<llvm::LLVMContext> context;
extern std::unique_ptr<llvm::Module> module;
extern std::unique_ptr<llvm::IRBuilder<>> builder;

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
    out << create_indent(indent) << binop_to_str.at(this->op) << "(" << std::endl;
    lhs->print(out, indent + 1);
    out << "," << std::endl;
    rhs->print(out, indent + 1);
    out << std::endl << create_indent(indent) << ")";
    return out;
}

llvm::Value *BinaryOp::codegen()
{
    // llvm::Value *l = this->lhs->codegen();
    // llvm::Value *r = this->rhs->codegen();

    // if (!l || !r)
    //     return nullptr;

    // switch (this->op)
    // {
    // case binop_add:
    //     return builder->CreateFAdd(l, r, "addtmp");
    // case binop_sub:
    //     return builder->CreateFSub(l, r, "subtmp");
    // case binop_mul:
    //     return builder->CreateFMul(l, r, "multmp");
    // case binop_div:
    //     return builder->CreateFDiv(l, r, "divtmp");
    // case binop_exp:
    //     // return builder->CreateF(l, r, "addtmp");
    //     break;
    // case binop_and:
    //     return builder->CreateLogicalAnd(l, r, "andtmp");
    // case binop_or:
    //     return builder->CreateLogicalOr(l, r, "ortmp");
    // case binop_bit_xor:
    //     return builder->CreateXor(l, r, "bxortmp");
    // case binop_bit_and:
    //     return builder->CreateAnd(l, r, "baddtmp");
    // case binop_bit_or:
    //     return builder->CreateOr(l, r, "bortmp");
    // case binop_gt:
    //     return builder->CreateFCmpUGT(l, r, "gttmp");
    // case binop_gte:
    //     return builder->CreateFCmpUGE(l, r, "getmp");
    // case binop_lt:
    //     return builder->CreateFCmpOLT(l, r, "lttmp");
    // case binop_lte:
    //     return builder->CreateFCmpOLE(l, r, "letmp");
    // case binop_eq:
    //     return builder->CreateFCmpOEQ(l, r, "eqtmp");
    // case binop_neq:
    //     return builder->CreateFCmpONE(l, r, "neqtmp");
    // }

    // return nullptr;

    return nullptr;
}

std::ostream &UnaryOp::print(std::ostream &out, size_t indent = 0) const
{
    return out << create_indent(indent) << unary_to_str.at(this->op) << "(" << *rhs << ")";
}

llvm::Value *UnaryOp::codegen()
{
    // llvm::Value *r = this->rhs->codegen();

    // if (!r)
    //     return nullptr;

    // switch (this->op)
    // {
    // case unary_add:
    //     return r;
    // case unary_sub:
    //     return builder->CreateFNeg(r, "negtmp");
    // case unary_not:
    //     return builder->CreateFCmpOEQ(r, llvm::ConstantInt::get(*context, llvm::APInt()), "nottmp"); // llvm::APInt() defaults to a value of 0
    // case unary_bit_not:
    //     return builder->CreateNot(r, "bnottmp");
    // }

    // return nullptr;

    return nullptr;
}

std::ostream &Assignment::print(std::ostream &out, size_t indent = 0) const
{
    return out << create_indent(indent) << "Assignment(" << *lhs << ", " << *rhs << ")" << std::endl;
}

llvm::Value *Assignment::codegen()
{
    // llvm::Value *r = this->rhs->codegen();

    // if (!r)
    //     return nullptr;

    // llvm::Value *var = namedValues[this->lhs->getName()];

    // // if the variable doesnt exist yet - declare it.
    // if (!var)
    // {
    //     llvm::Function *function = builder->GetInsertBlock()->getParent();
    //     llvm::AllocaInst *alloca = builder->CreateAlloca(llvm::Type::getDoubleTy(*context), nullptr, this->lhs->getName());
    //     var = namedValues[this->lhs->getName()] = alloca;
    // }

    // builder->CreateStore(r, var);

    // return r;

    return nullptr;
}


std::ostream &Prototype::print(std::ostream &out, size_t indent = 0) const
{
    out << create_indent(indent) << "fn " << this->name << "(";
    if (!this->args.empty())
        out << std::endl;
    
    for (auto &arg : this->args)
        arg->print(out, indent + 1) << '\n';

    out << create_indent(indent) << ")" << std::endl;

    return out;
}

llvm::Value *Prototype::codegen()
{
    return nullptr;
}

std::ostream &Definition::print(std::ostream &out, size_t indent = 0) const
{
    this->type->print(out, indent);

    this->body->print(out, indent + 1);

    return out;
}

llvm::Value *Definition::codegen()
{
    // llvm::Type* type;
    // std::vector<llvm::Type *> params;
    // if (this->name == "main")
    // {
    //     type = llvm::Type::getInt32Ty(*context);
    // }
    // else
    // {
    //     type = llvm::Type::getDoubleTy(*context);
    //     params.insert(params.end(), this->args.size(), type);
    // }
    
    // llvm::FunctionType *functionType = llvm::FunctionType::get(type, params, false);
    // llvm::Function *function = llvm::Function::Create(functionType, llvm::Function::ExternalLinkage, this->name, *module);
    
    // size_t idx = 0;
    // for (auto &arg : function->args())
    //     arg.setName(this->args[idx++]->getName());

    // BasicBlock *block = BasicBlock::Create(*context, "entry", function);
    // builder->SetInsertPoint(block);

    // namedValues.clear();
    // for (auto &arg : function->args())
    // {
    //     llvm::AllocaInst *alloca = builder->CreateAlloca(llvm::Type::getDoubleTy(*context), nullptr, arg.getName());

    //     builder->CreateStore(&arg, alloca);

    //     namedValues[std::string(arg.getName())] = alloca;
    // }

    // llvm::Value *retVal = nullptr;
    // for (auto &statement : this->body)
    //     retVal = statement->codegen();

    // if (!llvm::verifyFunction(*function))
    // {
    //     theFPM->run(*function, *theFAM);
    //     return function;
    // }

    // return nullptr;

    return nullptr;
}

std::ostream &Return::print(std::ostream &out, size_t indent = 0) const
{
    out << create_indent(indent) << "Return(" << std::endl;
    this->value->print(out, indent + 1);
    out << std::endl << create_indent(indent) << ")";
    return out;
}

llvm::Value *Return::codegen()
{
    // if (this->value == nullptr) {
    //     return builder->CreateRetVoid();
    // }

    // llvm::Value* retVal = this->value->codegen();
    // if (!retVal)
    //     return nullptr;

    // if (builder->GetInsertBlock()->getParent()->getName() == "main")
    // {
    //     if (retVal->getType()->isDoubleTy())
    //     {
    //         retVal = builder->CreateFPToSI(retVal, llvm::Type::getInt32Ty(*context), "retcast");
    //     }
    //     // is bool
    //     else if (retVal->getType()->isIntegerTy(1))
    //     {
    //         retVal = builder->CreateZExt(retVal, llvm::Type::getInt32Ty(*context), "zextbool");
    //     }
    //     else
    //     {
    //         return nullptr;
    //     }
    // }

    // return builder->CreateRet(retVal);

    return nullptr;
}

std::ostream &CallExpr::print(std::ostream &out, size_t indent = 0) const
{
    out << create_indent(indent) << this->callee << "(";
    if (!this->args.empty())
        out << std::endl;

    for (auto &arg : this->args)
        arg->print(out, indent + 1) << '\n';

    return out << create_indent(indent) << ")" << std::endl;
}

llvm::Value *CallExpr::codegen()
{
    // llvm::Function *callee = module->getFunction(this->callee);

    // if (!callee)
    //     return nullptr;

    // if (callee->arg_size() != this->args.size())
    //     return nullptr;

    // std::vector<llvm::Value*> argValues;
    // for (int i = 0; i < this->args.size(); ++i)
    // {
    //     argValues.push_back(this->args[i]->codegen());

    //     if (!argValues.back())
    //         return nullptr;
    // }

    // return builder->CreateCall(callee, argValues, "calltmp");

    return nullptr;
}

std::ostream &If::print(std::ostream &out, size_t indent = 0) const
{
    out << create_indent(indent) << "If(" << *cond << ")" << std::endl;
    this->then_branch->print(out, indent + 1);

    if (else_branch)
    {
        out << create_indent(indent) << "Else" << std::endl;
        this->else_branch->print(out, indent + 1);
    }

    return out;
}

llvm::Value *If::codegen()
{
    // llvm::Value *cond = this->cond->codegen();

    // if (!cond)
    //     return nullptr;

    // llvm::Function *func = builder->GetInsertBlock()->getParent();

    // BasicBlock *thenBB = BasicBlock::Create(*context, "then", func);
    // BasicBlock *elseBB = BasicBlock::Create(*context, "else");
    // BasicBlock *mergedBB = BasicBlock::Create(*context, "merged");

    // builder->CreateCondBr(cond, thenBB, elseBB);

    // // - - - THEN BLOCK - - - //
    // builder->SetInsertPoint(thenBB);
    // llvm::Value *thenVal = nullptr;
    // for (auto &statement : this->then_branch)
    // {
    //     thenVal = statement->codegen();
    //     if (!thenVal)
    //         return nullptr;
    // }
    // builder->CreateBr(mergedBB);
    // thenBB = builder->GetInsertBlock();

    // // - - - ELSE BLOCK - - - //
    // func->insert(func->end(), elseBB);

    // builder->SetInsertPoint(elseBB);
    // llvm::Value *elseVal = nullptr;
    // for (auto &statement : this->else_branch)
    // {
    //     elseVal = statement->codegen();
    //     if (!elseVal)
    //         return nullptr;
    // }
    // builder->CreateBr(mergedBB);
    // elseBB = builder->GetInsertBlock();

    // // - - - MERGED BLOCK - - - //
    // func->insert(func->end(), mergedBB);
    // builder->SetInsertPoint(mergedBB);

    // llvm::PHINode *phi = builder->CreatePHI(llvm::Type::getDoubleTy(*context), 2, "iftmp");

    // phi->addIncoming(thenVal, thenBB);
    // phi->addIncoming(elseVal, elseBB);

    // return phi;

    return nullptr;
}

std::ostream &While::print(std::ostream &out, size_t indent = 0) const
{
    out << create_indent(indent) << "While(" << *cond << ")" << std::endl;

    this->body->print(out, indent + 1);

    return out;
}

llvm::Value *While::codegen()
{
    return nullptr;
}

std::ostream &Block::print(std::ostream &out, size_t indent = 0) const
{
    for (auto &s : this->statements)
    {
        s->print(out, indent);
    }

    return out;
}

llvm::Value *Block::codegen()
{
    return nullptr;
}

std::ostream &ExprStatement::print(std::ostream &out, size_t indent = 0) const
{
    this->expression->print(out, indent);
    return out;
}

llvm::Value *ExprStatement::codegen()
{
    return nullptr;
}

