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
#include "llvm/ADT/StringMap.h"
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


llvm::Type* CodegenVisitor::type_to_llvm_type(Type type)
{
    switch (type)
    {
        case Type::Int:
            return llvm::Type::getInt32Ty(*context); break;
        case Type::Bool:
            return llvm::Type::getInt1Ty(*context); break;
        case Type::String:
            return llvm::Type::getInt8Ty(*context)->getPointerTo(); break;
        case Type::Void:
            return llvm::Type::getVoidTy(*context); break;

        default:
            throw std::runtime_error("Unknown type");
    }
}


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
    this->targetMachine = target->createTargetMachine(targetTriple, cpu, features, opt, llvm::Reloc::PIC_);

    module->setDataLayout(targetMachine->createDataLayout());
    module->setTargetTriple(targetTriple);
}

CodegenVisitor::~CodegenVisitor() = default;

bool CodegenVisitor::write_to_file(const std::string &path)
{
    std::error_code EC;
    llvm::raw_fd_ostream dest(path, EC, llvm::sys::fs::OF_None);

    if (EC)
    {
        llvm::errs() << "Could not open output file: " << EC.message();
        return 1;
    }

    llvm::legacy::PassManager pass;
    auto filetype = llvm::CodeGenFileType::ObjectFile;

    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, filetype)) {
        llvm::errs() << "Target machine can't emit a file of this type";
        return 1;
    }

    pass.run(*module);
    dest.flush();

    return 0;
}


void CodegenVisitor::visit(Variable &node)
{
    llvm::AllocaInst *alloca = namedValues[node.name];

    if (!alloca)
    {
        throw std::runtime_error(std::format("Referenced undeclared variable '{}'", node.name));
    }

    lastValue = builder->CreateLoad(alloca->getAllocatedType(), alloca, node.name.c_str());
}

void CodegenVisitor::visit(Number &node)
{
    lastValue = llvm::ConstantInt::get(*context, llvm::APInt(32, node.value));
}

void CodegenVisitor::visit(Boolean &node)
{
    lastValue = llvm::ConstantInt::getBool(*context, node.value);
}

void CodegenVisitor::visit(String &node)
{
    llvm::Constant *strLiteral = llvm::ConstantDataArray::getString(*context, node.value, true);

    llvm::ArrayType *strType = llvm::ArrayType::get(llvm::Type::getInt8Ty(*context), node.value.size() + 1);

    auto globalStr = new llvm::GlobalVariable(
        *module,
        strType,
        true,
        llvm::GlobalValue::PrivateLinkage,
        strLiteral,
        ".str"
    );
    globalStr->setAlignment(llvm::MaybeAlign(1));

    llvm::Constant *zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), 0);

    llvm::Constant *indices[] = { zero, zero };
    
    lastValue = llvm::ConstantExpr::getInBoundsGetElementPtr(strType, globalStr, indices);
}

void CodegenVisitor::visit(BinaryOp &node)
{
    node.lhs->accept(*this);
    llvm::Value *l = lastValue;

    node.rhs->accept(*this);
    llvm::Value *r = lastValue;

    if (!l || !r)
    {
        lastValue = nullptr;
        return;
    }

    switch (node.op)
    {
    case binop_add:
        lastValue = builder->CreateAdd(l, r, "addtmp");
        return;
    case binop_sub:
        lastValue = builder->CreateSub(l, r, "subtmp");
        return;
    case binop_mul:
        lastValue = builder->CreateMul(l, r, "multmp");
        return;
    case binop_div:
        lastValue = builder->CreateSDiv(l, r, "divtmp");
        return;
    case binop_mod:
        lastValue = builder->CreateSRem(l, r, "modtmp");
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
        lastValue = builder->CreateICmpSGT(l, r, "gttmp");
        return;
    case binop_gte:
        lastValue = builder->CreateICmpSGE(l, r, "getmp");
        return;
    case binop_lt:
        lastValue = builder->CreateICmpSLT(l, r, "lttmp");
        return;
    case binop_lte:
        lastValue = builder->CreateICmpSLE(l, r, "letmp");
        return;
    case binop_eq:
        lastValue = builder->CreateICmpEQ(l, r, "eqtmp");
        return;
    case binop_neq:
        lastValue = builder->CreateICmpNE(l, r, "neqtmp");
        return;
    }

    lastValue = nullptr;
}

