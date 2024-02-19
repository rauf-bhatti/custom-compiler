#ifndef _LEXER_H_
#define _LEXER_H_
#include <fstream>
#include <vector>
#include <iostream>
#include <string>
#include <cctype>
using namespace std;

//all your tokens goes here
enum class TokenType
{
	END_OF_FILE = 0,
	KEYWORD,
	IDENTIFIER,
	NUMERIC_LITERAL,
	STRING,
	SPECIAL_CHARACTER,
	IO_OPERATOR,
	ASSIGNMENT_OPERATOR,
	ARITHMETIC_OPERATOR,
	RELATIONAL_OPERATOR,
	COMMENT,
	UNIDENTIFIED,
	ERROR

};
//structure of a token 
struct token
{
	string lexeme;
	TokenType tokenType;//enum type
	//constructor
	token(string lexeme, TokenType tokenType);
	//constructor default
	token();
	void Print();
};

class lexer
{
	vector<char> stream;  //used for storing file sample_code.ol content
	void Tokenize();//populates tokens vector
	TokenType identifyToken(string); //will decide what the token type is
	void addTokens(string, TokenType token_type = TokenType::UNIDENTIFIED); //module to add tokens to the tokens vector
	int index;

public:
	vector<token> tokens; //vector to store all (tokens,lexeme) pairs
	lexer();
	lexer(const char filename[]);
	void printRaw();//just print everything as it is present in file
	token getNextToken();//get next token
	token getCurrentToken();
	void resetPointer();//reset pointer to start getting tokens from start
	int getCurrentPointer();//get where current pointer in tokens vector is
	void setCurrentPointer(int pos);//move current pointer to wherever
	token peek(int);//peek the next token
	token getPrevToken();
	vector<token> getTokenVector() { return tokens; }
	void printTokens();
};

#endif // !_LEXER_H
