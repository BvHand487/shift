#include <iostream>
#include <fstream>

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

std::string read_file(const std::string &path)
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