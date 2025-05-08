#ifndef COMPILER_H
#define COMPILER_H

#include <string>

std::string read_file(const std::string& path);

int compile(const std::string& filepath);


#endif
