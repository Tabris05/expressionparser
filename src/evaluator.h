#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <stack>
#include <vector>
#include "defs.h"


vector<ExpressionToken> traverse(AST root) {
	vector<ExpressionToken> result;

	[&result](this auto self, AST root) {
		if (!root) return;
		self(move(root->lhs));
		self(move(root->rhs));
		result.push_back(root->val);
	} (move(root));

	return result;
}

Number evaluate(Number lhs, Operator op, Number rhs) {
	switch (op) {
		case '+':
			return lhs + rhs;
		case '-':
			return lhs - rhs;
		case '*':
			return lhs * rhs;
		case '/':
			return lhs / rhs;
	}
}

Number evaluate(const vector<ExpressionToken>& expression) {
	stack<Number> s;
	for (ExpressionToken t : expression) {
		visit(
			overload{
				[&s](Number num) {
					s.push(num);
				},
				[&s](Operator op) { 
					Number rhs = s.top();
					s.pop();

					Number lhs = s.top();
					s.pop();

					s.push(evaluate(lhs, op, rhs));
				}
			},
		t);
	}
	return s.top();
}

#endif
