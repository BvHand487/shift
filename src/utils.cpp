#include <iostream>

#include "utils.h"

std::string to_escaped_string(const std::string& str)
{
    std::string out = "";

    for (int i = 0; i < str.length(); ++i)
    {
        if (str[i] == '\n')
            out += "\\n";
        else if (str[i] == '\t')
            out += "\\t";
        else
            out += str[i];
    }

    return out;
}


std::string create_indent(size_t indent_level, size_t indent_size)
{
    return std::string(indent_level * indent_size, ' ');
}