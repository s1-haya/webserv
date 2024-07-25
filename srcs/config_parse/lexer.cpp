#include "lexer.hpp"
#include "../utils/utils.hpp"
#include <algorithm>
#include <iostream>

namespace config {
namespace lexer {
namespace {

bool IsSpace(char c) { // 必要か
	if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
		return true;
	return false;
}

} // namespace

Lexer::Lexer(const std::string &buffer, std::list<node::Node> &tokens)
	: tokens_(tokens), buffer_(buffer) {
	InitDefinition();
	LexBuffer();
}

Lexer::~Lexer() {}

void Lexer::InitDefinition() {
	context_.push_back("server");
	context_.push_back("location");

	directive_.push_back("server_name");
	directive_.push_back("listen");
	directive_.push_back("rewrite");
	directive_.push_back("root");
	directive_.push_back("index");
}

void Lexer::LexBuffer() {
	bool is_next_to_word = false;

	for (std::string::const_iterator it = buffer_.begin(); it != buffer_.end(); ++it) {
		while (IsSpace(*it)) {
			++it;
		}
		switch (*it) {
		case SEMICOLON_CHR:
			AddToken(SEMICOLON_CHR, node::DELIM);
			is_next_to_word = false;
			break;
		case L_BRACKET_CHR:
			AddToken(L_BRACKET_CHR, node::L_BRACKET);
			is_next_to_word = false;
			break;
		case R_BRACKET_CHR:
			AddToken(R_BRACKET_CHR, node::R_BRACKET);
			is_next_to_word = false;
			break;
		case SHARP_CHR:
			SkipComment(it);
			is_next_to_word = false;
			break;
		default:
			if (is_next_to_word) {
				AddWordToken(it);
			} else {
				AddContextDirectiveWordToken(it);
			}
			is_next_to_word = true;
			break;
		}
	}
}

// For char
void Lexer::AddToken(char symbol, node::TokenType token_type) {
	const std::string symbol_str(1, symbol);
	const node::Node  token = {symbol_str, token_type};
	tokens_.push_back(token);
}

// For string
void Lexer::AddToken(const std::string &symbol, node::TokenType token_type) {
	const node::Node token = {symbol, token_type};
	tokens_.push_back(token);
}

void Lexer::AddWordToken(std::string::const_iterator &it) {
	std::string new_str;
	while (!IsSpace(*it) && *it != SEMICOLON_CHR) {
		new_str += *it;
		++it;
	}
	AddToken(new_str, node::WORD);
	--it; // loopでWordの次の文字から処理
}

void Lexer::AddContextDirectiveWordToken(std::string::const_iterator &it) {
	std::string new_str = "";
	while (!IsSpace(*it) && *it != SEMICOLON_CHR) {
		new_str += *it;
		++it;
	}
	AddToken(new_str, SearchWordTokenType(new_str)); // 予約語の検索
	--it;                                            // loopでWordの次の文字から処理
}

void Lexer::SkipComment(std::string::const_iterator &it) {
	while (*it != '\n')
		++it;
}

node::TokenType Lexer::SearchWordTokenType(std::string &word) {
	if (std::find(context_.begin(), context_.end(), word) != context_.end())
		return node::CONTEXT;
	else if (std::find(directive_.begin(), directive_.end(), word) != directive_.end())
		return node::DIRECTIVE;
	return node::WORD;
}

} // namespace lexer
} // namespace config
