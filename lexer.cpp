#include "lexer.h"
#include <string>
using namespace std;
//for printing tokens names, Same as the enum defined in lexer.h
string reserved[] = {
    "END_OF_FILE"
	};

token::token()
{
    this->lexeme = "";
    this->tokenType = TokenType::ERROR;
}

token::token(string lexeme, TokenType tokenType)
{
    this->lexeme = lexeme;
    this->tokenType = tokenType;
}

void token::Print()
{
    cout << "{" << lexeme << ", "
        << int(tokenType) << "}\n";
}

int lexer::getCurrentPointer()
{
    return index;
}

void lexer::setCurrentPointer(int pos)
{
    if (pos >= 0 && pos < tokens.size())
        index = pos;
    else
        index = 0;
}

TokenType lexer::identifyToken(string token_string)
{
    // First check will be to see if the token is a KEYWORD.
    // Longest keyword is 6 in length. So we will ignore strings with > 6 length
    if (token_string.length() <= 6)
    {
        if 
        (
            token_string == "markazi"   ||  token_string == "kaam"      || token_string == "karo"   || token_string == "rakho"
            || token_string == "jab"    ||  token_string == "tak"       || token_string == "bas"    || token_string == "agar"
            || token_string == "to"     ||  token_string == "warna"     || token_string == "phir"   || token_string == "dekhao"
            || token_string == "lo"     ||  token_string == "chalao"    || token_string == "wapas"  || token_string == "bhaijo"
            || token_string == "adad"   ||  token_string == "khali"     || token_string == "khatam"
        )
        {
            return TokenType::KEYWORD;
        }
    }


    if (!isdigit(token_string[0]) && (isalpha(token_string[0]) || token_string[0] == '_')) 
    {
        bool flag = true;
        // Condition 1 of identifier check passed!
        for (char c : token_string)
        {
            if (iswalnum(c) || c == '_') 
            {
                continue;
            }
            else 
            {
                flag = false;
                break;
            }
        }

        if (flag)
        {
            return TokenType::IDENTIFIER;
        }
    }

    if (!iswalnum(token_string[0])) // Neither alphabet nor number
    {
        if (token_string.length() == 1) // Arithmetic or Special
        {
            if 
            (
                token_string == "|" || token_string == ":" || token_string == ";" ||
                token_string == "@" || token_string == "`" || token_string == "(" ||
                token_string == ")" 
            )
            {
                return TokenType::SPECIAL_CHARACTER;
            }
            else if 
            (
                token_string == "+" || token_string == "-" || token_string == "*" ||
                token_string == "/" || token_string == "%"
            )
            {
                return TokenType::ARITHMETIC_OPERATOR;
            }
            else if (token_string == ">" || token_string == "<" || token_string == "=")
            {
                return TokenType::RELATIONAL_OPERATOR;
            }
        }
        else if (token_string.length() == 2)
        {
            if 
            (
                token_string == "<>" || token_string == "<=" || token_string == ">="
            )
            {
                return TokenType::RELATIONAL_OPERATOR;
            }
            else if (token_string == ">>" || token_string == "<<")
            {
                return TokenType::IO_OPERATOR;
            }
            else if (token_string == ":=") 
            {
                return TokenType::ASSIGNMENT_OPERATOR;
            }
        }
    }

    if (isdigit(token_string[0]))
    {
        bool flag = true;
        
        for (char c : token_string)
        {
            if (!isdigit(c))
            {
                flag = false;
            }
        }

        if (flag)
        {
            return TokenType::NUMERIC_LITERAL;
        }
    }

    return TokenType::ERROR;
}

void lexer::addTokens(string token_string, TokenType token_type)
{
    token _token;
    token_type == TokenType::UNIDENTIFIED ? 
        _token = token(token_string, identifyToken(token_string)) :
        _token = token(token_string, token_type); 

    this->tokens.push_back(_token);

}

