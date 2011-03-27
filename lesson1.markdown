#Make Your Own Programming Language!
This tutorial will teach you how to write your own programming language. I will be using C++ examples for the tutorial but the concepts should be easy to implement in any language you like, thought I assume the reader knows how read and understand C++ code.
##Lesson 1: Writing a basic expression evaluator.
In the following tutorial I will show you how to create a program that can take an input like this:

`1 + 2 x 3 + ( 10 / 10 )`

And output this:

`= 8`

_Notice how it takes order of operations into account._

##Part 1: Understanding the input.
Lets start with our main function:

    int main(int argc, char **argv) {
		vector<Token> expression;

		// Tokenize the command line arguments
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

As you can see; every command line argument is read, converted into a token and added to the expression. After all the arguments have been read we attempt to evaluate the expression, printing the result or an error message.

The keen eyed among you will have noticed the `Token` type. The first thing a compiler or interpreter does is to split its input into tokens. Tokens are all the different parts of the input expression. This is an example of how our input is to be broken down:

Input: `1 + 2 x 3 + ( 10 / 10 )`
Tokens:

* Number (1)
* Plus operator
* Number (2)
* Multiply operator
* Number (3)
* Plus operator
* Open bracket
* Number (10)
* Divide operator
* Number (10)
* Close bracket

To make things easy, we have used command line arguments as separate tokens. Each argument is passed into the `Token` constructor which works out the type of token:

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

These are added to the back of an std::vector in order and this makes up our 'expression'. 

##Part 2: Evaluating brackets.

Meet our new evaluation function:

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

This function attempts to reduce an expression (vector of Tokens) down into a single token. Each of the do* functions will modify the expression, reducing the number of token. Note how we pass the expression by reference as the function will modify its contents. After processing all the brackets and the operators we should have a single token, but if there is not we throw an error to let the user no that expression is not formed correctly.

Ok, lets have a look at this `doBrackets` function:

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

You are provably wondering what the hell is going on here at first glance, but in reality its not that complex. We are looping through every Token looking for opening and closing brackets. If we encounter an open bracket we first find the index of the matching closing bracket in the vector, throwing an error if it cannot be found. We then slice all the tokens in brackets out of the original expression, leaving a new `inBrackets` vector and reducing the size of our original `expression` vector. If we read a closing bracket before an open bracket we throw an error.

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

Once we have found an open bracket `getCloseBracket` is called. This function continues to loop through all the tokens in the expression until it finds the matching closing bracket. If the function encounters an open bracket it will increase the value of `scope` by one. If it finds a closing bracket it will reduce the value of `scope` by one. It will only return the index of the closing bracket when scope is zero. This means we can do `( ( ( ( ) ) ) )` with an `openi` index of 0 (the first bracket) and it will return an index of 7 (the matching bracket) rather than 4 (the first closing bracket).

	vector<Token> sliceExpression(vector<Token> *expression, 
			int openi, int closei) {

		vector<Token> subexp;

		for(int i = openi + 1; i < closei; i++)
			subexp.push_back(expression->at(i));

		expression->erase(expression->begin() + openi, 
			expression->begin() + closei + 1);

		return subexp;
	}

This code simply returns a new vector containing the Tokens between `openi` and `closei`. It also erases these tokens from the original `expression` vector.

If you look back; the `doBrackets` function has a call to `evaluate` on this line: 
`expression->insert(expression->begin() + i, evaluate(inBrackets));` This means that brackets will be evaluated recursively as `evaluate` calls `doBrackets` calls `evaluate` calls `doBrackets` etc. until all nested brackets are evaluated.

##Part 3: Operators

Remember this code inside evaluate?

	// Order is important here
	doOperator(&expression, TOKEN_DIVIDE, divide);
	doOperator(&expression, TOKEN_MULTIPLY, multiply);
	doOperator(&expression, TOKEN_SUBTRACT, subtract);
	doOperator(&expression, TOKEN_PLUS, add);

The doOperator function takes and expression, a `TokenType` for the operator and a function pointer to a function that performs the operation:

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

It loops through the Tokens in the expression looking for a `Token` with a `.type` that matches the operator. Upon finding one it will called the passed in function `func` with the `Token` to the left and right of the current token as parameters. It will then replace all 3 tokens with a single token that contains the result of the operation.

The operator functions are defined as follows:

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

Each operator does a basic check to make sure its operating on  two numbers, throwing an error if they are not.

##Part 4: Putting it all together
[The source code for this lesson can be found here.](https://github.com/tm1rbrt/myopl/blob/master/source/lesson1/main.cpp)

To compile use `g++ main.cpp -o lesson1.exe -static`

To test it use `lesson1 ( 1 + 2 ) x 3`
_Remember to separate each token with a space and the multiply operator is an `x` as the `*` symbol will do funny things on windows cmd prompt_

###Further notes
This code is only for teaching the basic mechanics of an expression evaluator and there are lots of things wrong with it (such as operators at the start or and of an expression not being handles correctly). Throughout the series of tutorials every piece of code here will be replaced with more improved, stable and better designed implementations.







