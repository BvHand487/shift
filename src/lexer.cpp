#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

#include "lexer.h"
#include "position.h"


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


bool has_even_number_of_quotations(const std::string& str)
{
    size_t single_quotation_count = static_cast<int>(std::count(str.begin(), str.end(), '\''));
    
    return single_quotation_count % 2 == 0;
}

bool has_even_number_of_brackets(const std::string& str)
{
    size_t left_bracket_count = static_cast<int>(std::count(str.begin(), str.end(), '{'));
    size_t right_bracket_count = static_cast<int>(std::count(str.begin(), str.end(), '}'));

    return left_bracket_count == right_bracket_count;
}


std::vector<Token> tokenize(const std::string& input)
{
    std::vector<Token> tokens;

    std::string lexeme = "";
    char character;


    // run basic checks before tokenizing
    if (!has_even_number_of_quotations(input))
        throw std::runtime_error("Strings must have matching closing quotation marks.");

    if (!has_even_number_of_brackets(input))
        throw std::runtime_error("Statement bodies must have matching closing brackets.");


    for (int idx = 0; idx < input.length(); idx++)
    {
        Token token;
        lexeme = "";

        character = input[idx];

        lexeme += character;

        if (character == ' ' || character == '\n') continue;

        switch (character)
        {
            case ';':
                token = Token(tok_delimiter, Position(0, idx), lexeme); break;

            case '(':
                token = Token(tok_open_paren, Position(0, idx), lexeme); break;

            case ')':
                token = Token(tok_close_paren, Position(0, idx), lexeme); break;

            case '{':
                token = Token(tok_open_brace, Position(0, idx), lexeme); break;

            case '}':
                token = Token(tok_close_brace, Position(0, idx), lexeme); break;

            case '=':
                idx++;
                character = input[idx];

                if (character == '=')
                {
                    lexeme += character;
                    token = Token(tok_eq, Position(0, idx), lexeme);
                    break;
                }

                token = Token(tok_assignment, Position(0, idx), lexeme);
                idx--;
                break;

            case '+':
                token = Token(tok_plus, Position(0, idx), lexeme); break;

            case '-':
                idx++;
                character = input[idx];

                if (character == '>')
                {
                    lexeme += character;
                    token = Token(tok_relationship, Position(0, idx), lexeme);
                    break;
                }

                token = Token(tok_minus, Position(0, idx), lexeme);
                idx--;
                break;

            case '*':
                idx++;
                character = input[idx];

                if (character == '*')
                {
                    lexeme += character;
                    token = Token(tok_exponentiation, Position(0, idx), lexeme);
                    break;
                }

                token = Token(tok_star, Position(0, idx), lexeme);
                idx--;
                break;

            case '/':
                idx++;
                character = input[idx];

                if (character == '/')
                {
                    while (character != '\n' && idx < input.length())
                    {
                        lexeme += character;
                        idx++;
                        character = input[idx];
                    }

                    token = Token(tok_comment, Position(0, idx), lexeme);
                    break;
                }

                token = Token(tok_slash, Position(0, idx), lexeme);
                idx--;
                break;

            case '&':
                idx++;
                character = input[idx];

                if (character == '&')
                {
                    lexeme += character;
                    token = Token(tok_and, Position(0, idx), lexeme);
                    break;
                }

                token = Token(tok_ampersand, Position(0, idx), lexeme);
                idx--;
                break;

            case '|':
                idx++;
                character = input[idx];

                if (character == '|')
                {
                    lexeme += character;
                    token = Token(tok_or, Position(0, idx), lexeme);
                    break;
                }

                token = Token(tok_pipe, Position(0, idx), lexeme);
                idx--;
                break;

            case '!':
                idx++;
                character = input[idx];

                if (character == '=')
                {
                    lexeme += character;
                    token = Token(tok_neq, Position(0, idx), lexeme);
                    break;
                }

                token = Token(tok_not, Position(0, idx), lexeme);
                idx--;
                break;

            case '~':
                token = Token(tok_tilde, Position(0, idx), lexeme); break;

            case '^':
                token = Token(tok_caret, Position(0, idx), lexeme); break;

            case '<':
                idx++;
                character = input[idx];

                if (character == '=')
                {
                    lexeme += character;
                    token = Token(tok_lte, Position(0, idx), lexeme);
                    break;
                }

                token = Token(tok_lt, Position(0, idx), lexeme);
                idx--;
                break;

            case '>':
                idx++;
                character = input[idx];

                if (character == '=')
                {
                    lexeme += character;
                    token = Token(tok_gte, Position(0, idx), lexeme);
                    break;
                }

                token = Token(tok_gt, Position(0, idx), lexeme);
                idx--;
                break;

            default:
                // identifiers
                if (is_valid_identifier_start(character))
                {
                    idx++;
                    character = input[idx];
                    
                    while (is_valid_identifier_body(character))
                    {
                        lexeme += character;
                        idx++;
                        character = input[idx];
                    }

                    // bool literal
                    if (lexeme == "true" || lexeme == "false")
                        token = Token(tok_boolean, Position(0, idx), lexeme);
                    // if keyword
                    else if (lexeme == "if")
                        token = Token(tok_if, Position(0, idx), lexeme);
                    // else keyword
                    else if (lexeme == "else")
                        token = Token(tok_else, Position(0, idx), lexeme);
                    // while keyword
                    else if (lexeme == "while")
                        token = Token(tok_while, Position(0, idx), lexeme);
                    // and
                    else if (lexeme == "and")
                        token = Token(tok_and, Position(0, idx), lexeme);
                    // or
                    else if (lexeme == "or")
                        token = Token(tok_or, Position(0, idx), lexeme);
                    // not
                    else if (lexeme == "not")
                        token = Token(tok_not, Position(0, idx), lexeme);
                    // identifier
                    else
                        token = Token(tok_identifier, Position(0, idx), lexeme);

                    idx--;
                }

                // literals
                else if (is_valid_number(character))
                {
                    bool has_decimal_point = false;

                    idx++;
                    character = input[idx];
                    
                    while (is_valid_number(character) || (character == '.' && has_decimal_point == false))
                    {
                        if (character == '.')
                            has_decimal_point = true;

                        lexeme += character;
                        idx++;
                        character = input[idx];
                    }

                    token = Token(tok_number, Position(0, idx), lexeme);
                    idx--;
                }

                else if (character == '\'')
                {
                    idx++;
                    character = input[idx];
                    
                    while (character != '\'')
                    {
                        lexeme += character;
                        idx++;
                        character = input[idx];
                    }

                    lexeme += character;
                    token = Token(tok_string, Position(0, idx), lexeme);
                }

                else
                {
                    token = Token(tok_invalid, Position(0, idx), lexeme);
                }
        }

        tokens.push_back(token);
    }

    return tokens;
}