void lexer::Tokenize()//function that tokenizes your input stream
{
    vector<char>::iterator it = stream.begin();
    int token_count = 0;
    char whitespace = ' ';


    vector<char> local_token_stack;

	//your implementation goes here
    for (it; it < stream.end(); it++) 
    {        
        //Add conditions where check needs to happen.
        //Add check at whitespace
        //cout << *it;
        if (*it == whitespace || *it == '\n')
        {
            //Now we have to check the contents of the local_token_stack
            //First check for keywords
            string token_string(local_token_stack.begin(), local_token_stack.end());
            //cout << token_string << endl;
            if (token_string.length() > 0)
            {
                addTokens(token_string);
                local_token_stack.clear(); // Reset vector for new tokens
            }
        }
        else if (*it == '@')
        {
            if (string(local_token_stack.begin(), local_token_stack.end()).length() > 0)
            {
                addTokens(string(local_token_stack.begin(), local_token_stack.end()));
                local_token_stack.clear(); // Reset vector for new tokens
            }


            addTokens(string(1, *it)); // Add @ to the tokens vector
        }
        else if (*it == '`') // String started
        {
            addTokens(string(1, *it), TokenType::SPECIAL_CHARACTER); // Push first backtick
            //local_token_stack.push_back(*it);
            it++;
            while (*it != '`' || *it == EOF) // EOF incase we never find the second 
            {
                local_token_stack.push_back(*it);
                it++;
            }
            //local_token_stack.push_back(*it);

            addTokens(string(local_token_stack.begin(), local_token_stack.end()), TokenType::STRING);
            addTokens(string(1, *it), TokenType::SPECIAL_CHARACTER); // Push second backtick
            local_token_stack.clear();
        }
        else if (*it == ';')
        {
            // First check if it is an ending statement or a comment.
            // If it ends something, there is something before it.
            if (string(local_token_stack.begin(), local_token_stack.end()).length() > 0)
            {
                addTokens(string(local_token_stack.begin(), local_token_stack.end()));
                local_token_stack.clear();
            }

            // Add the special token here.
            addTokens(string(1, *it));

            //Check if there is something else after the token/check for comment
            it++;
            while (*it != '\n')
            {
                local_token_stack.push_back(*it);
                it++;
            }

            if (local_token_stack.size() > 0) //Comment present.
            {
                addTokens(string(local_token_stack.begin(), local_token_stack.end()), TokenType::COMMENT);
                local_token_stack.clear();
            }
        }
        else if (*it == '-' || *it == '+' || *it == '*' || *it == '\\' || *it == '%' || *it == '=')
        {
            // Add the stuff before here
            if (string(local_token_stack.begin(), local_token_stack.end()).length() > 0)
            {
                addTokens(string(local_token_stack.begin(), local_token_stack.end()));
                local_token_stack.clear();
            }

            // Add the special token here.
            addTokens(string(1, *it));

            // Add the stuff after the operand here
            it++;

            while (*it != '\n')
            {
                if (*it == ';' || *it == ')' || *it == '>' || *it == '<' || *it == '=' || *it == '+' || *it == '-' || *it == '*' || *it == '\\')
                {
                    break;
                }

                if (*it != ' ' && string(1, *it).length() > 0)
                {
                    local_token_stack.push_back(*it);
                }
                it++;
            }
            it--;
            if (local_token_stack.size() > 0)
            {
                addTokens(string(local_token_stack.begin(), local_token_stack.end()));
                local_token_stack.clear();
            }
        }
        else if (*it == '|' || *it == '(' || *it == ')')
        {
            // Handle case when there is no space before brackets and pipe
            // In this case the local_token_stack will already have developed or will be empty
            // Empty will be true in case of space, tab etc.

            // PROCESS FIRST STRING HERE!
            if (string(local_token_stack.begin(), local_token_stack.end()).length() > 0)
            {
                addTokens(string(local_token_stack.begin(), local_token_stack.end()));
                local_token_stack.clear();
            }

            // Add the special character token here!
            addTokens(string(1, *it));
        }
        else if (*it == '<' || *it == '>' || *it == ':')
        {
            if (string(local_token_stack.begin(), local_token_stack.end()).length() > 0)
            {
                addTokens(string(local_token_stack.begin(), local_token_stack.end()));
                local_token_stack.clear();
            }

            // Check if I/O strings.
            char c = *(it + 1); // Check character after < or >

            if (*it == c || c == '<' || c == '>' || c == '=') // IO case!
            {
                local_token_stack.push_back(*it);
                it++;
                local_token_stack.push_back(*it);

                addTokens(string(local_token_stack.begin(), local_token_stack.end()));
                local_token_stack.clear();
            }
            else // It is an arithmetic operator!
            {
                addTokens(string(1, *it));
            }
            // Add the special token here.
        }
        else if (*it != '\t' && *it != '\0')
        {
            local_token_stack.push_back(*it);
        }
    }
	//push_back EOF token at end to identify End of File
    tokens.push_back(token(string(""), TokenType::END_OF_FILE));
}


lexer::lexer(const char filename[])
{
    //constructors: takes file name as an argument and store all
    //the contents of file into the class varible stream
    ifstream fin(filename);
    if (!fin) //if file not found
    {
        cout << "file not found" << endl;
    }
    else
    {
        char byte = 0;
        while (fin.get(byte))
        { //store file contents into stream
            if (byte != '\r')
                stream.push_back(byte);
        }
        stream.push_back(' ');//dummy spaces appended at the end
        stream.push_back(' ');
        //Tokenize function to get all the (token,lexeme) pairs
        Tokenize();
        //assigning tokens to iterator::pointer
        index = 0;
    }
}

lexer::lexer()
{
    index = -1;
}

void lexer::printRaw()
{
    //helper function to print the content of file
    vector<char>::iterator it = stream.begin();
    for (it = stream.begin(); it != stream.end(); it++)
    {
        cout << *it;
    }
    cout << endl;
}

token lexer::getNextToken()
{
    //this function will return single (token,lexeme) pair on each call
    //this is an iterator which is pointing to the beginning of vector of tokens
    token _token;
   // cout << "FUNC:" << tokens[index].lexeme << ", " << int(tokens[index].tokenType) << endl;

    if (index == tokens.size())
    {                       // return end of file if
        _token.lexeme = ""; // index is too large
        _token.tokenType = TokenType::END_OF_FILE;
    }
    else
    {
        _token = tokens[index];
        index = index + 1;
    }
    return _token;
}

token lexer::getCurrentToken()
{
    //this function will return single (token,lexeme) pair on each call
    //this is an iterator which is pointing to the beginning of vector of tokens
    return tokens[getCurrentPointer() - 1];
}

void lexer::resetPointer()
{
    index = 0;
}

token lexer::peek(int howFar)
{
    if (howFar <= 0)
    { // peeking backward or in place is not allowed
        cout << "LexicalAnalyzer:peek:Error: non positive argument\n";
        exit(-1);
    }

    int peekIndex = index + howFar - 1;
    if (peekIndex > (tokens.size() - 1))
    {                                                 // if peeking too far
        return token("", TokenType::END_OF_FILE); // return END_OF_FILE
    }
    else
        return tokens[peekIndex];
}

token lexer::getPrevToken()
{
    return tokens[getCurrentPointer() - 2];
}

void lexer::printTokens()
{
    for (int i = 0; i < tokens.size(); i++)
    {
        tokens[i].Print();
    }
}