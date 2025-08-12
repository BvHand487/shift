#ifndef OPERATORS_H
#define OPERATORS_H

#include <unordered_map>

enum BinaryOpType
{
    binop_none = -1,

    // Arithmetic
    binop_add,
    binop_sub,
    binop_mul,
    binop_div,
    binop_mod,
    binop_exp,

    // Logical
    binop_and,
    binop_or,

    // Bitwise
    binop_bit_xor,
    binop_bit_and,
    binop_bit_or,

    // Comparison
    binop_gt,
    binop_gte,
    binop_lt,
    binop_lte,
    binop_eq,
    binop_neq
};

enum UnaryOpType
{
    unary_none = -1,

    unary_add,
    unary_sub,
    unary_not,
    unary_bit_not
};

#define BINARY_OP_TO_STR_MAPPINGS \
    ROW(binop_add, "Add") \
    ROW(binop_sub, "Sub") \
    ROW(binop_mul, "Mul") \
    ROW(binop_div, "Div") \
    ROW(binop_mod, "Mod") \
    ROW(binop_exp, "Exp") \
    ROW(binop_and, "And") \
    ROW(binop_or, "Or") \
    ROW(binop_bit_xor, "bXor") \
    ROW(binop_bit_and, "bAnd") \
    ROW(binop_bit_or, "bOr") \
    ROW(binop_gt, "GT") \
    ROW(binop_gte, "GTE") \
    ROW(binop_lt, "LT") \
    ROW(binop_lte, "LTE") \
    ROW(binop_eq, "EQ") \
    ROW(binop_neq, "NEQ")

#define UNARY_OP_TO_STR_MAPPINGS \
    ROW(unary_add, "Pos") \
    ROW(unary_sub, "Neg") \
    ROW(unary_not, "Not") \
    ROW(unary_bit_not, "bNot")

#define ROW(op, str) { op, str },
const std::unordered_map<BinaryOpType, std::string> binop_to_str = {
    BINARY_OP_TO_STR_MAPPINGS
};
#undef ROW

#define ROW(op, str) { op, str },
const std::unordered_map<UnaryOpType, std::string> unary_to_str = {
    UNARY_OP_TO_STR_MAPPINGS
};
#undef ROW


#endif