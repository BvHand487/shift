#ifndef LEXER_H
#define LEXER_H

#include <cstdlib>
#include <vector>
#include <string>

#include "token.h"
#include "position.h"


std::vector<Token> tokenize(const std::string& input);


// valid identifiers start with a-zA-Z_
bool is_valid_identifier_start(char character);

// valid identifiers contain a-zA-Z0-9_
bool is_valid_identifier_body(char character);

// valid numbers contain 0-9
bool is_valid_number(char character);

bool has_even_number_of_quotations(const std::string& str);


#endif