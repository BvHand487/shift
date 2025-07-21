#ifndef UTILS_H
#define UTILS_H

#include <string>

std::string to_escaped_string(const std::string& str);

std::string read_file(const std::string& path);

#endif