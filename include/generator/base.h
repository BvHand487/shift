#ifndef GENERATOR_BASE_H
#define GENERATOR_BASE_H

#include <vector>
#include <string>
#include <memory>
#include <map>

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Scalar/Reassociate.h"
#include "llvm/Transforms/Scalar/SimplifyCFG.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/StandardInstrumentations.h"
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

#include "ast.h"


using namespace ast;


// Refactor context/module logic into a separate class like "Generator" or sum
class CodegenVisitor : public Visitor
{
private:
    llvm::Value* lastValue = nullptr;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::map<std::string, llvm::AllocaInst *> namedValues;

public:
    static std::unique_ptr<llvm::LLVMContext> context;
    static std::unique_ptr<llvm::Module> module;

    static std::unique_ptr<llvm::FunctionPassManager> theFPM;
    static std::unique_ptr<llvm::LoopAnalysisManager> theLAM;
    static std::unique_ptr<llvm::FunctionAnalysisManager> theFAM;
    static std::unique_ptr<llvm::CGSCCAnalysisManager> theCGAM;
    static std::unique_ptr<llvm::ModuleAnalysisManager> theMAM;
    static std::unique_ptr<llvm::PassInstrumentationCallbacks> thePIC;
    static std::unique_ptr<llvm::StandardInstrumentations> theSI;

    CodegenVisitor();
    ~CodegenVisitor();

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