#include <algorithm>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>

#include "compiler.h"
#include "lexer.h"
#include "parser.h"


std::string read_file(const std::string& path)
{
    std::ifstream file(path, std::ios::binary);

    if (!file.is_open())
        return "";

    file.seekg(0, std::ios::end);
    std::size_t size = file.tellg();
    std::string buffer(size, '\0');
    file.seekg(0);
    file.read(&buffer[0], size);

    file.close();
    return buffer;
}

int compile(const std::string& path)
{
    std::string contents = std::move(read_file(path));

    auto tokens = tokenize(contents);
    for (auto t : tokens)
        std::cout << t << std::endl;

    std::cout << "\n";

    auto ast = parse(tokens);
    for (const auto& a : ast)
        std::cout << *a << std::endl;

    return 0;
}