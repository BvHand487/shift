#ifndef POSITION_H
#define POSITION_H

#include <cstdlib>

struct Position
{
    size_t line;
    size_t column;

public:
    Position() = default;
    Position(size_t line, size_t column) : line(line), column(column) {}
};


#endif