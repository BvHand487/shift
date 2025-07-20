#ifndef POSITION_H
#define POSITION_H

#include <cstdlib>
#include <format>

struct Position
{
public:
    size_t line;
    size_t column;
    
    Position() = default;
    Position(size_t line, size_t column) : line(line), column(column) {}
};


#endif