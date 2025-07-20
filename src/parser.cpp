#include <sstream>
#include <iostream>

#include "parser.h"

bool Parser::valid_index() const { return idx >= 0 && idx < tokens.size(); }

const Token &Parser::peek() const { return this->tokens[idx]; }
const Token &Parser::prev() const { return this->tokens[idx - 1]; }
const Token &Parser::next() const { return this->tokens[idx + 1]; }
const Token &Parser::advance() {
    if (valid_index())
        idx++;

    return prev();
}

bool Parser::check(TokenType type) const { return valid_index() && peek().type == type; }
bool Parser::match(TokenType type) {
    if (check(type))
    {
        advance();
        return true;
    }

    return false;
}
const Token &Parser::consume(TokenType expected, const std::string &error) {
    if (check(expected))
        return advance();

    Position token_position = peek().position;
    std::string message = std::format("Parsing error at (line={}, col={}): {}", token_position.line, token_position.column, error);

    throw std::runtime_error(message);
}


std::unique_ptr<Declaration> Parser::parse_declaration()
{
    if (match(tok_fn))
        return parse_function();

    throw std::runtime_error("Expected declaration (e.g. 'fn')");
}

std::unique_ptr<Declaration> Parser::parse_function()
{
    auto proto = parse_prototype();

    // function definition
    if (check(tok_open_brace))
    {
        auto body = parse_block();
        return std::make_unique<Definition>(std::move(proto), std::move(body));
    }

    consume(tok_delimiter, "Expected ';' after function prototype");

    return std::move(proto);
}

std::unique_ptr<Prototype> Parser::parse_prototype()
{
    std::string name = consume(tok_identifier, "Expected function name").lexeme;

    consume(tok_open_paren, "Expected '(' after function name");

    std::vector<std::unique_ptr<Variable>> args;
    if (!check(tok_close_paren))
    {
        do
        {
            auto arg = parse_variable();
            args.push_back(std::move(arg));
        }
        while (match(tok_comma));
    }

    consume(tok_close_paren, "Expected ')' after parameters");

    return std::make_unique<Prototype>(name, std::move(args));
}

std::unique_ptr<Block> Parser::parse_block()
{
    consume(tok_open_brace, "Expected '{' before block");

    std::vector<std::unique_ptr<Statement>> statements;
    while (!check(tok_close_brace) && valid_index())
    {
        statements.push_back(parse_statement());
    }

    consume(tok_close_brace, "Expected '}' after block");

    return std::make_unique<Block>(std::move(statements));
}

std::unique_ptr<Statement> Parser::parse_statement()
{
    if (check(tok_identifier) && next().type == tok_assignment)
        return parse_assignment();

    if (match(tok_return))
        return parse_return_stmt();

    if (match(tok_if))
        return parse_if_stmt();

    if (match(tok_while))
        return parse_while_stmt();

    if (check(tok_open_brace))
        return parse_block();

    auto expr = parse_expression();
    consume(tok_delimiter, "Expected ';' after expression.");
    return std::make_unique<ExprStatement>(std::move(expr));
}

std::unique_ptr<Assignment> Parser::parse_assignment()
{
    std::unique_ptr<Variable> var = parse_variable();
    consume(tok_assignment, "Expected '=' after assignment identifier");

    std::unique_ptr<Expr> expr = parse_expression();
    
    consume(tok_delimiter, "Expected ';' after assignment");

    return std::make_unique<Assignment>(std::move(var), std::move(expr));
}

std::unique_ptr<Return> Parser::parse_return_stmt()
{
    std::unique_ptr<Expr> expression = parse_expression();
    consume(tok_delimiter, "Expected ';' after return");

    return std::make_unique<Return>(std::move(expression));
}

