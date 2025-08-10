#include "analyzer/symbols.h"

void SymbolTable::enterScope()
{
    varScopes.emplace_back();
}

void SymbolTable::exitScope()
{
    if (!varScopes.empty())
        varScopes.pop_back();
}

void SymbolTable::addVariable(const VarSymbol &var)
{
    auto& scope = varScopes.back();

    scope.insert({ var.name, var });
}

VarSymbol* SymbolTable::lookupVariable(const std::string &name)
{
    
    for (auto scope = varScopes.rbegin(); scope != varScopes.rend(); ++scope)
    {
        auto found = scope->find(name);
        if (found != scope->end()) {
            return &found->second;
        }
    }

    return nullptr;
}

void SymbolTable::addFunction(const FuncSymbol &func)
{
    functions.insert({ func.name, func });
}

FuncSymbol* SymbolTable::lookupFunction(const std::string &name)
{
    auto iter = functions.find(name);

    if (iter != functions.end())
        return &iter->second;
    else
        return nullptr;
}