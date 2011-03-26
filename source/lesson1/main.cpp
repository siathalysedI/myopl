#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

// This utility function attempts to convert a string to an int

bool parseInt(string s, int *value) {
	stringstream ss(s);
	if((ss >> *value).fail())
		return false;
	return true;
}

// These are the token type identifiers

enum TokenType {

	// Brackets
	TOKEN_OPENBRACKET,
	TOKEN_CLOSEBRACKET,

	// Operators
	TOKEN_PLUS,
	TOKEN_SUBTRACT,
	TOKEN_MULTIPLY,
	TOKEN_DIVIDE,	

	// Numbers
	TOKEN_INTEGER,

	// Errors
	TOKEN_UNKNOWN,
};

// This is the Token class
// Tokens consist of a type (and a value if its a number)

class Token {
public:
	TokenType type;
	int value;

	Token(string s) {

		// Check if it's an operator or bracket
		if(s.length() == 1)
			switch(s[0]) {
				case '+': type = TOKEN_PLUS; return;
				case '-': type = TOKEN_SUBTRACT; return;
				case 'x': type = TOKEN_MULTIPLY; return;
				case '/': type = TOKEN_DIVIDE; return;				

				case '(': type = TOKEN_OPENBRACKET; return;
				case ')': type = TOKEN_CLOSEBRACKET; return;
			}

		// Make sure its a valid integer
		if(!parseInt(s, &value))
			type = TOKEN_UNKNOWN;
		else
			type = TOKEN_INTEGER;
	}

	Token(int i) { value = i; type = TOKEN_INTEGER; }
};

// This function finds the index of the matching closing bracket for an
// open bracket

int getCloseBracket(vector<Token> expression, int openi) {
	int scope = 1;
	for(int i = openi + 1; i < expression.size(); i++) {
		if(expression[i].type == TOKEN_OPENBRACKET) {
			scope += 1;
		} else if(expression[i].type == TOKEN_CLOSEBRACKET) {
			scope -= 1;
			if(scope == 0) 
				return i;
		}
	}
	return -1;
}

// This function returns a sub vector, removing elements from the original

vector<Token> sliceExpression(vector<Token> *expression, 
		int openi, int closei) {

	vector<Token> subexp;

	for(int i = openi + 1; i < closei; i++)
		subexp.push_back(expression->at(i));

	expression->erase(expression->begin() + openi, 
		expression->begin() + closei + 1);

	return subexp;
}

Token evaluate(vector<Token> expression);

// This function evauluates the contents of brackets and replaces the brackets
// with the result

bool doBrackets(vector<Token> *expression) {
	for(int i = 0; i < expression->size(); i++) {
		if(expression->at(i).type == TOKEN_CLOSEBRACKET)
			throw("Unmatched closing bracket");

		if(expression->at(i).type == TOKEN_OPENBRACKET) {
			int closei = getCloseBracket(*expression, i);
			if(closei == -1)
				throw("Unmatched open bracket");

			vector<Token> inBrackets = sliceExpression(expression, i, closei);
			expression->insert(expression->begin() + i, evaluate(inBrackets));	
			i--;
		}
	}	
	return true;
}

// These are the operator functions

Token add(Token a, Token b) {
	if(a.type != TOKEN_INTEGER || b.type != TOKEN_INTEGER)
		throw("Can't add non integers");
	return Token(a.value + b.value);
}

Token subtract(Token a, Token b) {
	if(a.type != TOKEN_INTEGER || b.type != TOKEN_INTEGER)
		throw("Can't subtract non integers");
	return Token(a.value - b.value);
}

Token divide(Token a, Token b) {
	if(a.type != TOKEN_INTEGER || b.type != TOKEN_INTEGER)
		throw("Can't divide non integers");
	return Token(a.value / b.value);
}

Token multiply(Token a, Token b) {
	if(a.type != TOKEN_INTEGER || b.type != TOKEN_INTEGER)
		throw("Can't multiply non integers");
	return Token(a.value * b.value);
}

// This function finds the specified operator within an expression and calls
// the specified function on the tokens either side of it, replacing all three
// with the result of the function

void doOperator(vector<Token> *expression, TokenType op, 
		Token (*func) (Token, Token)) {

	for(int i = 1; i < (int)expression->size() - 1; i++) {
		if(expression->at(i).type == op) {
			expression->at(i-1) = 
				func(expression->at(i - 1), expression->at(i + 1));

			expression->erase(expression->begin() + i, 
				expression->begin() + i + 2); 

			i--;
		}	 
	}	
}

// This function evaluates an expression

Token evaluate(vector<Token> expression) {

	// Evaluate any brackets first
	doBrackets(&expression);

	// Order is important here
	doOperator(&expression, TOKEN_DIVIDE, divide);
	doOperator(&expression, TOKEN_MULTIPLY, multiply);
	doOperator(&expression, TOKEN_SUBTRACT, subtract);
	doOperator(&expression, TOKEN_PLUS, add);

	if(expression.size() > 1)
		throw("At least one operator missing");

	if(expression.size() == 0)
		return Token(0); // This might evaluate to nil in the future

	return expression[0];
}

// This is the entry point, We convert arguements from the command line into
// tokens that form an expression and attempt to evaluate it, printing the 
// result (or an error)

int main(int argc, char **argv) {

	vector<Token> expression;

	// Tokenize the command line arguements
	for(int i = 1; i < argc; i++) {
		Token t(argv[i]);
		if(t.type == TOKEN_UNKNOWN) {
			cout << "Invalid token: '" << argv[i] 
				<< "'. Can't evaluate!" << endl;
			return 1;
		}
		expression.push_back(t);
	}

	// Evaluate the tokens
	try {
		Token result = evaluate(expression);
		cout << result.value << endl;
	} catch(const char *error) {
		cout << error << ", Can't evaluate!" << endl;
	}

	return 0;

}
