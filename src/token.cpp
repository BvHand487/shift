#include <cstdlib>
#include <string>

#include "token.h"
#include "position.h"


Token::Token(TokenType type, Position position, std::string lexeme)
{
    this->type = type;
    this->position = position;
    this->lexeme = lexeme;
}