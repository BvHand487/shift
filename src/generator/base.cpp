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
#include "llvm/Support/TargetSelect.h"
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

#include "generator.h"

CodegenVisitor::CodegenVisitor()
{
    builder = std::make_unique<llvm::IRBuilder<>>(*context);

    theSI->registerCallbacks(*thePIC, theMAM.get());

    theFPM->addPass(llvm::InstCombinePass());
    theFPM->addPass(llvm::ReassociatePass());
    theFPM->addPass(llvm::GVNPass());
    theFPM->addPass(llvm::SimplifyCFGPass());

    llvm::PassBuilder PB;
    PB.registerModuleAnalyses(*theMAM);
    PB.registerFunctionAnalyses(*theFAM);
    PB.crossRegisterProxies(*theLAM, *theFAM, *theCGAM, *theMAM);

    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    std::string error;
    std::string targetTriple = llvm::sys::getDefaultTargetTriple();
    const llvm::Target *target = llvm::TargetRegistry::lookupTarget(targetTriple, error);

    if (!target)
    {
        llvm::errs() << error;
        throw std::runtime_error("Target lookup failed");
    }

    std::string cpu = "generic";
    std::string features = "";

    llvm::TargetOptions opt;
    llvm::TargetMachine *targetMachine = target->createTargetMachine(targetTriple, cpu, features, opt, llvm::Reloc::PIC_);

    module->setDataLayout(targetMachine->createDataLayout());
    module->setTargetTriple(targetTriple);
}

CodegenVisitor::~CodegenVisitor() = default;

void CodegenVisitor::visit(Variable &node)
{
    llvm::AllocaInst *allocated = namedValues[node.name];

    if (!allocated)
    {
        lastValue = nullptr;
        return;
    }

    lastValue = builder->CreateLoad(allocated->getAllocatedType(), allocated, node.name.c_str());
}

void CodegenVisitor::visit(Number &node)
{
    lastValue = llvm::ConstantFP::get(*context, llvm::APFloat(node.value));
}

void CodegenVisitor::visit(Boolean &node)
{
    lastValue = llvm::ConstantInt::getBool(*context, node.value);
}

void CodegenVisitor::visit(String &node)
{
    lastValue = nullptr; // TODO
}

void CodegenVisitor::visit(BinaryOp &node)
{
    node.lhs->accept(*this);
    llvm::Value* l = lastValue;

    node.rhs->accept(*this);
    llvm::Value* r = lastValue;

    if (!l || !r)
    {
        lastValue = nullptr;
        return;
    }

    switch (node.op)
    {
    case binop_add:
        lastValue = builder->CreateFAdd(l, r, "addtmp");
        return;
    case binop_sub:
        lastValue = builder->CreateFSub(l, r, "subtmp");
        return;
    case binop_mul:
        lastValue = builder->CreateFMul(l, r, "multmp");
        return;
    case binop_div:
        lastValue = builder->CreateFDiv(l, r, "divtmp");
        return;
    case binop_exp:
        // lastValue = builder->CreateF(l, r, "addtmp");
        return;
    case binop_and:
        lastValue = builder->CreateLogicalAnd(l, r, "andtmp");
        return;
    case binop_or:
        lastValue = builder->CreateLogicalOr(l, r, "ortmp");
        return;
    case binop_bit_xor:
        lastValue = builder->CreateXor(l, r, "bxortmp");
        return;
    case binop_bit_and:
        lastValue = builder->CreateAnd(l, r, "baddtmp");
        return;
    case binop_bit_or:
        lastValue = builder->CreateOr(l, r, "bortmp");
        return;
    case binop_gt:
        lastValue = builder->CreateFCmpUGT(l, r, "gttmp");
        return;
    case binop_gte:
        lastValue = builder->CreateFCmpUGE(l, r, "getmp");
        return;
    case binop_lt:
        lastValue = builder->CreateFCmpOLT(l, r, "lttmp");
        return;
    case binop_lte:
        lastValue = builder->CreateFCmpOLE(l, r, "letmp");
        return;
    case binop_eq:
        lastValue = builder->CreateFCmpOEQ(l, r, "eqtmp");
        return;
    case binop_neq:
        lastValue = builder->CreateFCmpONE(l, r, "neqtmp");
        return;
    }

    lastValue = nullptr;
}

void CodegenVisitor::visit(UnaryOp &node)
{
    node.rhs->accept(*this);
    llvm::Value* r = lastValue;

    if (!r)
    {
        lastValue = nullptr;
        return;
    }

    switch (node.op)
    {
    case unary_add:
        lastValue = r;
        return;
    case unary_sub:
        lastValue = builder->CreateFNeg(r, "negtmp");
        return;
    case unary_not:
        lastValue = builder->CreateFCmpOEQ(r, llvm::ConstantInt::get(*context, llvm::APInt()), "nottmp"); // llvm::APInt() defaults to a value of 0
        return;
    case unary_bit_not:
        lastValue = builder->CreateNot(r, "bnottmp");
        return;
    }

    lastValue = nullptr;
}

