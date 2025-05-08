#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include <vector>

#include "token.h"
#include "ast.h"


std::unique_ptr<Expr> parse_expr(const std::vector<Token>& tokens, size_t& idx);

std::unique_ptr<Expr> parse_main(const std::vector<Token>& tokens, size_t& idx);
std::unique_ptr<Expr> parse_unary(const std::vector<Token>& tokens, size_t& idx);
std::unique_ptr<Expr> parse_binary(int expr_precedence, std::unique_ptr<Expr> lhs, const std::vector<Token>& tokens, size_t& idx);

std::vector<std::unique_ptr<ASTNode>> parse(const std::vector<Token>& tokens);


#endif