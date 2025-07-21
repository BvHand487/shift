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

#include "compiler.h"
#include "ast.h"

std::unique_ptr<llvm::LLVMContext> context;
std::unique_ptr<llvm::Module> module;
std::unique_ptr<llvm::IRBuilder<>> builder;

std::unique_ptr<llvm::FunctionPassManager> theFPM;
std::unique_ptr<llvm::LoopAnalysisManager> theLAM;
std::unique_ptr<llvm::FunctionAnalysisManager> theFAM;
std::unique_ptr<llvm::CGSCCAnalysisManager> theCGAM;
std::unique_ptr<llvm::ModuleAnalysisManager> theMAM;
std::unique_ptr<llvm::PassInstrumentationCallbacks> thePIC;
std::unique_ptr<llvm::StandardInstrumentations> theSI;

static void initModule(const std::string_view &name)
{
    context = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>(name, *context);
    builder = std::make_unique<llvm::IRBuilder<>>(*context);

    theFPM = std::make_unique<llvm::FunctionPassManager>();
    theLAM = std::make_unique<llvm::LoopAnalysisManager>();
    theFAM = std::make_unique<llvm::FunctionAnalysisManager>();
    theCGAM = std::make_unique<llvm::CGSCCAnalysisManager>();
    theMAM = std::make_unique<llvm::ModuleAnalysisManager>();
    thePIC = std::make_unique<llvm::PassInstrumentationCallbacks>();
    theSI = std::make_unique<llvm::StandardInstrumentations>(*context, true);
    theSI->registerCallbacks(*thePIC, theMAM.get());

    theFPM->addPass(llvm::InstCombinePass());
    theFPM->addPass(llvm::ReassociatePass());
    theFPM->addPass(llvm::GVNPass());
    theFPM->addPass(llvm::SimplifyCFGPass());

    llvm::PassBuilder PB;
    PB.registerModuleAnalyses(*theMAM);
    PB.registerFunctionAnalyses(*theFAM);
    PB.crossRegisterProxies(*theLAM, *theFAM, *theCGAM, *theMAM);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Invalid usage.\n";
        return 1;
    }

    initModule("main");

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
        return 1;
    }

    std::string cpu = "generic";
    std::string features = "";

    llvm::TargetOptions opt;
    llvm::TargetMachine *targetMachine = target->createTargetMachine(targetTriple, cpu, features, opt, llvm::Reloc::PIC_);

    module->setDataLayout(targetMachine->createDataLayout());
    module->setTargetTriple(targetTriple);

    compile(argv[1]);
    module->print(llvm::errs(), nullptr);

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
