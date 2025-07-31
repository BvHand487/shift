#include <algorithm>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>

#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"

#include "lexer.h"
#include "parser.h"
#include "compiler.h"


int compile(const std::string &path)
{
    std::string contents = std::move(read_file(path));

    auto lexer = Lexer(contents);
    auto tokens = lexer.tokenize();

    for (auto t : tokens)
        std::cout << t << std::endl;

    std::cout << "\n";

    auto parser = Parser(tokens);
    auto ast = parser.parse();

    auto printer = PrintVisitor();
    for (const auto &a : ast)
    {
        a->accept(printer);
    }

    // for (const auto &statement : ast)
    //     statement->codegen();

    return 0;
}