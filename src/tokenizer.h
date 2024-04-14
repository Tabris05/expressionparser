#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <queue>
#include <format>
#include <string_view>
#include "defs.h"

optional<Number> tryParseNumber(string_view line, size_t& idx) {
	size_t i;
	size_t decimalOffset = 1;
	bool seenDecimal = false;
	Number result = 0.0;

	if (idx >= line.size() || (!isdigit(line[idx]) && line[idx] != '.')) return nullopt;

	for (i = idx; i < line.size() && (isdigit(line[i]) || line[i] == '.'); i++) {
		if (line[i] == '.') {
			if (seenDecimal) return nullopt;
			seenDecimal = true;
			continue;
		}
		result *= 10;
		if(seenDecimal) decimalOffset *= 10;
		result += line[i] - '0';
	}
	idx = i;
	return result / decimalOffset;
}

optional<Operator> tryParseOperator(string_view line, size_t& idx) {
	switch (line[idx]) {
	case '+':
	case '-':
	case '*':
	case '/':
		return line[idx++];
	default:
		return nullopt;
	}
}

Result<queue<Token>> tokenize(string_view line) {
	queue<Token> result;
	size_t idx = 0;

	while (idx < line.size()) {
		while (idx < line.size() && isspace(line[idx])) idx++;

		optional<Number> maybeNum = tryParseNumber(line, idx);
		if (maybeNum) {
			result.push(maybeNum.value());
			continue;
		}

		optional<Operator> maybeOp = tryParseOperator(line, idx);
		if (maybeOp) {
			result.push(maybeOp.value());
			continue;
		}

		if(idx < line.size()) return unexpected(format("Invalid token at position {}", idx));
	}

	return result;
}

#endif
