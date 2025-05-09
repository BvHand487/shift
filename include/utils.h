#ifndef UTILS_H
#define UTILS_H

#include <string>

std::string to_escaped_string(const std::string& str);

std::string create_indent(size_t indent_level, size_t indent_size=4);

#endif