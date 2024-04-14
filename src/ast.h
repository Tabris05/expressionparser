#ifndef AST_H
#define AST_H

#include <queue>
#include "defs.h"

size_t precedence(Operator op) {
	switch (op) {
		case '*':
		case '/':
			return 2;
		case '+':
		case '-':
			return 1;
		default:
			return 0;
	}
}

Result<AST> make_ast(queue<Token>& tokens, size_t prevPrecedence = 0);

Result<AST> make_ast_helper(AST left, queue<Token>& tokens, size_t prevPrecedence) {
	Token cur = tokens.front();
	tokens.pop();

	Result<AST> right = make_ast(tokens, prevPrecedence);
	if (!right) return right;
	
	unique_ptr<Node> result = make_unique<Node>(get<Operator>(cur), move(left), move(right.value()));
	return result;
}

Result<AST> make_ast(queue<Token>& tokens, size_t prevPrecedence) {
	if (holds_alternative<Operator>(tokens.front())) {
		return unexpected("Binary operator requires two arguments");
	}
	if (holds_alternative<RParen>(tokens.front())) {
		return unexpected("Mismatched parentheses");
	}

	Result<AST> result;
	if (holds_alternative<LParen>(tokens.front())) {
		tokens.pop();
		result = make_ast(tokens);
		if (tokens.empty() || !holds_alternative<RParen>(tokens.front())) {
			return unexpected("Mismatched parentheses");
		}
		tokens.pop();
	}
	else {
		result = make_unique<Node>(get<Number>(tokens.front()));
		tokens.pop();
	}

	while (!tokens.empty()) {
		if (!holds_alternative<Operator>(tokens.front())) break;

		size_t curPrecedence = precedence(get<Operator>(tokens.front()));
		if(curPrecedence <= prevPrecedence) break;

		result = make_ast_helper(move(result.value()), tokens, curPrecedence);
		if (!result) return result;
	}
	
	return result;
}

#endif