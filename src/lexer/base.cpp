#include <algorithm>
#include <sstream>
#include <iostream>

#include "lexer.h"

// valid identifiers start with a-zA-Z_
bool is_valid_identifier_start(char character)
{
    return ('a' <= character && character <= 'z') ||
           ('A' <= character && character <= 'Z') ||
           character == '_';
}

// valid identifiers contain a-zA-Z0-9_
bool is_valid_identifier_body(char character)
{
    return ('a' <= character && character <= 'z') ||
           ('A' <= character && character <= 'Z') ||
           ('0' <= character && character <= '9') ||
           character == '_';
}

// valid numbers contain 0-9
bool is_valid_number(char character)
{
    return ('0' <= character && character <= '9');
}

bool has_even_number_of_quotations(const std::string &str)
{
    size_t single_quotation_count = static_cast<int>(std::count(str.begin(), str.end(), '\''));

    return single_quotation_count % 2 == 0;
}

bool has_even_number_of_brackets(const std::string &str)
{
    size_t left_bracket_count = static_cast<int>(std::count(str.begin(), str.end(), '{'));
    size_t right_bracket_count = static_cast<int>(std::count(str.begin(), str.end(), '}'));

    return left_bracket_count == right_bracket_count;
}

bool Lexer::valid_index() const { return current >= 0 && current < source.size(); }

const char Lexer::peek() const { return source[current]; }
const char Lexer::prev() const { return source[current - 1]; }
const char Lexer::next() const { return source[current + 1]; }
const char Lexer::advance()
{
    if (valid_index())
    {
        current++;

        column++;
    }

    return prev();
}

bool Lexer::check(char expected) const { return valid_index() && peek() == expected; }
bool Lexer::match(char expected)
{
    if (check(expected))
    {
        advance();
        return true;
    }

    return false;
}

void Lexer::add_token(TokenType type)
{
    Token token(type, Position(line, column), source.substr(start, current - start));
    tokens.push_back(token);
}

void Lexer::add_token(TokenType type, const std::string &lexeme)
{
    Token token(type, Position(line, column), lexeme);
    tokens.push_back(token);
}

void Lexer::skip_empty()
{
    while (valid_index())
    {
        char c = peek();

        switch (c)
        {
        // whitespaces, tabs
        case ' ':
        case '\t':
            advance();
            break;

        // CRLF, LF
        case '\r':
        case '\n':
            column = 0;
            line++;
            advance();
            break;

        // single-line comments
        case '/':
            if (next() == '/')
                while (!check('\n'))
                    advance();
            else
                return;
            break;

        default:
            return;
        }
    }
}

void Lexer::scan_token()
{
    skip_empty();
    start = current;

    if (!valid_index()) return;

    char c = advance();

    switch (c)
    {
    case ';':
        add_token(tok_delimiter);
        break;
    case '(':
        add_token(tok_open_paren);
        break;
    case ')':
        add_token(tok_close_paren);
        break;
    case '{':
        add_token(tok_open_brace);
        break;
    case '}':
        add_token(tok_close_brace);
        break;
    case '<':
        add_token(match('=') ? tok_lte : tok_lt);
        break;
    case '>':
        add_token(match('=') ? tok_gte : tok_gt);
        break;
    case '+':
        add_token(tok_plus);
        break;
    case '-':
        add_token(tok_minus);
        break;
    case '*':
        add_token(match('*') ? tok_exponentiation : tok_star);
        break;
    case '/':
        add_token(tok_slash);
        break;
    case '~':
        add_token(tok_tilde);
        break;
    case '=':
        add_token(match('=') ? tok_eq : tok_assignment);
        break;
    case '!':
        add_token(match('=') ? tok_neq : tok_not);
        break;
    case '"':
        scan_string();
        break;

    default:
        if (std::isdigit(c)) scan_number();
        else if (std::isalpha(c) || c == '_') scan_identifier();
        else
        {
            add_token(tok_invalid, std::string(1, c));
        }
    }
}

void Lexer::scan_identifier()
{
    while (std::isalnum(peek()) || peek() == '_') advance();

    std::string lexeme = source.substr(start, current - start);

    if (keyword_to_token.contains(lexeme))
    {
        TokenType type = keyword_to_token.at(lexeme);
        add_token(type);
    }
    else
        add_token(tok_identifier);
}

void Lexer::scan_string()
{
    while (valid_index() && peek() != '"')
    {
        if (peek() == '\n')
        {
            column = 0;
            line++;
        }

        advance();
    }

    if (!valid_index()) {
        add_token(tok_invalid, "Unterminated string.");
        return;
    }

    add_token(tok_string);
}

void Lexer::scan_number()
{
    while (valid_index() && std::isdigit(peek()))
        advance();

    // decimal separator
    if (peek() == '.' && std::isdigit(next()))
    {
        advance();
        while (std::isdigit(peek())) advance();
    }

    add_token(tok_number);
}


std::vector<Token> Lexer::tokenize()
{
    while (valid_index())
        scan_token();

    return tokens;
}
