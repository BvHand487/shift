#include <iostream>
#include <fstream>
#include <filesystem>

#include "llvm/Support/Format.h"
#include "llvm/Support/raw_os_ostream.h"

#include "compiler.h"
#include "ast.h"

std::unique_ptr<llvm::LLVMContext> context;
std::unique_ptr<llvm::Module> module;
std::unique_ptr<llvm::IRBuilder<>> builder;

static void initModule(const std::string_view& name)
{
    context = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>(name, *context);

    builder = std::make_unique<llvm::IRBuilder<>>(*context);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Invalid usage.\n";
        return 1;
    }

    initModule("main");

    compile(argv[1]);

    llvm::raw_os_ostream os(std::cout);
    // module->print(os, nullptr);
    module->print(llvm::errs(), nullptr);

    return 0;
}