void CodegenVisitor::visit(UnaryOp &node)
{
    node.rhs->accept(*this);
    llvm::Value *r = lastValue;

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
        lastValue = builder->CreateNeg(r, "negtmp");
        return;
    case unary_not:
        lastValue = builder->CreateICmpEQ(r, llvm::ConstantInt::get(*context, llvm::APInt()), "nottmp"); // llvm::APInt() defaults to a value of 0
        return;
    case unary_bit_not:
        lastValue = builder->CreateNot(r, "bnottmp");
        return;
    }

    lastValue = nullptr;
}

void CodegenVisitor::visit(Parameter &node) { lastValue = nullptr; }

void CodegenVisitor::visit(VariableDecl &node)
{
    llvm::Function *function = builder->GetInsertBlock()->getParent();

    llvm::IRBuilder<> tmpB(&function->getEntryBlock(), function->getEntryBlock().begin());

    llvm::AllocaInst *alloca = tmpB.CreateAlloca(type_to_llvm_type(node.type), nullptr, node.name);
    namedValues[node.name] = alloca;
    
    if (node.init != nullptr)
    {
        node.init->accept(*this);
        builder->CreateStore(lastValue, alloca);
    }
    else
    {
        llvm::Value *defaultVal = nullptr;

        switch (node.type)
        {
            case Type::Int:
                defaultVal = llvm::ConstantInt::get(type_to_llvm_type(Type::Int), 0);
                break;
            case Type::Bool:
                defaultVal = llvm::ConstantInt::get(type_to_llvm_type(Type::Bool), 0);
                break;
            case Type::String:
                defaultVal = llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(type_to_llvm_type(Type::String)));
                break;
            default:
                throw std::runtime_error("No default initializer for this type");
        }

        builder->CreateStore(defaultVal, alloca);
    }

    lastValue = nullptr;
}

void CodegenVisitor::visit(Assignment &node)
{
    node.rhs->accept(*this);
    llvm::Value *r = lastValue;

    if (!r)
    {
        lastValue = nullptr;
        return;
    }

    llvm::Value *var = namedValues[node.lhs->name];

    builder->CreateStore(r, var);

    lastValue = r;
}

void CodegenVisitor::visit(Prototype &node)
{
    llvm::Type *type = type_to_llvm_type(node.retType);

    std::vector<llvm::Type *> params;
    for (size_t i = 0; i < node.args.size(); ++i)
        params.push_back(type_to_llvm_type(node.args[i]->type));

    llvm::FunctionType *functionType = llvm::FunctionType::get(type, params, node.isVarArg);
    llvm::Function *function = llvm::Function::Create(functionType, llvm::Function::ExternalLinkage, node.name, *module);

    size_t idx = 0;
    for (auto &arg : function->args())
        arg.setName(node.args[idx++]->name);

    lastValue = function;
}

void CodegenVisitor::visit(Definition &node)
{
    llvm::Function *function = module->getFunction(node.type->name);

    if (function == nullptr)
    {
        node.type->accept(*this);
        function = (llvm::Function *) lastValue;
    }

    if (function == nullptr)
    {
        lastValue = nullptr;
        return;
    }

    // TODO: handle function redefinition

    llvm::BasicBlock *block = llvm::BasicBlock::Create(*context, "entry", function);
    builder->SetInsertPoint(block);

    namedValues.clear();
    for (auto &arg : function->args())
    {
        llvm::AllocaInst *alloca = builder->CreateAlloca(arg.getType(), nullptr, arg.getName());
        builder->CreateStore(&arg, alloca);
        namedValues[arg.getName().str()] = alloca;
    }

    node.body->accept(*this);

    if (!llvm::verifyFunction(*function))
    {
        theFPM->run(*function, *theFAM);
        lastValue = function;
        return;
    }

    lastValue = nullptr;
}

