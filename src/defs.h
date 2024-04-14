#ifndef DEFS_H
#define DEFS_H

#include <expected>
#include <variant>
#include <memory>
#include <string>

using namespace std;

struct LParen {};
struct RParen {};

using Number = double;
using Operator = char;
using Token = variant<Number, Operator, LParen, RParen>;
using ExpressionToken = variant<Number, Operator>;

template<typename T>
using Result = expected<T, string>;

template<typename... Fs>
struct overload : Fs... {
	using Fs::operator()...;
};

struct Node {
	ExpressionToken val;
	unique_ptr<Node> lhs;
	unique_ptr<Node> rhs;
};

using AST = unique_ptr<Node>;

#endif

