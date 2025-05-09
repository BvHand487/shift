#ifndef BUILDER_H
#define BUILDER_H

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>


llvm::LLVMContext Context;
llvm::Module* TheModule = new llvm::Module("main", Context);
llvm::IRBuilder<> Builder(Context);


#endif