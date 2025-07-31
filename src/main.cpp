#include <map>
#include <filesystem>
#include <iostream>
#include <fstream>

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
#include "compiler.h"


std::unique_ptr<llvm::LLVMContext> CodegenVisitor::context = std::make_unique<llvm::LLVMContext>();
std::unique_ptr<llvm::Module> CodegenVisitor::module = std::make_unique<llvm::Module>("main", *context);

std::unique_ptr<llvm::FunctionPassManager> CodegenVisitor::theFPM = std::make_unique<llvm::FunctionPassManager>();
std::unique_ptr<llvm::LoopAnalysisManager> CodegenVisitor::theLAM = std::make_unique<llvm::LoopAnalysisManager>();
std::unique_ptr<llvm::FunctionAnalysisManager> CodegenVisitor::theFAM = std::make_unique<llvm::FunctionAnalysisManager>();
std::unique_ptr<llvm::CGSCCAnalysisManager> CodegenVisitor::theCGAM = std::make_unique<llvm::CGSCCAnalysisManager>();
std::unique_ptr<llvm::ModuleAnalysisManager> CodegenVisitor::theMAM = std::make_unique<llvm::ModuleAnalysisManager>();
std::unique_ptr<llvm::PassInstrumentationCallbacks> CodegenVisitor::thePIC = std::make_unique<llvm::PassInstrumentationCallbacks>();
std::unique_ptr<llvm::StandardInstrumentations> CodegenVisitor::theSI = std::make_unique<llvm::StandardInstrumentations>(*context, true);


int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Invalid usage.\n";
        return 1;
    }

    compile(argv[1]);
    CodegenVisitor::module->print(llvm::errs(), nullptr);

    // std::string inputFilename = argv[1];
    // std::string outputFilename = inputFilename.substr(0, inputFilename.find_last_of('.')) + ".o";

    // std::error_code EC;
    // llvm::raw_fd_ostream dest(outputFilename, EC, llvm::sys::fs::OF_None);

    // if (EC)
    // {
    //     llvm::errs() << "Could not open output file: " << EC.message();
    //     return 1;
    // }

    // llvm::legacy::PassManager pass;
    // auto filetype = llvm::CodeGenFileType::ObjectFile;

    // if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, filetype)) {
    //     llvm::errs() << "Target machine can't emit a file of this type";
    //     return 1;
    // }

    // pass.run(*module);
    // dest.flush();

    return 0;
}
