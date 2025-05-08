#include <iostream>
#include <fstream>
#include <filesystem>

#include "compiler.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Invalid usage.\n";
        return 1;
    }

    return compile(argv[1]);
}
