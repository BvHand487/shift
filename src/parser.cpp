#include <sstream>
#include <iostream>

#include "parser.h"


UnaryOps token_to_unary_op(TokenType type)
{
    switch (type)
    {
        case tok_plus: return UnaryOps::Addition;
        case tok_minus: return UnaryOps::Subtraction;
        case tok_not: return UnaryOps::Not;
        case tok_bitwise_not: return UnaryOps::BitwiseNot;

        default:
            return UnaryOps::None;
    }
}

BinaryOps token_to_binary_op(TokenType type)
{
    switch (type)
    {
        case tok_plus: return BinaryOps::Addition;
        case tok_minus: return BinaryOps::Subtraction;
        case tok_multiplication: return BinaryOps::Multiplication;
        case tok_division: return BinaryOps::Division;

        default:
            return BinaryOps::None;
    }
}

int token_op_to_precedence(TokenType type)
{
    switch (type)
    {
        case tok_left_parentheses: return 15;
        case tok_right_parentheses: return 15;

        case tok_exponentiation: return 14;

        case tok_multiplication: return 12;
        case tok_division: return 12;

        case tok_plus: return 11;
        case tok_minus: return 11;

        case tok_bitwise_and: return 9;

        case tok_bitwise_xor: return 8;

        case tok_bitwise_or: return 7;

        case tok_gt: return 6;
        case tok_gte: return 6;
        case tok_lt: return 6;
        case tok_lte: return 6;
        case tok_eq: return 6;
        case tok_neq: return 6;

        case tok_not: return 5;
        
        case tok_and: return 4;

        case tok_or: return 3;
        
        default:
            return -1;
    }
}

int token_is_binary_op(TokenType type)
{
    switch (type)
    {
        case tok_exponentiation:
        case tok_multiplication:
        case tok_division:
        case tok_plus:
        case tok_minus:
        case tok_bitwise_and:
        case tok_bitwise_xor:
        case tok_bitwise_or:
        case tok_gt:
        case tok_gte:
        case tok_lt:
        case tok_lte:
        case tok_eq:
        case tok_neq:
        case tok_not:
        case tok_and:
        case tok_or:
            return true;
        default:
            return false;
    }
}

bool token_match(const std::vector<Token>& tokens, size_t& idx, TokenType type)
{
    return (idx < tokens.size() && tokens[idx].type == type);
}

std::unique_ptr<Expr> parse_main(const std::vector<Token>& tokens, size_t& idx)
{
    // variable for now
    if (token_match(tokens, idx, tok_identifier))
    {
        std::string name = tokens[idx].lexeme;
        idx++;

        std::unique_ptr<Variable> variable = std::make_unique<Variable>(name);
        return variable;
    }

    // number literal
    if (token_match(tokens, idx, tok_number))
    {
        double value = std::stod(tokens[idx].lexeme);
        idx++;

        std::unique_ptr<Number> literal = std::make_unique<Number>(value);
        return literal;
    }

    // boolean literal
    if (token_match(tokens, idx, tok_boolean))
    {
        bool value;
        std::istringstream(tokens[idx].lexeme) >> std::boolalpha >> value;
        idx++;

        std::unique_ptr<Boolean> literal = std::make_unique<Boolean>(value);
        return literal;
    }

    // string literal
    if (token_match(tokens, idx, tok_string))
    {
        std::string value = "None";

        if (tokens[idx].lexeme[0] == '\'' && tokens[idx].lexeme[tokens[idx].lexeme.length() - 1] == '\'')
            value = tokens[idx].lexeme.substr(1, tokens[idx].lexeme.length() - 2);
        
        idx++;

        std::unique_ptr<String> literal = std::make_unique<String>(value);
        return literal;
    }

    // parentheses
    if (token_match(tokens, idx, tok_left_parentheses))
    {
        idx++;

        std::unique_ptr<Expr> expr = parse_expr(tokens, idx);

        if (!token_match(tokens, idx, tok_right_parentheses))
        {
            std::cout << "Null is returning1\n";    
            return nullptr;
        }

        idx++;

        return expr;
    }

    std::cout << "Null is returning2\n";
    return nullptr;
}