void CodegenVisitor::visit(Assignment &node)
{
    node.rhs->accept(*this);
    llvm::Value* r = lastValue;

    if (!r)
    {
        lastValue = nullptr;
        return;
    }

    llvm::Value* var = namedValues[node.lhs->name];

    // if the variable doesnt exist yet - declare it.
    if (!var)
    {
        llvm::Function *function = builder->GetInsertBlock()->getParent();
        llvm::AllocaInst *alloca = builder->CreateAlloca(llvm::Type::getDoubleTy(*context), nullptr, node.lhs->name);
        var = namedValues[node.lhs->name] = alloca;
    }

    builder->CreateStore(r, var);

    lastValue = r;
}

void CodegenVisitor::visit(Prototype &node)
{
    lastValue = nullptr;
}

void CodegenVisitor::visit(Definition &node)
{
    // llvm::Type* type;
    // std::vector<llvm::Type *> params;
    // if (node.name == "main")
    // {
    //     type = llvm::Type::getInt32Ty(*context);
    // }
    // else
    // {
    //     type = llvm::Type::getDoubleTy(*context);
    //     params.insert(params.end(), node.args.size(), type);
    // }

    // llvm::FunctionType *functionType = llvm::FunctionType::get(type, params, false);
    // llvm::Function *function = llvm::Function::Create(functionType, llvm::Function::ExternalLinkage, node.name, *module);

    // size_t idx = 0;
    // for (auto &arg : function->args())
    //     arg.setName(node.args[idx++]->name);

    // BasicBlock *block = BasicBlock::Create(*context, "entry", function);
    // builder->SetInsertPoint(block);

    // namedValues.clear();
    // for (auto &arg : function->args())
    // {
    //     llvm::AllocaInst *alloca = builder->CreateAlloca(llvm::Type::getDoubleTy(*context), nullptr, arg.name);

    //     builder->CreateStore(&arg, alloca);

    //     namedValues[std::string(arg.name)] = alloca;
    // }

    // llvm::Value* retVal = nullptr;
    // for (auto &statement : node.body)
    //     retVal = statement->codegen();

    // if (!llvm::verifyFunction(*function))
    // {
    //     theFPM->run(*function, *theFAM);
    //     lastValue = function;
    // }

    // lastValue = nullptr;

    lastValue = nullptr;
}

void CodegenVisitor::visit(Return &node)
{
    // if (node.value == nullptr) {
    //     lastValue = builder->CreateRetVoid();
    // }

    // llvm::Value* retVal = node.value->codegen();
    // if (!retVal)
    //     lastValue = nullptr;return;

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
    //         lastValue = nullptr;
    //         return;
    //     }
    // }

    // lastValue = builder->CreateRet(retVal);

    lastValue = nullptr;
}

void CodegenVisitor::visit(CallExpr &node)
{
    // llvm::Function *callee = module->getFunction(node.callee);

    // if (!callee)
    //     lastValue = nullptr;return;

    // if (callee->arg_size() != node.args.size())
    //     lastValue = nullptr;return;

    // std::vector<llvm::Value*> argValues;
    // for (int i = 0; i < node.args.size(); ++i)
    // {
    //     argValues.push_back(node.args[i]->codegen());

    //     if (!argValues.back()) {
    //         lastValue = nullptr;
    //         return;
    //     }
    // }

    // lastValue = builder->CreateCall(callee, argValues, "calltmp");

    lastValue = nullptr;
}

void CodegenVisitor::visit(If &node)
{
    node.cond->accept(*this);
    llvm::Value* cond = lastValue;

    if (!cond)
    {
        lastValue = nullptr;
        return;
    }

    llvm::Function *func = builder->GetInsertBlock()->getParent();

    llvm::BasicBlock *thenBB = llvm::BasicBlock::Create(*context, "then", func);
    llvm::BasicBlock *elseBB = llvm::BasicBlock::Create(*context, "else");
    llvm::BasicBlock *mergedBB = llvm::BasicBlock::Create(*context, "merged");

    builder->CreateCondBr(cond, thenBB, elseBB);

    // - - - THEN BLOCK - - - //
    builder->SetInsertPoint(thenBB);

    node.then_branch->accept(*this);
    llvm::Value* thenLastValue = lastValue;

    builder->CreateBr(mergedBB);
    thenBB = builder->GetInsertBlock();

    // - - - ELSE BLOCK - - - //
    func->insert(func->end(), elseBB);

    builder->SetInsertPoint(elseBB);
    
    node.else_branch->accept(*this);
    llvm::Value* elseLastValue = lastValue;

    builder->CreateBr(mergedBB);
    elseBB = builder->GetInsertBlock();

    // - - - MERGED BLOCK - - - //
    func->insert(func->end(), mergedBB);
    builder->SetInsertPoint(mergedBB);

    llvm::PHINode *phi = builder->CreatePHI(llvm::Type::getDoubleTy(*context), 2, "iftmp");

    phi->addIncoming(thenLastValue, thenBB);
    phi->addIncoming(elseLastValue, elseBB);

    lastValue = phi;
}

void CodegenVisitor::visit(While &node)
{
    lastValue = nullptr;
}

void CodegenVisitor::visit(Block &node)
{
    if (node.statements.empty())
    {
        lastValue = nullptr;
        return;
    }
        
    for (auto &statement : node.statements)
    {
        statement->accept(*this);

        if (!lastValue)
        {
            lastValue = nullptr;
            return;
        }
    }
}

void CodegenVisitor::visit(ExprStatement &node)
{
    lastValue = nullptr;
}
