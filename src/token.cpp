#include <cstdlib>
#include <string>

#include "token.h"
#include "position.h"

std::string token_type_to_string(TokenType type)
{
    switch (type)
    {
        // structure
        case tok_invalid: return "tok_invalid";
        case tok_delimiter: return "tok_delimiter";

        case tok_comment: return "tok_comment";

        case tok_left_parentheses: return "tok_left_parentheses";
        case tok_right_parentheses: return "tok_right_parentheses";
        case tok_left_braces: return "tok_left_braces";
        case tok_right_braces: return "tok_right_braces";

        // literals
        case tok_number: return "tok_number";
        case tok_string: return "tok_string";
        case tok_boolean: return "tok_boolean";

        // primary
        case tok_identifier: return "tok_identifier";
        
        // operators
        case tok_assignment: return "tok_assignment";
        case tok_relationship: return "tok_relationship";

        case tok_plus: return "tok_plus";
        case tok_minus: return "tok_minus";
        case tok_multiplication: return "tok_multiplication";
        case tok_division: return "tok_division";
        case tok_exponentiation: return "tok_exponentiation";

        case tok_bitwise_xor: return "tok_bitwise_xor";
        case tok_bitwise_not: return "tok_bitwise_not";
        case tok_bitwise_and: return "tok_bitwise_and";
        case tok_bitwise_or: return "tok_bitwise_or";
        case tok_not: return "tok_not";
        case tok_and: return "tok_and";
        case tok_or: return "tok_or";
        
        case tok_gt: return "tok_gt";
        case tok_gte: return "tok_gte";
        case tok_lt: return "tok_lt";
        case tok_lte: return "tok_lte";
        case tok_eq: return "tok_eq";
        case tok_neq: return "tok_neq";

        // keywords
        case tok_if: return "tok_if";
        case tok_else: return "tok_else";
        case tok_while: return "tok_while";

        default:
            return "None";
    }
}

Token::Token(TokenType type, Position position, std::string lexeme)
{
    this->type = type;
    this->position = position;
    this->lexeme = lexeme;
}