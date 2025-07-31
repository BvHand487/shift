#include <cstdlib>
#include <string>

#include "lexer.h"


Token::Token(TokenType type, Position position, std::string lexeme)
{
    this->type = type;
    this->position = position;
    this->lexeme = lexeme;
}


int token_op_to_precedence(TokenType type)
{
    switch (type)
    {
    case tok_exponentiation:
        return 14;

    case tok_tilde:
        return 13;

    case tok_star:
        return 12;
    case tok_slash:
        return 12;

    case tok_plus:
        return 11;
    case tok_minus:
        return 11;

    case tok_ampersand:
        return 9;

    case tok_caret:
        return 8;

    case tok_pipe:
        return 7;

    case tok_gt:
        return 6;
    case tok_gte:
        return 6;
    case tok_lt:
        return 6;
    case tok_lte:
        return 6;
    case tok_eq:
        return 6;
    case tok_neq:
        return 6;

    case tok_not:
        return 5;

    case tok_and:
        return 4;

    case tok_or:
        return 3;

    default:
        return -1;
    }
}
