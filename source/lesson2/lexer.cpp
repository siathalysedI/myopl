#include <iostream>
#include <sstream>
#include "lexer.h"
using namespace std;

struct SymbolToken {
	string symbol;
	TokenType type;
};

// These need to be in reverse size order (important)
SymbolToken SymbolTokenMap [] = {
	{"==", TOKEN_EQUALTO},
	{"(", TOKEN_OPENBRACKET},
	{")", TOKEN_CLOSEBRACKET},
	{"+", TOKEN_PLUS},
	{"-", TOKEN_MINUS},
	{"/", TOKEN_DIVIDE},
	{"*", TOKEN_MULTIPLY},

	{"", TOKEN_UNKNOWN}, // Used to indicate end of array
};

int parseInt(string s) {
	int i = 0;
	stringstream ss(s);
	ss >> i;
	return i;
}

string intToString(int i) {
	stringstream ss;
	ss << i;
	return ss.str();
}

void eatWhiteSpace(string *input) {
	while(input->length() > 0) {
		switch(input->at(0)) {
			case ' ':
			case '\t':
			case '\r':
			case '\n': // Should never find one of these! But just incase...
				*input = input->substr(1);
			default:
				return;
		}
	}
}

// This function checks if a symbol is the next token
Token *getSymbolToken(string *input) {
	for(int i = 0; SymbolTokenMap[i].type != TOKEN_UNKNOWN; i++) {
		SymbolToken s = SymbolTokenMap[i];

		if(input->length() < s.symbol.length())
			continue;

		if(input->substr(0, s.symbol.length()).compare(s.symbol) == 0) {
			Token *t = new Token();
			t->type = s.type;

			*input = input->substr(s.symbol.length());
			return t;
		}
	}
	return NULL;
}

// This function reads a number. 
// TODO: Determine whether number is floating point or not.

Token * getNumberToken(string *input) {

	int endi = -1;

	for(int i = 1; i < input->length(); i++) {
		if(input->at(i) < '0' || input->at(i) > '9') {
			endi = i;
			break;
		}
	}

	string numberString;

	if(endi == -1) {
		numberString = *input;
		*input = "";
	} else {
		numberString = input->substr(0, endi);
		*input = input->substr(endi);
	}

	return new IntegerToken(parseInt(numberString));
}

Token *getNextToken(string *input) {
	if(input->length() == 0)
		return NULL;

	Token *t; // 

	// First check if its a symbol
	t = getSymbolToken(input);	
	if(t)
		return t;
	
	// Is it a Number
	if(input->at(0) >= '0' && input->at(0) <= '9')	
		return getNumberToken(input);

	//Unable to determine token type, thow an error
	string errorMessage("Unexpected input:");
	errorMessage += input->at(0);
	throw(errorMessage);	
}

vector<Token*> tokenizeLine(string input, int lineNumber) {	
	vector<Token*> tokens;

	while(input.length() > 0) {
		eatWhiteSpace(&input);	

		Token *t = getNextToken(&input);
		if(t)
			tokens.push_back(t);		
	}

	return tokens;
}

std::string tokenToString(Token *t) {

	for(int i = 0; SymbolTokenMap[i].type != TOKEN_UNKNOWN; i++)
		if(t->type == SymbolTokenMap[i].type)
			return SymbolTokenMap[i].symbol;

	switch(t->type) {
		case TOKEN_INTEGER:	
			return intToString(((IntegerToken*)t)->value);

		case TOKEN_IDENTIFIER: return "<id>";
	}
	return "<Unknown Token>";
}


