#include <sstream>
#include <iostream>

#include "parser.h"

UnaryOps token_to_unary_op(TokenType type)
{
    switch (type)
    {
    case tok_plus:
        return UnaryOps::Addition;
    case tok_minus:
        return UnaryOps::Subtraction;
    case tok_not:
        return UnaryOps::Not;
    case tok_tilde:
        return UnaryOps::BitwiseNot;

    default:
        return UnaryOps::None;
    }
}

BinaryOps token_to_binary_op(TokenType type)
{
    switch (type)
    {
    // Arithmetic
    case tok_plus:
        return BinaryOps::Addition;
    case tok_minus:
        return BinaryOps::Subtraction;
    case tok_star:
        return BinaryOps::Multiplication;
    case tok_slash:
        return BinaryOps::Division;
    case tok_exponentiation:
        return BinaryOps::Exponentiation;

    // Logical
    case tok_and:
        return BinaryOps::And;
    case tok_or:
        return BinaryOps::Or;

    // Bitwise
    case tok_caret:
        return BinaryOps::BitXor;
    case tok_ampersand:
        return BinaryOps::BitAnd;
    case tok_pipe:
        return BinaryOps::BitOr;

    // Comparison
    case tok_gt:
        return BinaryOps::Greater;
    case tok_gte:
        return BinaryOps::GreaterEqual;
    case tok_lt:
        return BinaryOps::Less;
    case tok_lte:
        return BinaryOps::LessEqual;
    case tok_eq:
        return BinaryOps::Equal;
    case tok_neq:
        return BinaryOps::NotEqual;

    default:
        return BinaryOps::None;
    }
}

