#include <iostream>
#include <optional>
#include <variant>
#include <ranges>
#include <string>
#include <vector>
#include <print>

using namespace std;
using Token = variant<double, char>;

template<typename... Fs> 
struct overload : Fs... {
	using Fs::operator()...;
};

optional<vector<Token>> tokenize(const string& line) {
	vector<Token> result;

	for (char c : line) {
		switch (c) {
			case ' ':
				continue;
			case '+':
			case '-':
			case '*':
			case '/':
				result.push_back(c);
				break;
			default:
				return nullopt;
		}

	}

	return result;
}

string to_string(const vector<Token>& tokens) {
	return tokens
		| views::transform(
			[](Token val) -> string {
				return visit(
					overload{
						[](double d) -> string { return to_string(d); },
						[](char c) -> string { return string{c}; },
					},
				val);
			}
		)
		| views::join_with(',')
		| ranges::to<string>();
}

int main() {
	string line;
	while (getline(cin, line)) {
		optional<vector<Token>> tokens = tokenize(line);
		if (!tokens) {
			println("Syntax error!");
			continue;
		}

		println("[{}]", to_string(tokens.value()));
	}
}