void CodegenVisitor::visit(Return &node)
{
    // no return value, e.g.: "return;"
    if (node.value == nullptr)
    {
        lastValue = builder->CreateRetVoid();
        return;
    }

    // get return value
    node.value->accept(*this);
    llvm::Value *retVal = lastValue;

    if (!retVal)
    {
        lastValue = nullptr;
        return;
    }

    lastValue = builder->CreateRet(retVal);
}

void CodegenVisitor::visit(CallExpr &node)
{
    llvm::Function *callee = module->getFunction(node.callee);

    if (!callee)
    {
        lastValue = nullptr;
        return;
    }

    if (!callee->isVarArg())
    {
        if (node.args.size() != callee->arg_size())
        {
            lastValue = nullptr;
            return;
        }
    }
    else
    {
        if (node.args.size() < callee->arg_size())
        {
            lastValue = nullptr;
            return;
        }
    }

    std::vector<llvm::Value *> argValues;

    for (size_t i = 0; i < node.args.size(); ++i)
    {
        node.args[i]->accept(*this);
        if (!lastValue)
        {
            lastValue = nullptr;
            return;
        }

        argValues.push_back(lastValue);
    }

    lastValue = builder->CreateCall(callee, argValues, "calltmp");
}

void CodegenVisitor::visit(If &node)
{
    node.cond->accept(*this);
    llvm::Value *cond = lastValue;

    if (!cond)
    {
        lastValue = nullptr;
        return;
    }

    llvm::Function *func = builder->GetInsertBlock()->getParent();

    llvm::BasicBlock *thenBB = llvm::BasicBlock::Create(*context, "then", func);
    llvm::BasicBlock *elseBB = nullptr;
    llvm::BasicBlock *mergedBB = llvm::BasicBlock::Create(*context, "merged");

    if (node.else_branch)
        elseBB = llvm::BasicBlock::Create(*context, "else", func);

    builder->CreateCondBr(cond, thenBB, elseBB ? elseBB : mergedBB);

    // - - - THEN BLOCK - - - //
    builder->SetInsertPoint(thenBB);
    node.then_branch->accept(*this);

    auto terminator = builder->GetInsertBlock()->getTerminator();
    if (!terminator || !llvm::isa<llvm::ReturnInst>(terminator))
        builder->CreateBr(mergedBB);

    thenBB = builder->GetInsertBlock();

    // - - - ELSE BLOCK - - - //
    if (node.else_branch)
    {
        builder->SetInsertPoint(elseBB);
        node.else_branch->accept(*this);

        auto terminator = builder->GetInsertBlock()->getTerminator();
        if (!terminator || !llvm::isa<llvm::ReturnInst>(terminator))
            builder->CreateBr(mergedBB);

        elseBB = builder->GetInsertBlock();
    }

    // - - - MERGED BLOCK - - - //
    func->insert(func->end(), mergedBB);
    builder->SetInsertPoint(mergedBB);

    lastValue = nullptr;
}

void CodegenVisitor::visit(While &node)
{
    llvm::Function *func = builder->GetInsertBlock()->getParent();
    llvm::BasicBlock *condBB = llvm::BasicBlock::Create(*context, "cond", func);
    llvm::BasicBlock *bodyBB = llvm::BasicBlock::Create(*context, "body", func);
    llvm::BasicBlock *mergedBB = llvm::BasicBlock::Create(*context, "merged");

    builder->CreateBr(condBB);

    // - - - CONDITION - - - //
    builder->SetInsertPoint(condBB);
    node.cond->accept(*this);
    llvm::Value *cond = lastValue;

    if (!cond)
    {
        lastValue = nullptr;
        return;
    }

    builder->CreateCondBr(cond, bodyBB, mergedBB);

    // - - - BODY - - - //
    builder->SetInsertPoint(bodyBB);
    node.body->accept(*this);

    auto terminator = builder->GetInsertBlock()->getTerminator();
    if (!terminator || !llvm::isa<llvm::ReturnInst>(terminator))
        builder->CreateBr(condBB);

    // - - - MERGED - - - //
    func->insert(func->end(), mergedBB);
    builder->SetInsertPoint(mergedBB);

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
    }
}

void CodegenVisitor::visit(ExprStatement &node)
{
    node.expression->accept(*this);
    lastValue = nullptr;
}
