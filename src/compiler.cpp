#include <algorithm>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>


#include "lexer.h"
#include "parser.h"
#include "analyzer/base.h"
#include "generator.h"
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
        a->accept(printer);

    std::cout << std::endl << std::endl;

    auto analyzer = AnalyzerVisitor();
    for (const auto &a : ast)
        a->accept(analyzer);

    auto printer2 = PrintVisitor();
    for (const auto &a : ast)
        a->accept(printer2);

    auto generator = CodegenVisitor();
    for (const auto &a : ast)
        a->accept(generator);

    std::string inputFilename = path.substr(path.find_last_of("/") + 1);
    std::string outputFilename = inputFilename.substr(0, inputFilename.find_last_of('.')) + ".o";

    std::string objectFilePath = "./" + outputFilename;
    bool fail = generator.write_to_file(objectFilePath);
    if (fail) {
        std::cerr << "Failed to generate object file\n";
        return 1;
    }

    std::string executableName = inputFilename.substr(0, inputFilename.find_last_of('.'));

    std::string linkCommand = "gcc " + objectFilePath + " -o ./" + executableName;

    int linkResult = std::system(linkCommand.c_str());
    if (linkResult != 0) {
        std::cerr << "Linking failed with exit code " << linkResult << "\n";
        return 1;
    }

    std::cout << "Successfully generated executable: ./" << executableName << "\n";

    return 0;
}