std::unique_ptr<Expr> parse_unary(const std::vector<Token>& tokens, size_t& idx)
{
    // if valid unary op
    if (token_match(tokens, idx, tok_plus) || token_match(tokens, idx, tok_minus) || 
    token_match(tokens, idx, tok_not) || token_match(tokens, idx, tok_bitwise_not))
    {
        TokenType type = tokens[idx++].type;

        std::unique_ptr<Expr> rhs = parse_unary(tokens, idx);
        
        return std::make_unique<UnaryOp>(token_to_unary_op(type), std::move(rhs));
    }

    return parse_main(tokens, idx);
}


std::unique_ptr<Expr> parse_binary(int expr_precedence, std::unique_ptr<Expr> lhs, const std::vector<Token>& tokens, size_t& idx)
{
    while (idx < tokens.size())
    {
        if (!token_is_binary_op(tokens[idx].type))
            break;

        int prev_precedence = token_op_to_precedence(tokens[idx].type);
        if (prev_precedence < expr_precedence)
            break;

        TokenType type = tokens[idx++].type;
        auto rhs = parse_unary(tokens, idx);
        if (!rhs)
        {
            std::cout << "Null is returning3\n";
            return nullptr;
        }

        int next_precedence = 0;
        if (idx < tokens.size() && token_is_binary_op(tokens[idx].type))
            next_precedence = token_op_to_precedence(tokens[idx].type);

        if (prev_precedence < next_precedence)
        {
            rhs = parse_binary(prev_precedence + 1, std::move(rhs), tokens, idx);
            if (!rhs)
            {
                std::cout << "Null is returning3\n";
                return nullptr;
            }
        }

        lhs = std::make_unique<BinaryOp>(token_to_binary_op(type), std::move(lhs), std::move(rhs));
    }

    return lhs;
}

std::unique_ptr<Expr> parse_expr(const std::vector<Token>& tokens, size_t& idx)
{
    auto lhs = parse_unary(tokens, idx);
    return parse_binary(0, std::move(lhs), tokens, idx);
}

std::unique_ptr<ASTNode> parse_node(const std::vector<Token>& tokens, size_t& idx)
{
    if (idx >= tokens.size()) 
    {
        std::cout << "Null is returning3\n";
        return nullptr;
    }

    // skip redundant semi-colons
    if (token_match(tokens, idx, tok_delimiter))
    {
        while (token_match(tokens, idx, tok_delimiter)) idx++;
    }            

    // statements
    if (token_match(tokens, idx, tok_identifier))
    {
        // lhs expr
        std::unique_ptr<Expr> lhs(static_cast<Expr*>(parse_expr(tokens, idx).release()));
        if (!lhs) return nullptr;

        if (token_match(tokens, idx, tok_assignment))
        {
            idx++;

            // rhs expr
            std::unique_ptr<Expr> rhs(static_cast<Expr*>(parse_expr(tokens, idx).release()));
            if (!rhs) return nullptr;

            if (token_match(tokens, idx, tok_delimiter))
            {
                while (token_match(tokens, idx, tok_delimiter)) idx++;

                return std::make_unique<Assignment>(
                    std::unique_ptr<Variable>(static_cast<Variable*>(lhs.release())),
                    std::move(rhs)
                );
            }
        }
    }

    if (token_match(tokens, idx, tok_comment))
    {
        idx++;
    }

    return nullptr;
}


std::vector<std::unique_ptr<ASTNode>> parse(const std::vector<Token>& tokens)
{
    size_t idx = 0;
    std::vector<std::unique_ptr<ASTNode>> ast;

    while (idx < tokens.size())
    {
        std::unique_ptr<ASTNode> node = parse_node(tokens, idx);

        if (node == nullptr)
            continue;

        ast.push_back(std::move(node));
    }

    return ast;
}