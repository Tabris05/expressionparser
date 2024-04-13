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

bool isoperator(char c) {
	return (c == '+')
		|| (c == '-')
		|| (c == '*')
		|| (c == '/');
}

bool isnumber(char c) {
	return (c == '.') || isdigit(c);
}

optional<vector<Token>> tokenize(const string& line) {
	vector<Token> result;
	string number;

	bool parsingNumber = false;

	for (char c : line) {
		if (isnumber(c)) {
			parsingNumber = true;
			number.push_back(c);
			continue;
		}
		else if (parsingNumber) {
			result.push_back(stod(number));
			parsingNumber = false;
			number.clear();
		}

		if (isspace(c)) {
			continue;
		}
		else if (isoperator(c)) {
			result.push_back(c);
		}
		else {
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
		optional<vector<Token>> tokens = tokenize(line + "\n");
		if (!tokens) {
			println("Syntax error!");
			continue;
		}

		println("[{}]", to_string(tokens.value()));
	}
}