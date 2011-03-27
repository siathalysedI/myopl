// TODO: Prettify


#include <iostream>
#include "lexer.h"
using namespace std;

enum StatementType {
	STATEMENT_EXPRESSION
};

struct Statement { StatementType type; };

struct Expression : Statement {
	vector<Token*> parts;

	Expression(vector<Token*> _parts) { 
		type = STATEMENT_EXPRESSION;		
		parts = _parts;
	}
};

// These are the operator functions

IntegerToken add(IntegerToken a, IntegerToken b) {
	return IntegerToken(a.value + b.value);
}

IntegerToken subtract(IntegerToken a, IntegerToken b) {
	return IntegerToken(a.value - b.value);
}

IntegerToken divide(IntegerToken a, IntegerToken b) {
	return IntegerToken(a.value / b.value);
}

IntegerToken multiply(IntegerToken a, IntegerToken b) {
	return IntegerToken(a.value * b.value);
}

// Operator definitions

struct RelationalOperator {
	TokenType type;
	IntegerToken (*func) (IntegerToken a, IntegerToken b);
};

// These must be in order of importance

RelationalOperator RelationalOperatorMap [] = {
	{ TOKEN_DIVIDE, divide },
	{ TOKEN_MULTIPLY, multiply },
	{ TOKEN_MINUS, subtract },
	{ TOKEN_PLUS, add },

	{ TOKEN_UNKNOWN, NULL },
};

bool isRelationalOperator(TokenType type) {
	for(int i = 0; RelationalOperatorMap[i].type != TOKEN_UNKNOWN; i++)
		if(type == RelationalOperatorMap[i].type)
			return true;
	return false;
}

vector<Statement*> statementize(vector<Token*> tokens) {
	vector<Statement*> statements;
	vector<Token*> currentStatement;

	for(int i = 0; i < tokens.size(); i++) {

		if(currentStatement.size() == 0) {

			if(tokens[i]->type != TOKEN_INTEGER && 
					tokens[i]->type != TOKEN_OPENBRACKET)
				throw("Statement starts with:" + tokenToString(tokens[i]));

			currentStatement.push_back(tokens[i]);
			continue;
		}

		if(tokens[i]->type == TOKEN_CLOSEBRACKET) {
			currentStatement.push_back(tokens[i]);
			continue;
		}

		// Check opererators
		if(isRelationalOperator(tokens[i]->type)) {

			if(currentStatement.back()->type == TOKEN_INTEGER ||
					currentStatement.back()->type == TOKEN_CLOSEBRACKET) {
				currentStatement.push_back(tokens[i]);
				continue;
			}
			throw("No lhand for operator:" + tokenToString(tokens[i]));
		}

		// check numbers + open brackets
		if(tokens[i]->type == TOKEN_INTEGER ||
				tokens[i]->type == TOKEN_OPENBRACKET) {

			if(isRelationalOperator(currentStatement.back()->type) ||
					currentStatement.back()->type == TOKEN_OPENBRACKET) {

				currentStatement.push_back(tokens[i]);
				continue;

			} else {
				
				statements.push_back(new Expression(currentStatement));
				currentStatement.clear();
				currentStatement.push_back(tokens[i]);
				continue;
			}
		}

		throw("Unknown token:" + tokenToString(tokens[i]));				
	}

	// Add currentStatement to statements as long as last token is num or ')'
	
	if(currentStatement.size() > 0) {

		if(currentStatement.back()->type == TOKEN_INTEGER ||
				currentStatement.back()->type == TOKEN_CLOSEBRACKET) {

			statements.push_back(new Expression(currentStatement));
		} else {
			throw("Unexpected end of statement:" + 
				tokenToString(currentStatement.back()));
		}
	}		

	return statements;
}

string statementToString(Statement *sttmnt) {
	string s;

	if(sttmnt->type == STATEMENT_EXPRESSION) {
		vector<Token*> parts = ((Expression*)sttmnt)->parts;
		for(int i = 0; i < parts.size(); i++)
			s += tokenToString(parts[i]) + " ";

	} else {
		s = "<Unknown Statement>";
	}
	return s;	
}

// This function returns a sub vector, removing elements from the original

vector<Token*> sliceExpressionParts(vector<Token*> *parts, 
		int openi, int closei) {

	vector<Token*> subparts;

	for(int i = openi + 1; i < closei; i++)
		subparts.push_back(parts->at(i));

	parts->erase(parts->begin() + openi, 
		parts->begin() + closei + 1);

	return subparts;
}

// This function finds the index of the matching closing bracket for an
// open bracket

int getCloseBracket(vector<Token*> parts, int openi) {
	int scope = 1;
	for(int i = openi + 1; i < parts.size(); i++) {
		if(parts[i]->type == TOKEN_OPENBRACKET) {
			scope += 1;
		} else if(parts[i]->type == TOKEN_CLOSEBRACKET) {
			scope -= 1;
			if(scope == 0) 
				return i;
		}
	}
	return -1;
}

IntegerToken *evaluateExpression(Expression *e);

// This function evauluates the contents of brackets and replaces the brackets
// with the result

void doBrackets(Expression *e) {
	vector<Token*> *parts = &e->parts;
	for(int i = 0; i < parts->size(); i++) {
		if(parts->at(i)->type == TOKEN_CLOSEBRACKET)
			throw(string("Unmatched closing bracket"));

		if(parts->at(i)->type == TOKEN_OPENBRACKET) {
			int closei = getCloseBracket(*parts, i);
			if(closei == -1)
				throw(string("Unmatched open bracket"));

			vector<Token*> inBrackets = sliceExpressionParts(parts, i, closei);
			parts->insert(parts->begin() + i, 
				evaluateExpression(new Expression(inBrackets)));	
			i--;
		}
	}
}

// TODO:

void doOperators(Expression *e) {

}

IntegerToken *evaluateExpression(Expression *e) {

	doBrackets(e);

	doOperators(e);

	if(e->parts.size() > 1)
		throw(string("Expression was not reduced to a single element"));

	if(e->parts.size() == 0)
		return new IntegerToken(0); // This might evaluate to nil in the future

	if(e->parts[0]->type != TOKEN_INTEGER)
		throw(string("Expression did not evaluate to a number"));

	return (IntegerToken*) e->parts[0];
}

string doStatement(Statement *sttmnt) {
	string result;
	if(sttmnt->type == STATEMENT_EXPRESSION) {
		result = tokenToString(evaluateExpression((Expression*)sttmnt));
	} else {
		result = "<Unknown expression>";
	}
	return result;
}

int main(int argc, char **argv) {

	cout << "Enter one or more statements and press enter"
		<< " (Press CTRL + C to exit)" << endl;

	string lineInput;
	while(getline(cin,lineInput)) {

		vector<Token*> tokens;	
		vector<Statement*> statements;

		try {
			tokens = tokenizeLine(lineInput, 1);
			statements = statementize(tokens);

			for(int i = 0; i < statements.size(); i++) {
				string result = doStatement(statements[i]);
				cout << statementToString(statements[i]) 
					<< "= " << result << endl;
			}		

		} catch(string error) {
			cout << "Error: " << error << endl;
		}

		
		// TODO: delete tokens and statements
	}
	return 0;
}



