#ifndef ANALYZER_SYMBOLS_H
#define ANALYZER_SYMBOLS_H

#include "llvm/IR/Value.h"

#include "types.h"


struct Symbol
{
    std::string name;
};

struct VarSymbol : public Symbol {
    Type type;
    bool isMutable = true;
    llvm::Value* llvmValue = nullptr;
};

struct ParamSymbol : public Symbol {
    Type type;
    bool hasInit = false;
};

struct FuncSymbol : public Symbol {
    Type retType;
    bool isExtern;
    bool isVarArg;
    std::vector<ParamSymbol> args;
    bool isDefined = false;
    llvm::Value* llvmValue = nullptr;
};


class SymbolTable
{
private:
    std::unordered_map<std::string, FuncSymbol> functions;
    std::vector<std::unordered_map<std::string, VarSymbol>> varScopes;

public:
    void enterScope();
    void exitScope();
    
    void addVariable(const VarSymbol& var);
    VarSymbol* lookupVariable(const std::string& name);

    void addFunction(const FuncSymbol& func);
    FuncSymbol* lookupFunction(const std::string& name);
};

#endif