#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>

enum TokenType {

	// Brackets
	TOKEN_OPENBRACKET,
	TOKEN_CLOSEBRACKET,

	// Operators
	TOKEN_PLUS,
	TOKEN_MINUS,
	TOKEN_MULTIPLY,
	TOKEN_DIVIDE,	

	// Comparison
	TOKEN_EQUALTO,

	// Numbers
	TOKEN_INTEGER,

	// Identifier
	TOKEN_IDENTIFIER,

	// Errors
	TOKEN_UNKNOWN,
};

struct Token { TokenType type; };

struct IntegerToken : Token { 
	int value; 
	IntegerToken(int _value) { type = TOKEN_INTEGER; value = _value; }
};

std::vector<Token*> tokenizeLine(std::string input, int lineNumber);

std::string tokenToString(Token *t);

#endif
