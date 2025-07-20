#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <ostream>
#include <unordered_map>

#include "operators.h"
#include "position.h"
#include "utils.h"

enum TokenType
{
    // structure
    tok_invalid = -1,

    tok_delimiter = 0,
    tok_comma,
    tok_comment,

    tok_open_paren,
    tok_close_paren,
    tok_open_brace,
    tok_close_brace,

    // literals
    tok_number,
    tok_string,
    tok_true,
    tok_false,

    // primary
    tok_identifier,
    
    // operators
    tok_assignment,
    tok_relationship,

    tok_plus,
    tok_minus,
    tok_star,
    tok_slash,
    tok_exponentiation,

    tok_caret,
    tok_tilde,
    tok_ampersand,
    tok_pipe,
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
    tok_fn,
    tok_return,
    tok_if,
    tok_else,
    tok_while,
    tok_extern
};

#define TOKEN_TO_STR_MAPPINGS \
    ROW(tok_invalid, "tok_invalid") \
    ROW(tok_delimiter, "tok_delimiter") \
    ROW(tok_comma, "tok_comma") \
    ROW(tok_comment, "tok_comment") \
    ROW(tok_open_paren, "tok_open_paren") \
    ROW(tok_close_paren, "tok_close_paren") \
    ROW(tok_open_brace, "tok_open_brace") \
    ROW(tok_close_brace, "tok_close_brace") \
    ROW(tok_number, "tok_number") \
    ROW(tok_string, "tok_string") \
    ROW(tok_true, "tok_true") \
    ROW(tok_false, "tok_false") \
    ROW(tok_identifier, "tok_identifier") \
    ROW(tok_assignment, "tok_assignment") \
    ROW(tok_relationship, "tok_relationship") \
    ROW(tok_plus, "tok_plus") \
    ROW(tok_minus, "tok_minus") \
    ROW(tok_star, "tok_star") \
    ROW(tok_slash, "tok_slash") \
    ROW(tok_exponentiation, "tok_exponentiation") \
    ROW(tok_caret, "tok_caret") \
    ROW(tok_tilde, "tok_tilde") \
    ROW(tok_ampersand, "tok_ampersand") \
    ROW(tok_pipe, "tok_pipe") \
    ROW(tok_not, "tok_not") \
    ROW(tok_and, "tok_and") \
    ROW(tok_or, "tok_or") \
    ROW(tok_gt, "tok_gt") \
    ROW(tok_gte, "tok_gte") \
    ROW(tok_lt, "tok_lt") \
    ROW(tok_lte, "tok_lte") \
    ROW(tok_eq, "tok_eq") \
    ROW(tok_neq, "tok_neq") \
    ROW(tok_fn, "tok_fn") \
    ROW(tok_return, "tok_return") \
    ROW(tok_if, "tok_if") \
    ROW(tok_else, "tok_else") \
    ROW(tok_while, "tok_while") \
    ROW(tok_extern, "tok_extern")

#define KEYWORD_MAPPINGS \
    ROW(tok_true, "true") \
    ROW(tok_false, "false") \
    ROW(tok_not, "not") \
    ROW(tok_and, "and") \
    ROW(tok_or, "or") \
    ROW(tok_fn, "fn") \
    ROW(tok_return, "return") \
    ROW(tok_if, "if") \
    ROW(tok_else, "else") \
    ROW(tok_while, "while") \
    ROW(tok_extern, "extern")

#define BINARY_OPERATOR_MAPPINGS \
    ROW(tok_plus, binop_add) \
    ROW(tok_minus, binop_sub) \
    ROW(tok_star, binop_mul) \
    ROW(tok_slash, binop_div) \
    ROW(tok_exponentiation, binop_exp) \
    ROW(tok_and, binop_and) \
    ROW(tok_or, binop_or) \
    ROW(tok_caret, binop_bit_xor) \
    ROW(tok_ampersand, binop_bit_and) \
    ROW(tok_pipe, binop_bit_or) \
    ROW(tok_gt, binop_gt) \
    ROW(tok_gte, binop_gte) \
    ROW(tok_lt, binop_lt) \
    ROW(tok_lte, binop_gte) \
    ROW(tok_eq, binop_eq) \
    ROW(tok_neq, binop_neq)

#define UNARY_OPERATOR_MAPPINGS \
    ROW(tok_plus, unary_add) \
    ROW(tok_minus, unary_sub) \
    ROW(tok_not, unary_not) \
    ROW(tok_tilde, unary_bit_not)


#define ROW(tok, str) { tok, str },
const std::unordered_map<TokenType, std::string> token_to_str = {
    TOKEN_TO_STR_MAPPINGS
};
#undef ROW

#define ROW(tok, str) { str, tok },
const std::unordered_map<std::string, TokenType> str_to_token = {
    TOKEN_TO_STR_MAPPINGS
};
#undef ROW

#define ROW(tok, str) { str, tok },
const std::unordered_map<std::string, TokenType> keyword_to_token = {
    KEYWORD_MAPPINGS
};
#undef ROW

#define ROW(tok, op) { tok, op },
const std::unordered_map<TokenType, BinaryOpType> token_to_binary_op = {
    BINARY_OPERATOR_MAPPINGS
};
#undef ROW

#define ROW(tok, op) { tok, op },
const std::unordered_map<TokenType, UnaryOpType> token_to_unary_op = {
    UNARY_OPERATOR_MAPPINGS
};
#undef ROW


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
        out << "\'" << to_escaped_string(obj.lexeme) << "\' -> " << token_to_str.at(obj.type);
        return out;
    }
};


int token_op_to_precedence(TokenType type);


#endif