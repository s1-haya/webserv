#ifndef LEXER_HPP_
#define LEXER_HPP_
#include <list>

#include "node.hpp"

class Lexer {
  private:
	std::list<Node>         &tokens_;
	std::vector<std::string> context_;
	std::vector<std::string> directive_;
	enum TokenType {
		DELIM,
		L_BRACKET,
		R_BRACKET,
		SHARP,
		CONTEXT,
		DIRECTIVE,
		WORD
	};

	Lexer();
	void              AddToken(const char symbol, int token_type);
	void              AddToken(const std::string &symbol, int token_type);
	void              AddWordToken(std::string::const_iterator &it);
	void              SkipComment(std::string::const_iterator &it);
	void              Init();
	const std::string buffer_;

  public:
	Lexer(const std::string &, std::list<Node> &);
	~Lexer();
	void              PrintTokens();
	static const char CR            = '\r';
	static const char LF            = '\n';
	static const char SEMICOLON_CHR = ';';
	static const char L_BRACKET_CHR = '{';
	static const char R_BRACKET_CHR = '}';
	static const char SHARP_CHR     = '#';
};

#endif
