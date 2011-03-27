#include <iostream>
#include "lexer.h"
using namespace std;

struct statement {
//	StatementType type;
};

int main(int argc, char **argv) {

	cout << "Enter one or more statements and press enter"
		<< " (Press CTRL + C to exit)" << endl;

	string lineInput;
	while(getline(cin,lineInput)) {

		vector<Token*> tokens;	

		try {
			tokens = tokenizeLine(lineInput, 1);
		} catch(string error) {
			cout << "Error: " << error << endl;
		}

		for(int i = 0; i < tokens.size(); i++) {
			cout << tokenToString(tokens[i]) << endl;
		}

		// TODO: delete tokens
	}
	return 0;
}
