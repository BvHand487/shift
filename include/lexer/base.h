#ifndef LEXER_BASE_H
#define LEXER_BASE_H

#include <memory>
#include <vector>

#include "lexer/token.h"


// valid identifiers start with a-zA-Z_
bool is_valid_identifier_start(char character);

// valid identifiers contain a-zA-Z0-9_
bool is_valid_identifier_body(char character);

// valid numbers contain 0-9
bool is_valid_number(char character);

bool has_even_number_of_quotations(const std::string& str);


class Lexer
{
    const std::string source;
    std::vector<Token> tokens;
    size_t start = 0;
    size_t current = 0;
    size_t line = 1;
    size_t column = 1;

    bool valid_index() const;

    const char peek() const;
    const char prev() const;
    const char next() const;
    const char advance();

    bool check(char expected) const;
    bool match(char expected);

public:
    explicit Lexer(const std::string &source) : source(source) {}
    std::vector<Token> tokenize();

    void skip_empty();  // handles whitespaces, newlines and comments
    void scan_token();
    void scan_identifier();
    void scan_string();
    void scan_number();
    void add_token(TokenType type);
    void add_token(TokenType type, const std::string &text);
};

#endif