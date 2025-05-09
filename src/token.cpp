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

        case tok_open_paren: return "tok_open_paren";
        case tok_close_paren: return "tok_close_paren";
        case tok_open_brace: return "tok_open_brace";
        case tok_close_brace: return "tok_close_brace";

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
        case tok_star: return "tok_star";
        case tok_slash: return "tok_slash";
        case tok_exponentiation: return "tok_exponentiation";

        case tok_caret: return "tok_caret";
        case tok_tilde: return "tok_tilde";
        case tok_ampersand: return "tok_ampersand";
        case tok_pipe: return "tok_pipe";
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