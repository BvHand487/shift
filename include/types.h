#ifndef TYPES_H
#define TYPES_H

#include "lexer/token.h"

enum class Type
{
    Unknown = -1,
    Int,
    Bool,
    String,
    Void
};

namespace std
{
    template <>
    struct hash<Type>
    {
        size_t operator()(const Type &t) const noexcept {
            return static_cast<size_t>(t);
        }
    };
}


#define TYPE_STR_MAPPINGS     \
    ROW(Type::Unknown, "???") \
    ROW(Type::Int, "int")     \
    ROW(Type::Bool, "bool")   \
    ROW(Type::String, "str")  \
    ROW(Type::Void, "")

#define TYPE_TOKEN_MAPPINGS   \
    ROW(Type::Int, tok_int)   \
    ROW(Type::Bool, tok_bool) \
    ROW(Type::String, tok_str)

#define ROW(type, str) {type, str},
const std::unordered_map<Type, std::string> type_to_str = {
    TYPE_STR_MAPPINGS
};
#undef ROW

#define ROW(type, tok) {tok, type},
const std::unordered_map<TokenType, Type> token_to_type = {
    TYPE_TOKEN_MAPPINGS
};
#undef ROW

#endif