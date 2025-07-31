#ifndef PARSER_BASE_H
#define PARSER_BASE_H

#include <memory>
#include <vector>

#include "ast.h"
#include "lexer.h"

using namespace ast;

class Parser
{
    const std::vector<Token>& tokens;
    size_t idx = 0;

    bool valid_index() const;

    const Token& peek() const;
    const Token& prev() const;
    const Token& next() const;
    const Token& advance();

    bool check(TokenType type) const;
    bool match(TokenType type);
    const Token& consume(TokenType expected, const std::string& error);

public:
    explicit Parser(const std::vector<Token>& tokens) : tokens(tokens) {}
    std::vector<std::unique_ptr<Declaration>> parse();

    // Expressions
    std::unique_ptr<Expr> parse_expression(int precedence = 0);
    std::unique_ptr<Expr> parse_primary();
    std::unique_ptr<Expr> parse_unary_expr();
    std::unique_ptr<CallExpr> parse_call_expr();
    std::unique_ptr<Variable> parse_variable();

    // Statements
    std::unique_ptr<Statement> parse_statement();
    std::unique_ptr<Assignment> parse_assignment();
    std::unique_ptr<Return> parse_return_stmt();
    std::unique_ptr<If> parse_if_stmt();
    std::unique_ptr<While> parse_while_stmt();
    std::unique_ptr<Block> parse_block();

    // Declarations
    std::unique_ptr<Declaration> parse_function();
    std::unique_ptr<Declaration> parse_declaration();
    std::unique_ptr<Prototype> parse_prototype();
};


#endif