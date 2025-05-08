#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <unordered_map>
#include <variant>

#include "ast.h"


using Value = std::variant<int, double, std::string>();


class Environment
{
    std::unordered_map<std::string, Value> vars;
};


#endif