std::unique_ptr<If> Parser::parse_if_stmt()
{
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Block> then_block;
    std::unique_ptr<Block> else_block = nullptr;
    
    consume(tok_open_paren, "Expected '(' before 'if' condition");
    condition = parse_expression();
    consume(tok_close_paren, "Expected ')' after 'if' condition");

    consume(tok_open_brace, "Expected '{' before 'if' body");
    then_block = parse_block();

    if (match(tok_else))
    {
        consume(tok_open_brace, "Expected '{' before 'else' body");
        else_block = parse_block();
    }

    return std::make_unique<If>(
        std::move(condition),
        std::move(then_block),
        std::move(else_block)
    );
}

std::unique_ptr<While> Parser::parse_while_stmt()
{
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Block> body;
    
    consume(tok_open_paren, "Expected '(' before 'while' condition");
    condition = parse_expression();
    consume(tok_close_paren, "Expected ')' after 'while' condition");

    consume(tok_open_brace, "Expected '{' before 'while' body");
    body = parse_block();

    return std::make_unique<While>(
        std::move(condition),
        std::move(body)
    );
}


std::unique_ptr<Expr> Parser::parse_expression(int precedence)
{
    auto lhs = parse_unary_expr();

    while (valid_index())
    {
        int current_prec = token_op_to_precedence(peek().type);
        if (current_prec < precedence) break;

        const Token& op = advance();
        auto rhs = parse_expression(current_prec + 1);
        lhs = std::make_unique<BinaryOp>(token_to_binary_op.at(op.type), std::move(lhs), std::move(rhs));
    }

    return lhs;
}

std::unique_ptr<Expr> Parser::parse_primary()
{
    if (match(tok_number))
        return std::make_unique<Number>(std::stod(prev().lexeme));

    if (match(tok_string))
        return std::make_unique<String>(prev().lexeme);

    if (match(tok_true))
        return std::make_unique<Boolean>(true);

    if (match(tok_false))
        return std::make_unique<Boolean>(false);
    
    if (check(tok_identifier))
    {
        // function call expr
        if (next().type == tok_open_paren)
            return parse_call_expr();
        
        // variable expr
        return parse_variable();
    }

    if (match(tok_open_paren)) {
        auto expr = parse_expression();
        consume(tok_close_paren, "Expected ')' after expression");
        return expr;
    }

    throw std::runtime_error("Expected expression.");
}

std::unique_ptr<Expr> Parser::parse_unary_expr()
{
    if (match(tok_plus))
        return std::make_unique<UnaryOp>(unary_add, parse_unary_expr());

    if (match(tok_minus))
        return std::make_unique<UnaryOp>(unary_sub, parse_unary_expr());

    if (match(tok_not))
        return std::make_unique<UnaryOp>(unary_not, parse_unary_expr());

    if (match(tok_tilde))
        return std::make_unique<UnaryOp>(unary_bit_not, parse_unary_expr());

    return parse_primary();
}

std::unique_ptr<CallExpr> Parser::parse_call_expr()
{
    consume(tok_identifier, "Expected identifier");
    std::string name = prev().lexeme;
    
    consume(tok_open_paren, "Expected '(' before function call args");

    std::vector<std::unique_ptr<Expr>> args;
    if (!check(tok_close_paren))
    {
        do
        {
            auto expr = parse_expression();
            args.push_back(std::move(expr));
        }
        while (match(tok_comma));
    }

    consume(tok_close_paren, "Expected ')' after function call args");
    
    return std::make_unique<CallExpr>(name, std::move(args));
}

std::unique_ptr<Variable> Parser::parse_variable()
{
    consume(tok_identifier, "Expected identifier");
    std::string name = prev().lexeme;

    return std::make_unique<Variable>(name);
}

std::vector<std::unique_ptr<Declaration>> Parser::parse()
{
    std::vector<std::unique_ptr<Declaration>> declarations;

    while (this->valid_index())
    {
        auto decl = parse_declaration();

        if (!decl)
            break;

        declarations.push_back(std::move(decl));
    }

    return std::move(declarations);
}