#include <iostream>
#include <optional>
#include <variant>
#include <ranges>
#include <string>
#include <print>
#include <queue>
#include <stack>

using namespace std;
using Token = variant<double, char>;

template<typename... Fs> 
struct overload : Fs... {
	using Fs::operator()...;
};

struct Node {
	Token val;
	unique_ptr<Node> lhs;
	unique_ptr<Node> rhs;
};

size_t precedence(char op) {
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

optional<unique_ptr<Node>> make_ast(queue<Token>& tokens, size_t prevPrecedence = 0);

optional<unique_ptr<Node>> make_ast_helper(unique_ptr<Node> left, queue<Token>& tokens, size_t prevPrecedence) {
	Token cur = tokens.front();
	if(!holds_alternative<char>(cur)) return left;

	size_t curPrecedence = precedence(get<char>(cur));
	if (curPrecedence <= prevPrecedence) {
		return left;
	}
	else {
		tokens.pop();
		optional<unique_ptr<Node>> right = make_ast(tokens, curPrecedence);
		if (!right) return nullopt;

		unique_ptr<Node> result = make_unique<Node>(cur, move(left), move(right.value()));
		return result;
	}
}

optional<unique_ptr<Node>> make_ast(queue<Token>& tokens, size_t prevPrecedence) {
	if (tokens.size() == 0 || !holds_alternative<double>(tokens.front())) return nullopt;

	optional<unique_ptr<Node>> result = make_unique<Node>(tokens.front());
	tokens.pop();

	while (!tokens.empty()) {
		Node* temp = result.value().get();
		result = make_ast_helper(move(result.value()), tokens, prevPrecedence);
		if (!result) return nullopt;
		if (result.value().get() == temp) break;
	}
	
	return result;
}

bool isoperator(char c) {
	return (c == '+')
		|| (c == '-')
		|| (c == '*')
		|| (c == '/');
}

bool isnumber(char c) {
	return (c == '.') || isdigit(c);
}

optional<queue<Token>> tokenize(const string& line) {
	queue<Token> result;
	string number;

	bool parsingNumber = false;

	for (char c : line) {
		if (isnumber(c)) {
			parsingNumber = true;
			number.push_back(c);
			continue;
		}
		else if (parsingNumber) {
			result.push(stod(number));
			parsingNumber = false;
			number.clear();
		}

		if (isspace(c)) {
			continue;
		}
		else if (isoperator(c)) {
			result.push(c);
		}
		else {
			return nullopt;
		}
	}

	return result;
}

vector<Token> traverse(unique_ptr<Node> root) {
	vector<Token> result;

	[&result](this auto self, unique_ptr<Node> root) {
		if (!root) return;
		self(move(root->lhs));
		self(move(root->rhs));
		result.push_back(root->val);
	}(move(root));

	return result;
}

double evaluate(double lhs, char op, double rhs) {
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

optional<double> evaluate(const vector<Token>& expression) {
	stack<double> s;
	for (Token t : expression) {
		bool good = visit(
			overload{
				[&s](double d) { s.push(d); return true; },
				[&s](char c) { 
					if (s.size() < 2) return false;
					double rhs = s.top(); s.pop();
					double lhs = s.top(); s.pop();
					s.push(evaluate(lhs, c, rhs));
				}
			},
		t);
		if (!good) return nullopt;
	}
	return s.top();
}

int main() {
	string line;
	while (getline(cin, line)) {
		optional<queue<Token>> tokens = tokenize(line + "\n");
		if (!tokens) {
			println("Syntax error!");
			continue;
		}

		optional<unique_ptr<Node>> ast = make_ast(tokens.value());
		if (!ast) {
			println("Syntax error!");
			continue;
		}

		vector<Token> expression = traverse(move(ast.value()));
		optional<double> result = evaluate(expression);

		if (result) println("{}", result.value());
	}
}