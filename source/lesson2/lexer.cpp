#include <iostream>
#include <sstream>
#include "lexer.h"
using namespace std;

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

	// First check if its a symbol

	// It's not a symbol so determine the type
	
	// Is it a Number
	if(input->at(0) >= '0' && input->at(0) <= '9')	
		return getNumberToken(input);

	//Unable to determine token type, thow an error

	*input = input->substr(1);
	
	return false;
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
	switch(t->type) {
		case TOKEN_INTEGER:
			IntegerToken *it = (IntegerToken*) t;
			return string(intToString(it->value));
	}
	return "<Unknown Token>";
}


