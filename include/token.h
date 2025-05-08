#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <ostream>

#include "position.h"
#include "utils.h"

enum TokenType
{
    // structure
    tok_invalid = -1,
    tok_delimiter = 0,

    tok_comment,

    tok_left_parentheses,
    tok_right_parentheses,
    tok_left_braces,
    tok_right_braces,

    // literals
    tok_number,
    tok_string,
    tok_boolean,

    // primary
    tok_identifier,
    
    // operators
    tok_assignment,
    tok_relationship,

    tok_plus,
    tok_minus,
    tok_multiplication,
    tok_division,
    tok_exponentiation,

    tok_bitwise_xor,
    tok_bitwise_not,
    tok_bitwise_and,
    tok_bitwise_or,
    tok_not,
    tok_and,
    tok_or,
    
    tok_gt,
    tok_gte,
    tok_lt,
    tok_lte,
    tok_eq,
    tok_neq,

    // keywords
    tok_if,
    tok_else,
    tok_while,
};

std::string token_type_to_string(TokenType type);

struct Token
{
    TokenType type;
    Position position;
    std::string lexeme;

public:
    Token() = default;    
    Token(TokenType type, Position position, std::string lexeme);

    friend std::ostream& operator<< (std::ostream& out, const Token& obj)
    {
        out << "< " << token_type_to_string(obj.type) << " : " << to_escaped_string(obj.lexeme) << " >";
        return out;
    }
};

#endif