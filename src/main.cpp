#include <iostream>
#include <print>

#include "tokenizer.h"
#include "ast.h"
#include "evaluator.h"

int main() {
	string line;
	while (getline(cin, line)) {
		Result<queue<Token>> tokens = tokenize(line);
		if (!tokens) {
			println("Syntax error: {}", tokens.error());
			continue;
		}

		Result<AST> expressionTree = make_ast(tokens.value());
		if (!expressionTree) {
			println("Syntax error: {}", expressionTree.error());
			continue;
		}
		else if (!tokens.value().empty()) {
			println("Syntax error: Unused trailing tokens");
			continue;
		}

		vector<ExpressionToken> expression = traverse(move(expressionTree.value()));
		Result<Number> result = evaluate(expression);
		if (result) println("{}", result.value());
		else println("Logic Error: {}", result.error());
	}
}