int token_op_to_precedence(TokenType type)
{
    switch (type)
    {
    case tok_open_paren:
        return 15;
    case tok_close_paren:
        return 15;

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

int token_is_binary_op(TokenType type)
{
    switch (type)
    {
    case tok_exponentiation:
    case tok_star:
    case tok_slash:
    case tok_plus:
    case tok_minus:
    case tok_ampersand:
    case tok_caret:
    case tok_pipe:
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

bool token_match(const std::vector<Token> &tokens, const size_t &idx, TokenType type)
{
    return (idx < tokens.size() && tokens[idx].type == type);
}

std::unique_ptr<Expr> parse_main(const std::vector<Token> &tokens, size_t &idx)
{
    // variable | function call
    if (token_match(tokens, idx, tok_identifier))
    {
        std::string name = tokens[idx].lexeme;
        idx++;

        // function call
        if (token_match(tokens, idx, tok_open_paren))
        {
            if (!token_match(tokens, idx, tok_open_paren))
                throw std::runtime_error("Expected '(' after function identifer");
            idx++;

            // args
            std::vector<std::unique_ptr<Expr>> args;
            while (!token_match(tokens, idx, tok_close_paren))
            {
                args.push_back(std::move(parse_expr(tokens, idx)));

                if (token_match(tokens, idx, tok_close_paren))
                    break;

                if (!token_match(tokens, idx, tok_comma))
                    throw std::runtime_error("Expected ',' between args of function call");
                idx++;
            }
            idx++;

            while (token_match(tokens, idx, tok_delimiter))
                idx++;

            return std::make_unique<Call>(
                name,
                std::move(args));
        }
        else
        {
            std::unique_ptr<Variable> variable = std::make_unique<Variable>(name);
            return variable;
        }
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
    if (token_match(tokens, idx, tok_true) || token_match(tokens, idx, tok_false))
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
    if (token_match(tokens, idx, tok_open_paren))
    {
        idx++;

        std::unique_ptr<Expr> expr = parse_expr(tokens, idx);

        if (!token_match(tokens, idx, tok_close_paren))
            return nullptr;

        idx++;

        return expr;
    }

    return nullptr;
}

std::unique_ptr<Expr> parse_unary(const std::vector<Token> &tokens, size_t &idx)
{
    // if valid unary op
    if (token_match(tokens, idx, tok_plus) || token_match(tokens, idx, tok_minus) ||
        token_match(tokens, idx, tok_not) || token_match(tokens, idx, tok_tilde))
    {
        TokenType type = tokens[idx++].type;

        std::unique_ptr<Expr> rhs = parse_unary(tokens, idx);

        return std::make_unique<UnaryOp>(token_to_unary_op(type), std::move(rhs));
    }

    return parse_main(tokens, idx);
}

std::unique_ptr<Expr> parse_binary(int expr_precedence, std::unique_ptr<Expr> lhs, const std::vector<Token> &tokens, size_t &idx)
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
            return nullptr;

        int next_precedence = 0;
        if (idx < tokens.size() && token_is_binary_op(tokens[idx].type))
            next_precedence = token_op_to_precedence(tokens[idx].type);

        if (prev_precedence < next_precedence)
        {
            rhs = parse_binary(prev_precedence + 1, std::move(rhs), tokens, idx);
            if (!rhs)
                return nullptr;
        }

        lhs = std::make_unique<BinaryOp>(token_to_binary_op(type), std::move(lhs), std::move(rhs));
    }

    return lhs;
}

std::unique_ptr<Expr> parse_expr(const std::vector<Token> &tokens, size_t &idx)
{
    auto lhs = parse_unary(tokens, idx);
    return parse_binary(0, std::move(lhs), tokens, idx);
}

std::unique_ptr<ASTNode> parse_statement(const std::vector<Token> &tokens, size_t &idx)
{
    if (idx >= tokens.size())
        throw std::runtime_error("Parsing index surpassed token size.");

    // skip redundant semi-colons
    if (token_match(tokens, idx, tok_delimiter))
        while (token_match(tokens, idx, tok_delimiter))
            idx++;

    // skip comments
    if (token_match(tokens, idx, tok_comment))
        while (token_match(tokens, idx, tok_comment))
            idx++;

    // assignment
    if (token_match(tokens, idx, tok_identifier))
    {
        // lhs expr
        std::unique_ptr<Expr> lhs(parse_expr(tokens, idx));

        if (token_match(tokens, idx, tok_assignment))
        {
            idx++;

            Variable *var = dynamic_cast<Variable *>(lhs.get());
            if (var == nullptr)
                throw std::runtime_error("Left side of assignment must be a variable.");

            // rhs expr
            std::unique_ptr<Expr> rhs(static_cast<Expr *>(parse_expr(tokens, idx).release()));
            if (!rhs)
                return nullptr;

            while (token_match(tokens, idx, tok_delimiter))
                    idx++;

            return std::make_unique<Assignment>(
                std::unique_ptr<Variable>(static_cast<Variable *>(lhs.release())),
                std::move(rhs));
        }
    }

    // function definition
    if (token_match(tokens, idx, tok_fn))
    {
        idx++;

        if (!token_match(tokens, idx, tok_identifier))
            throw std::runtime_error("Missing function identifier");
        std::string identifier = tokens[idx].lexeme;
        idx++;

        if (!token_match(tokens, idx, tok_open_paren))
            throw std::runtime_error("Expected '(' after function identifer");
        idx++;

        // args
        std::vector<std::unique_ptr<Variable>> args;
        while (!token_match(tokens, idx, tok_close_paren))
        {
            std::string name = tokens[idx].lexeme;
            std::unique_ptr<Variable> arg = std::make_unique<Variable>(name);
            args.push_back(std::move(arg));
            idx++;

            if (token_match(tokens, idx, tok_close_paren))
                break;

            if (!token_match(tokens, idx, tok_comma))
                throw std::runtime_error("Expected ',' between function args");
            idx++;
        }
        idx++;

        if (!token_match(tokens, idx, tok_open_brace))
            throw std::runtime_error("Expected '{' before function body");
        idx++;

        // body statements
        std::vector<std::unique_ptr<Statement>> body;
        while (!token_match(tokens, idx, tok_close_brace))
        {
            std::unique_ptr<Statement> statement(static_cast<Statement *>(parse_statement(tokens, idx).release()));
            body.push_back(std::move(statement));
        }
        idx++;

        return std::make_unique<Function>(
            identifier,
            std::move(args),
            std::move(body));
    }

    // function return
    if (token_match(tokens, idx, tok_return))
    {
        idx++;

        std::unique_ptr<Expr> retVal(static_cast<Expr *>(parse_expr(tokens, idx).release()));
        if (!retVal)
            return nullptr;
            
        while (token_match(tokens, idx, tok_delimiter))
            idx++;

        return std::make_unique<Return>(std::move(retVal));
    }

    // if
    if (token_match(tokens, idx, tok_if))
    {
        idx++;

        if (!token_match(tokens, idx, tok_open_paren))
            throw std::runtime_error("Expected '(' after 'if'");
        idx++;

        // condition expr
        std::unique_ptr<Expr> cond(static_cast<Expr *>(parse_expr(tokens, idx).release()));
        if (!cond)
            return nullptr;

        if (!token_match(tokens, idx, tok_close_paren))
            throw std::runtime_error("Expected ')' after 'if' condition");
        idx++;

        if (!token_match(tokens, idx, tok_open_brace))
            throw std::runtime_error("Expected '{' before 'if' body");
        idx++;

        // then statements
        std::vector<std::unique_ptr<Statement>> then_branch;
        while (!token_match(tokens, idx, tok_close_brace))
        {
            std::unique_ptr<Statement> statement(static_cast<Statement *>(parse_statement(tokens, idx).release()));
            then_branch.push_back(std::move(statement));
        }

        idx++;

        // optional else
        // else statements
        std::vector<std::unique_ptr<Statement>> else_branch;
        if (token_match(tokens, idx, tok_else))
        {
            idx++;

            if (!token_match(tokens, idx, tok_open_brace))
                throw std::runtime_error("Expected '{' before 'else' body");
            idx++;

            while (!token_match(tokens, idx, tok_close_brace))
            {
                std::unique_ptr<Statement> statement(static_cast<Statement *>(parse_statement(tokens, idx).release()));
                else_branch.push_back(std::move(statement));
            }

            idx++;
        }

        return std::make_unique<IfStatement>(
            std::unique_ptr<Expr>(static_cast<Expr *>(cond.release())),
            std::move(then_branch),
            std::move(else_branch));
    }

    // while
    if (token_match(tokens, idx, tok_while))
    {
        idx++;

        if (!token_match(tokens, idx, tok_open_paren))
            throw std::runtime_error("Expected '(' after 'while'");
        idx++;

        // condition expr
        std::unique_ptr<Expr> cond(static_cast<Expr *>(parse_expr(tokens, idx).release()));
        if (!cond)
            return nullptr;

        if (!token_match(tokens, idx, tok_close_paren))
            throw std::runtime_error("Expected ')' after 'while' condition");
        idx++;

        if (!token_match(tokens, idx, tok_open_brace))
            throw std::runtime_error("Expected '{' before 'while' body");
        idx++;

        // body statements
        std::vector<std::unique_ptr<Statement>> body;
        while (!token_match(tokens, idx, tok_close_brace))
        {
            std::unique_ptr<Statement> statement(static_cast<Statement *>(parse_statement(tokens, idx).release()));
            body.push_back(std::move(statement));
        }
        idx++;

        return std::make_unique<WhileStatement>(
            std::unique_ptr<Expr>(static_cast<Expr *>(cond.release())),
            std::move(body));
    }

    throw std::runtime_error("Unexpected expression or invalid statement.");
}

std::vector<std::unique_ptr<ASTNode>> parse(const std::vector<Token> &tokens)
{
    size_t idx = 0;
    std::vector<std::unique_ptr<ASTNode>> ast;

    while (idx < tokens.size())
    {
        std::unique_ptr<ASTNode> node = parse_statement(tokens, idx);

        if (node == nullptr)
            continue;

        ast.push_back(std::move(node));
    }

    return ast;
}