#include "parser.h"
#include <cmath>
void parser::syntax_error()
{
    cout << "SYNTAX ERROR ENCOUNTERED AT: " << _lexer.getCurrentToken().lexeme << "\n";
    cout << "TYPE: " << int(_lexer.getCurrentToken().tokenType) << "\n";
    printSymbolTable();
    exit(1);
}
token parser::expect(TokenType expected_type)
{
    token t = _lexer.getNextToken();
    this->local_tokens.push_back(t);
    if (t.tokenType != expected_type)
        syntax_error();
    return t;
}
parser::parser(const char filename[])
{
    _lexer = lexer(filename);
}
void parser::readAndPrintAllInput() //read and print allinputs (provided)
{
    token t;
    t = _lexer.getNextToken();
    while (t.tokenType != TokenType::END_OF_FILE)
    {
        t.Print();
        t = _lexer.getNextToken();
    }
}
void parser::resetPointer()
{
    _lexer.resetPointer();
}

// PARSER LOGIC STARTS FROM HERE

void parser::checkExpression()
{
    vector<string> openBracket = vector<string>();

    while (true)
    {
        if (_lexer.peek(1).tokenType == TokenType::SPECIAL_CHARACTER && _lexer.peek(1).lexeme == "(")
        {
            openBracket.push_back(expect(TokenType::SPECIAL_CHARACTER).lexeme);
        }
        else if (_lexer.peek(1).tokenType == TokenType::IDENTIFIER)
        {
            expect(TokenType::IDENTIFIER);
        }
        else if (_lexer.peek(1).tokenType == TokenType::NUMERIC_LITERAL)
        {
            expect(TokenType::NUMERIC_LITERAL);
        }
        else if (_lexer.peek(1).tokenType == TokenType::ARITHMETIC_OPERATOR)
        {
            expect(TokenType::ARITHMETIC_OPERATOR);

            if ((_lexer.getPrevToken().tokenType == TokenType::IDENTIFIER || _lexer.getPrevToken().tokenType == TokenType::NUMERIC_LITERAL)
                &&
                _lexer.peek(1).tokenType == TokenType::IDENTIFIER || _lexer.peek(1).tokenType == TokenType::NUMERIC_LITERAL)
            {
                if (_lexer.peek(1).tokenType == TokenType::IDENTIFIER) 
                {
                    expect(TokenType::IDENTIFIER);
                }
                else
                {
                    expect(TokenType::NUMERIC_LITERAL);
                }
            }
            else
            {
                syntax_error();
            }
        }
        else if (openBracket.size() > 0 && _lexer.peek(1).tokenType == TokenType::SPECIAL_CHARACTER && _lexer.peek(1).lexeme == ")")
        {
          //  cout << "POPOP" << endl;
            expect(TokenType::SPECIAL_CHARACTER);
            openBracket.pop_back();
        }
        else
        {
            return;
        }
    }
}

void parser::checkConditions()
{
    // exp op exp
    checkExpression();
    expect(TokenType::RELATIONAL_OPERATOR);
    checkExpression();
}


bool parser::checkIdentifierDatatypePair(bool generateTAC, bool isFunction)
{
    symbol_table_entry entry;
    entry.id_name = expect(TokenType::IDENTIFIER).lexeme;

    if (isFunction)
    {
        // this->address = 0;
        entry.type = "func";
    }
    else 
    {
        entry.type = "var";
        entry.datatype = "unknown";
        entry.address = to_string(address);
        entry.initialValue = "0";
        address += 1;
    }


    if (_lexer.peek(1).tokenType == TokenType::SPECIAL_CHARACTER)
    {
        if (expect(TokenType::SPECIAL_CHARACTER).lexeme != "@")
        {
            syntax_error();
        }

        entry.datatype = expect(TokenType::KEYWORD).lexeme;

        if (entry.datatype != "adad" && entry.datatype != "khali")
        {
            syntax_error();
        }

        if (!isFunction)
            addSymbolTableEntry(entry);
        return true;
    }
    else if (_lexer.peek(1).tokenType == TokenType::ASSIGNMENT_OPERATOR) // unknown type on compile time.
    {
        addSymbolTableEntry(entry);
        return false;
    }
}


void parser::checkFunctionDeclaration()
{
    if (!checkIdentifierDatatypePair(false, true))
    {
        syntax_error();
    }

    // If everything is good above, the code below will continue..
    token t;
    if (expect(TokenType::SPECIAL_CHARACTER).lexeme != "(")
    {
        syntax_error();
    }
    
    // Check all arguments
    if (_lexer.peek(1).tokenType == TokenType::IDENTIFIER)
    {
        if (!checkIdentifierDatatypePair(false, false))
        {
            syntax_error();
        }

        while (_lexer.peek(1).lexeme == "|") 
        { 
            expect(TokenType::SPECIAL_CHARACTER);
            if (!checkIdentifierDatatypePair(false, false))
            {
                syntax_error();
            }
        }
    }
    
    if (expect(TokenType::SPECIAL_CHARACTER).lexeme != ")")
    {
        syntax_error();
    }

    if (expect(TokenType::KEYWORD).lexeme != "karo") {
        syntax_error();
    }

}

void parser::addTACVariables(string id_name, string datatype)
{
    symbol_table_entry entry;
    entry.id_name = id_name;
    entry.datatype = datatype;
    entry.type = "var";
    entry.initialValue = "0";
    entry.address = to_string(address);

    this->_symbol_table.push_back(entry);
    address += 1;
}

void parser::addEntryInitialValue(string value) 
{
    int lastIndex = this->_symbol_table.size() - 1;

    // This is where the last initial value is kept.

    _symbol_table.at(lastIndex).initialValue = value;
}

void parser::checkVariableDeclaration(bool flag_call) 
{
    bool flag = checkIdentifierDatatypePair(true, false);

    // Variables might be initialized..

    if (_lexer.peek(1).tokenType == TokenType::ASSIGNMENT_OPERATOR)
    {
        // After this, there are a number of possibilities that can happen.
        expect(TokenType::ASSIGNMENT_OPERATOR);

       // token t = _lexer.getNextToken();

        if (_lexer.peek(1).tokenType == TokenType::KEYWORD && _lexer.peek(1).lexeme == "chalao") // No @ was there
        {
            cout << "Chalao encountered rakho" << endl;
            expect(TokenType::KEYWORD);
            checkFunctionCallSyntax();
            return; 
        }
        else
        {
            while (true)
            {
                if (_lexer.peek(1).tokenType == TokenType::ARITHMETIC_OPERATOR)
                {
                    expect(TokenType::ARITHMETIC_OPERATOR);
                   // cout << "Current token: " << _lexer.getCurrentToken().lexeme << endl;
                    // cout << "Arithmetic OP" << endl;
                    //  Check both sides!
                    if (_lexer.getPrevToken().tokenType == TokenType::IDENTIFIER || _lexer.getPrevToken().tokenType == TokenType::NUMERIC_LITERAL
                        &&
                        _lexer.peek(1).tokenType == TokenType::IDENTIFIER || _lexer.peek(1).tokenType == TokenType::NUMERIC_LITERAL
                        )
                    {
                        cout << "Checking.." << endl;
                        if (_lexer.peek(1).tokenType == TokenType::IDENTIFIER)
                        {
                            cout << "adadada" << endl;
                            expect(TokenType::IDENTIFIER);
                        }
                        else
                        {
                            expect(TokenType::NUMERIC_LITERAL);
                        }
                    }
                    else
                    {
                        cout << "?" << endl;
                        syntax_error();
                    }
                }
                else if (_lexer.peek(1).tokenType == TokenType::IDENTIFIER)
                {
                    expect(TokenType::IDENTIFIER);

                }
                else if ( _lexer.peek(1).tokenType == TokenType::NUMERIC_LITERAL) 
                {
                    string val = expect(TokenType::NUMERIC_LITERAL).lexeme;
                    addEntryInitialValue(val);
                }
                else
                {
                    syntax_error();
                }

                if (_lexer.peek(1).tokenType == TokenType::SPECIAL_CHARACTER && _lexer.peek(1).lexeme == ";")
                {
                    break; // Exit from this statement.
                }
            }
        }
    }
    else 
    {
        //cout << _lexer.getCurrentToken().lexeme << endl;
        if (_lexer.getCurrentToken().lexeme != "adad" && _lexer.getCurrentToken().lexeme != "khali")
        {
            syntax_error();
        }
    }

   // cout << "fail: " + _lexer.getCurrentToken().lexeme << endl;
    if (expect(TokenType::SPECIAL_CHARACTER).lexeme != ";")
    {
        syntax_error();
    }

    if (flag_call) 
    {
        cout << "ind. call" << endl;
        emptyStack();
        cout << "finish" << endl;
    }
    else 
    {
        cout << "nested call.." << endl;
    }
}
void parser::printParseTree(vector<token> arr) {
    cout << "\n\n== Parse Tree For Function Call ==\n\n";
    int indexOfMainFuncInToken;
    bool funcFound = false;
    string searchStr = "markazi";
    cout << "markazi\n";
    int identationSum = 0;
    int indetationFac = 1;
    for (int i = 0; i < arr.size(); i++) {
        if (arr.at(i).lexeme == searchStr) {
            funcFound = true;
            identationSum += indetationFac;
        }
        else if (arr.at(i).lexeme == "chalao" && funcFound) {
            searchStr = arr.at(i + 1).lexeme;
            for (int f = 0; f < identationSum; f++) {
                cout << "   ";
            }
            cout << searchStr << endl;
            i = 0;
            funcFound = false;
        }
        else if (funcFound && arr.at(i).lexeme == "kaam" && arr.at(i + 1).lexeme == "khatam") {
            cout << "\n\nNo further func call." << endl;
            break;
        }
    }
}

void parser::checkInputVariableDeclaration()
{
    if (_lexer.peek(1).tokenType == TokenType::IDENTIFIER)
    {
        expect(TokenType::IDENTIFIER);

        if (_lexer.peek(1).tokenType == TokenType::SPECIAL_CHARACTER && _lexer.peek(1).lexeme == "@")
        {
            expect(TokenType::SPECIAL_CHARACTER);

            if (_lexer.peek(1).tokenType == TokenType::KEYWORD && _lexer.peek(1).lexeme == "adad")
            {
                expect(TokenType::KEYWORD);
            }
            else 
            {
                syntax_error();
            }
        }

        if (_lexer.peek(1).tokenType == TokenType::SPECIAL_CHARACTER && _lexer.peek(1).lexeme == ";")
        {
            expect(TokenType::SPECIAL_CHARACTER);
        }
        else
        {
            syntax_error();
        }
    }
}

void parser::checkStringSyntax() 
{
    if (_lexer.peek(1).tokenType == TokenType::SPECIAL_CHARACTER && _lexer.peek(1).lexeme == "`")
    {
        if (expect(TokenType::SPECIAL_CHARACTER).lexeme == "`")
        {
            if (_lexer.peek(1).tokenType == TokenType::STRING)
            {
                expect(TokenType::STRING);
            }

            if (expect(TokenType::SPECIAL_CHARACTER).lexeme == "`")
            {
            }
        }
    }
}

void parser::checkOutputSyntax() 
{
    if (expect(TokenType::IO_OPERATOR).lexeme == "<<")
    {
        while (true)
        {
            // Case 1: String
            checkStringSyntax();

            if (_lexer.peek(1).tokenType == TokenType::SPECIAL_CHARACTER && _lexer.peek(1).lexeme == ";")
            {
                expect(TokenType::SPECIAL_CHARACTER);
            //    cout << "End of Output: " << _lexer.getCurrentToken().lexeme << endl;
                return; // End of statement
            }
            else if (_lexer.peek(1).tokenType == TokenType::SPECIAL_CHARACTER)
            {
                syntax_error();
            }
            else if (_lexer.peek(1).tokenType == TokenType::IO_OPERATOR)
            {
                expect(TokenType::IO_OPERATOR);
            }
            else
            {
                expect(TokenType::IDENTIFIER);
            }
        }
    }
}

void parser::checkReturnSyntax()
{
    if (expect(TokenType::KEYWORD).lexeme == "bhaijo")
    {
        checkExpression();

        if (expect(TokenType::SPECIAL_CHARACTER).lexeme != ";")
        {
            syntax_error();
        }

        emptyStack();
    }
}

void parser::checkInputWithOutputSyntax()
{
    if (_lexer.peek(1).tokenType == TokenType::SPECIAL_CHARACTER)
    {
        if (_lexer.peek(1).lexeme == "`")
        {
            checkStringSyntax();
        }
    }
    else if (_lexer.peek(1).tokenType == TokenType::IDENTIFIER)
    {
        expect(TokenType::IDENTIFIER);
    }
    else if (_lexer.peek(1).tokenType == TokenType::NUMERIC_LITERAL)
    {
        checkExpression();
    }
}

void parser::checkInputSyntax()
{
//    cout << endl << endl << endl;
  //  cout << "CHECKING INPUT 12334" << endl;

    if (_lexer.peek(1).tokenType == TokenType::IO_OPERATOR && _lexer.peek(1).lexeme == "<<")
    {
        expect(TokenType::IO_OPERATOR);
        checkInputWithOutputSyntax();
    }

    if (expect(TokenType::IO_OPERATOR).lexeme == ">>")
    {
       // cout << "HIHIHI" << endl;
        checkInputVariableDeclaration();
    }

    emptyStack();
}

bool parser::actionsMenu()
{

    if (_lexer.peek(1).lexeme == "rakho")
    {
        expect(TokenType::KEYWORD);
        checkVariableDeclaration(false);
    }
    else if (_lexer.peek(1).lexeme == "dekhao")
    {
        expect(TokenType::KEYWORD);
        checkOutputSyntax();
       // return true;
    }
    else if (_lexer.peek(1).lexeme == "wapas")
    {
        expect(TokenType::KEYWORD);
        checkReturnSyntax();
       // return true;
    }
    else if (_lexer.peek(1).lexeme == "jab")
    {
        expect(TokenType::KEYWORD);
        checkLoopSyntax();
       // return true;
    }
    else if (_lexer.peek(1).lexeme == "lo")
    {
        expect(TokenType::KEYWORD);
        checkInputSyntax();
       // return true;
    }
    else if (_lexer.peek(1).lexeme == "chalao")
    {
        expect(TokenType::KEYWORD);
        checkFunctionCallSyntax();
      //  return true;
    }
    else if (_lexer.peek(1).tokenType == TokenType::COMMENT)
    {
        expect(TokenType::COMMENT);
    }
    else
    {
        return false;
    }

    return true;
}

void parser::checkIfControl()
{
    if (expect(TokenType::SPECIAL_CHARACTER).lexeme == "(")
    {
        checkConditions(); // Check enclosed conditions
       // _lexer.getNextToken();
        if (expect(TokenType::SPECIAL_CHARACTER).lexeme == ")")
        {
            if (expect(TokenType::KEYWORD).lexeme == "to")
            {
                if (expect(TokenType::KEYWORD).lexeme == "phir")
                {
                    if (expect(TokenType::KEYWORD).lexeme == "karo")
                    {
                        while (true)
                        {                  
                            // _lexer.getNextToken();
                            if (!actionsMenu()) // Weird token encountered
                            {
                                return;
                            }
                        }
                       
                    }
                }
            }
        }
    }
    else 
    {
        syntax_error();
    }
}

void parser::checkElseIfControl()
{
    if (expect(TokenType::SPECIAL_CHARACTER).lexeme == "(")
    {
        checkConditions(); // Check enclosed conditions
       // cout << "After checking: " << _lexer.getCurrentToken().lexeme << endl;

        if (expect(TokenType::SPECIAL_CHARACTER).lexeme == ")")
        {
            if (expect(TokenType::KEYWORD).lexeme == "to")
            {
                if (expect(TokenType::KEYWORD).lexeme == "phir")
                {
                    while (true)
                    {
                       // cout << "else if.." << endl;
                        // _lexer.getNextToken();
                        if (!actionsMenu()) // Weird token encountered
                        {
                          //  cout << "EXITING" << endl;
                            return;
                        }
                    }
                }
            }
        }
    }
}

void parser::checkElseControl()
{
   // cout << "Else control" << endl;
    while (true)
    {
     //   cout << "stuck in else...." << endl;
     //   _lexer.getNextToken();
        if (!actionsMenu()) // Weird token encountered
        {
            return;
        }
    }
}

void parser::checkFunctionCallSyntax()
{
    expect(TokenType::IDENTIFIER); // Function name should be followed from chalao

    if (expect(TokenType::SPECIAL_CHARACTER).lexeme == "(")
    {
        // Now check the arguments passed!

        while (true)
        {
            if (_lexer.peek(1).tokenType == TokenType::IDENTIFIER)
            {
                expect(TokenType::IDENTIFIER);
            }
            else if (_lexer.peek(1).tokenType == TokenType::SPECIAL_CHARACTER)
            {
                if (_lexer.peek(1).lexeme == "|")
                {
                    expect(TokenType::SPECIAL_CHARACTER);
                    continue;
                }
                else if (_lexer.peek(1).lexeme == ")")
                {
                    break;
                }
                else
                {
                    syntax_error();
                }
            }
            else if (_lexer.peek(1).tokenType == TokenType::NUMERIC_LITERAL)
            {
                expect(TokenType::NUMERIC_LITERAL);
            }
            else 
            {
                break;
            }
        }

        if (expect(TokenType::SPECIAL_CHARACTER).lexeme != ")")
        {
            syntax_error();
        }

        if (expect(TokenType::SPECIAL_CHARACTER).lexeme != ";")
        {
            syntax_error();
        }

        emptyStack();
        cout << "Done outputting!" << endl;
    }
}

void parser::checkIfBlockSyntax()
{
    checkIfControl();

    while (true) // Check all nested conditions
    {
        // _lexer.getNextToken();
        //cout << "1" << endl;
        if (_lexer.peek(1).lexeme == "warna")
        {
            expect(TokenType::KEYWORD);

            if (_lexer.peek(1).lexeme == "agar")
            {
                expect(TokenType::KEYWORD);
                checkElseIfControl();
            }
            else if (_lexer.peek(1).lexeme == "phir")
            {
                expect(TokenType::KEYWORD);
                checkElseControl();
            }
        }
        else if (_lexer.peek(1).lexeme == "bas")
        {
            expect(TokenType::KEYWORD);
            if (expect(TokenType::KEYWORD).lexeme != "karo")
            {
                syntax_error();
            }

            // return;
        }
        else if (_lexer.peek(1).lexeme == "jab")
        {
            expect(TokenType::KEYWORD);
            checkLoopSyntax();
        }
        else {
            break;
        }
       // cout << _lexer.getCurrentToken().lexeme << endl;
    }
    cout << "Token on exit: " << _lexer.getCurrentToken().lexeme << endl;
    emptyStack();
}

void parser::checkFunctionSyntax() 
{
    checkFunctionDeclaration(); //First check the function declaration and arguments.
    // Now check function start.

    emptyStack();
   // exit(0);
    // token t = _lexer.getCurrentToken();
    // cout << t.lexeme << endl;

    
    while (_lexer.peek(1).lexeme != "kaam")
    {        
        if (_lexer.peek(1).tokenType == TokenType::END_OF_FILE)
        {
            syntax_error(); // function end token was never found.
        }

        // Check other statements here

        // Case 1 : Variable Declaration
        if (_lexer.peek(1).lexeme == "rakho")
        {
            cout << "Here..." << endl;
            expect(TokenType::KEYWORD);
            checkVariableDeclaration(true);
        }
        // Case 2 : If statements
        else if (_lexer.peek(1).lexeme == "agar")
        {
            expect(TokenType::KEYWORD);
            checkIfBlockSyntax();
           // emptyStack();
           cout << "Exited the agar function & check!" << endl;
           cout << _lexer.getCurrentToken().lexeme << endl;
           cout << _lexer.peek(1).lexeme << endl;
        }
        // Case 3 : Output statements
        else if (_lexer.peek(1).lexeme == "dekhao")
        {
            expect(TokenType::KEYWORD);
            checkOutputSyntax();
        }
        else if (_lexer.peek(1).lexeme == "wapas")
        {
            cout << "Here.. in wapas" << endl;
            expect(TokenType::KEYWORD);
            checkReturnSyntax();
        }
        else if (_lexer.peek(1).lexeme == "lo")
        {
            expect(TokenType::KEYWORD);
            checkInputSyntax();
        }
        else if (_lexer.peek(1).tokenType == TokenType::COMMENT)
        {
            expect(TokenType::COMMENT);
            continue;
        }
        else if (_lexer.peek(1).lexeme == "jab")
        {
            expect(TokenType::KEYWORD);
            checkLoopSyntax();
        }
        else 
        {
            syntax_error();
        }

       // cout << _lexer.getCurrentToken().lexeme << endl;
      //  t = _lexer.getNextToken();
      //  continue;
    }

    if (expect(TokenType::KEYWORD).lexeme != "kaam") {
        syntax_error();
    }

    if (expect(TokenType::KEYWORD).lexeme != "khatam")
    {
        syntax_error();
    }
    // Else, OK.
}

void parser::printSymbolTable()
{
    ofstream symbol_file = ofstream("symbol_table.txt");
    if (symbol_file.is_open())
    {
        string ID = "ID";
        string Type = "Type";
        string Datatype = "Datatype";
        string InitialValue = "Initial Value";

        symbol_file << ID;
        int spacing = 25;

        symbol_file << setw(spacing - ID.length()) << right << "Type";
        symbol_file << setw(spacing - Type.length()) << right << "Datatype";
        symbol_file << setw(spacing - Datatype.length()) << right << "Initial Value";
        symbol_file << setw(spacing - InitialValue.length()) << right << "Address" << endl;

        for (int i = 0; i < _symbol_table.size(); i++)
        {
            symbol_file << _symbol_table[i].id_name;
            symbol_file << std::setw(spacing - _symbol_table[i].id_name.length()) << std::right << _symbol_table[i].type;
            symbol_file << std::setw(spacing - Type.length()) << std::right << _symbol_table[i].datatype;
            symbol_file << std::setw(spacing - Datatype.length()) << std::right << _symbol_table[i].initialValue;
            symbol_file << std::setw(spacing - InitialValue.length()) << std::right << _symbol_table[i].address << endl;

        }
        symbol_file.close();
    }
}

void parser::printLocalStack() {
    cout << endl;
    for (int i = 0; i < local_tokens.size(); i++) {
        cout << local_tokens[i].lexeme << endl;
    }

    cout << endl << endl;
}

vector<token> parse_conditions(vector<token> local_tokens, int index) 
{
    vector<token> l_tokens = vector<token>();

    while (local_tokens[index].lexeme == "(")
    {
        index++;
    }

    while (local_tokens[index].lexeme != ")")
    {
        l_tokens.push_back(local_tokens[index]);
        index++;
    }

    if (local_tokens[index + 1].lexeme != "to" && local_tokens[index + 1].lexeme != "karo")
    {
        index++;
        while (local_tokens[index].lexeme != ")")
        {
            l_tokens.push_back(local_tokens[index]);
            index++;
        }
    }

    return l_tokens;
}

void parser::processTAC(int currentLineNo, int blockSize)
{
    if (currentLineNo >= blockSize) 
    {
        return;
    }

    cout << "PIT: " << local_tokens[currentLineNo].lexeme << endl;

    // cout << currentLineNo << " " << blockSize << endl;
    // cout << local_tokens[currentLineNo].lexeme << endl;

    string tac_line = "";
    int index = currentLineNo + 1;

    if (local_tokens[currentLineNo].lexeme == "agar" && local_tokens[currentLineNo].tokenType == TokenType::KEYWORD) 
    {
        tac_line = "";

        vector<token> parsed_condition = parse_conditions(local_tokens, currentLineNo + 1);

        tac_line = "if";
        for (int i = 0; i < parsed_condition.size(); i++)
        {
            tac_line += " " + parsed_condition[i].lexeme;
        }
        tac_line += " goto";

        this->tac.push_back(tac_entry(tac_line, this->tac.size() + 1));
        processTAC(currentLineNo + 1, blockSize);
    } 
    else if (local_tokens[currentLineNo].lexeme == "dekhao" && local_tokens[currentLineNo].tokenType == TokenType::KEYWORD) 
    {
        int localIndex = currentLineNo + 1;

        while (local_tokens[localIndex].lexeme == "<<" && local_tokens[localIndex].tokenType == TokenType::IO_OPERATOR) 
        {
            tac_line = "";

            if (local_tokens[localIndex + 1].lexeme == "`" && local_tokens[localIndex + 1].tokenType == TokenType::SPECIAL_CHARACTER)
            {
                // cout << "Investigating.." << endl;
                tac_line += "out " + string("\"") + local_tokens[localIndex + 2].lexeme + string("\"");
                localIndex = localIndex + 4;
                // cout << tac_line << endl;
            }
            else
            {
                tac_line += "out "+ local_tokens[localIndex + 1].lexeme;
                localIndex = localIndex + 2;
                //cout << "Checking: " << local_tokens[localIndex].lexeme << endl;
            }
        

            this->tac.push_back(tac_entry(tac_line, this->tac.size() + 1));
        }
        processTAC(currentLineNo + 1, blockSize);
    }

    else if (local_tokens[currentLineNo].lexeme == "wapas" && local_tokens[currentLineNo].tokenType == TokenType::KEYWORD) 
    {
        tac_line = "";
        tac_line += "ret " + local_tokens[currentLineNo + 2].lexeme;

        this->tac.push_back(tac_entry(tac_line, this->tac.size() + 1));
        cout << "end";
        processTAC(currentLineNo + 1, blockSize);
    }
    else if (local_tokens[currentLineNo].lexeme == "warna" && local_tokens[currentLineNo].tokenType == TokenType::KEYWORD) 
    {
        cout << "AM I HERE?" << endl;
        if (local_tokens[currentLineNo + 1].lexeme == "agar" && local_tokens[currentLineNo + 1].tokenType == TokenType::KEYWORD) 
        {
            vector<token> parsed_condition = parse_conditions(local_tokens, currentLineNo + 2);

            tac_line = "if";
           // cout << "SIZE:" << parsed_condition.size() << endl;
            for (int i = 0; i < parsed_condition.size(); i++)
            {
                tac_line += " " + parsed_condition[i].lexeme;
            }
            tac_line += " goto";

            this->tac.push_back(tac_entry(tac_line, this->tac.size() + 1));
            processTAC(currentLineNo + 2, blockSize); // So agar is not encountered!
        }
        else if (local_tokens[currentLineNo + 1].lexeme == "phir" && local_tokens[currentLineNo + 1].tokenType == TokenType::KEYWORD)
        {
            tac_line = "";
            tac_line = "wphir";
            this->tac.push_back(tac_entry(tac_line, this->tac.size() + 1));
            processTAC(currentLineNo + 2, blockSize); // So agar is not encountered!
        }
    }
    else if (local_tokens[currentLineNo].lexeme == "rakho" && local_tokens[currentLineNo].tokenType == TokenType::KEYWORD)
    {
        if (local_tokens[currentLineNo + 4].lexeme == ":=" && local_tokens[currentLineNo + 4].tokenType == TokenType::ASSIGNMENT_OPERATOR) 
        {
            tac_line = "";

            if (local_tokens[currentLineNo + 5].lexeme == "chalao")
            {
                // Push the function call
                // Get the arguments
                vector<token> args = vector<token>();
                int l_index = currentLineNo + 7; // opening bracke

                while (local_tokens[l_index].lexeme != ")") // until closing bracket seen
                {
                    if (local_tokens[l_index].tokenType != TokenType::SPECIAL_CHARACTER)
                    {
                        args.push_back(local_tokens[l_index]);
                    }
                    l_index++;
                }

                for (int i = 0; i < args.size(); i++)
                {
                    tac_line = "";
                    tac_line = "param " + args[i].lexeme;
                    this->tac.push_back(tac_entry(tac_line, this->tac.size() + 1));
                }

                tac_line = "";


                newVarCount++;
                tac_line += "call " + local_tokens[currentLineNo + 6].lexeme + ", " + to_string(args.size()) + ", t" + to_string(newVarCount);
                this->tac.push_back(tac_entry(tac_line, this->tac.size() + 1));
                string new_var = "t" + to_string(newVarCount);
                addTACVariables(new_var, "adad");

                tac_line = "";
                tac_line += local_tokens[currentLineNo + 1].lexeme + " = " + "t" + to_string(newVarCount);
                this->tac.push_back(tac_entry(tac_line, this->tac.size() + 1));

                processTAC(currentLineNo + 6, blockSize); // Big jump because we already have seen the chalao sequence
            }
            else 
            {
                // What if after the assignment operator, there are multiple variables?
                tac_line = "";
                int y = currentLineNo + 5; // Address where the first variable/literal should be
                int expressionFlag = false;
                int l_index = y;
                int count = 0;
                vector<string> newVars = vector<string>();

                while (local_tokens[l_index].lexeme != ";")
                {
                    count++;
                    l_index++;
                }

                l_index = y;

                if (count > 3)
                {
                    float res = float(count) / 3.0;
                    int totalVar = int(round(res));
                    // cout << "Total Var Needed: " << totalVar << endl;

                    string firstVar = "t" + to_string(this->newVarCount + 1) + " = " + local_tokens[l_index].lexeme + " " + local_tokens[l_index + 1].lexeme + " " + local_tokens[l_index + 2].lexeme;
                    l_index += 3;
                    this->newVarCount++;

                    string var_name = "t" + to_string(this->newVarCount);
                    addTACVariables(var_name, "adad");
                    
                    this->tac.push_back(tac_entry(firstVar, this->tac.size() +  1));

                    if (count == 5) 
                    {
                        totalVar--;
                    }


                    for (int i = 1; i <= totalVar; i++)
                    {
                        cout << "nextVar: " << "t" << to_string(this->newVarCount + 1) << " + " << to_string(this->newVarCount + 1) << "var" << endl;
                        firstVar = "t" + to_string(this->newVarCount + 1) + " = " + "t" + to_string(this->newVarCount) + " " + local_tokens[l_index].lexeme + " " + local_tokens[l_index + 1].lexeme;
                        this->tac.push_back(tac_entry(firstVar, this->tac.size() +  1));

                        l_index += 2;
                        this->newVarCount++;
                        var_name = "t" + to_string(this->newVarCount);
                        addTACVariables(var_name, "adad");

                    }

                    int lastGeneratedVar = this->newVarCount;
                    tac_line = local_tokens[currentLineNo + 1].lexeme + " = " + "t" + to_string(lastGeneratedVar);
                    this->tac.push_back(tac_entry(tac_line, this->tac.size() + 1));
                }
                else 
                {
                    tac_line += local_tokens[currentLineNo + 1].lexeme + " = " + local_tokens[currentLineNo + 5].lexeme;

                    this->tac.push_back(tac_entry(tac_line, this->tac.size() + 1));
                    processTAC(currentLineNo + 1, blockSize); 
                }

                
                
            }
        }
        else 
        {
            processTAC(currentLineNo + 1, blockSize); 
        }

    }
    else if (local_tokens[currentLineNo].lexeme == "jab" && local_tokens[currentLineNo].tokenType == TokenType::KEYWORD) 
    {
        vector<token> parsed_condition = parse_conditions(local_tokens, currentLineNo + 2);

        tac_line = "loop";
        cout << "SIZE:" << parsed_condition.size() << endl;
        for (int i = 0; i < parsed_condition.size(); i++)
        {
            tac_line += " " + parsed_condition[i].lexeme;
        }
        tac_line += " goto";

        this->tac.push_back(tac_entry(tac_line, this->tac.size() + 1));
        processTAC(currentLineNo + 1, blockSize);

    }
    else if (local_tokens[currentLineNo].lexeme == "lo" && local_tokens[currentLineNo].tokenType == TokenType::KEYWORD)
    {
        int localIndex = currentLineNo + 1;

        while (local_tokens[localIndex].lexeme == "<<")
        {
            tac_line = "";
            tac_line += "out " + string("\"") + local_tokens[localIndex + 2].lexeme + string("\"");
            this->tac.push_back(tac_entry(tac_line, this->tac.size() + 1));
            localIndex = localIndex + 4;
        }

        if (local_tokens[localIndex].lexeme == ">>")
        {
            tac_line = "";
            tac_line += "in " + local_tokens[localIndex + 1].lexeme;
            this->tac.push_back(tac_entry(tac_line, this->tac.size() + 1));
        }
    }
    else if (local_tokens[currentLineNo].lexeme == "bas" && local_tokens[currentLineNo].tokenType == TokenType::KEYWORD)
    {
        tac_line = "bas";

        tac.push_back(tac_entry(tac_line, this->tac.size() + 1));
        processTAC(currentLineNo + 1, blockSize);
    }
    else if (local_tokens[currentLineNo].lexeme == "chalao" && local_tokens[currentLineNo].tokenType == TokenType::KEYWORD)
    {
        // Push the function call
        // Get arguments?

        vector<token> args = vector<token>();
        int l_index = currentLineNo + 2; // opening bracke
        
        while (local_tokens[l_index].lexeme != ")") // until closing bracket seen
        {   
            if (local_tokens[l_index].tokenType != TokenType::SPECIAL_CHARACTER)
            {
                args.push_back(local_tokens[l_index]);
            }
            l_index++;
        }

        for (int i = 0; i < args.size(); i++)
        {
            tac_line = "";
            tac_line = "param " + args[i].lexeme;
            this->tac.push_back(tac_entry(tac_line, this->tac.size() + 1));
        }

        tac_line = "";

        tac_line += "call " + local_tokens[currentLineNo + 1].lexeme + ", " + to_string(args.size());
        this->tac.push_back(tac_entry(tac_line, this->tac.size() + 1));
        processTAC(currentLineNo + 1, blockSize);
    }
    else if (local_tokens[currentLineNo].lexeme == "kaam" && local_tokens[currentLineNo + 1].tokenType == TokenType::IDENTIFIER)
    {
        symbol_table_entry entry;
        entry.id_name = local_tokens[currentLineNo + 1].lexeme;
        entry.address = to_string(this->tac.size() + 1);
        entry.datatype = "adad";
        entry.type = "func";
        entry.initialValue = "0";

        addSymbolTableEntry(entry);

        processTAC(currentLineNo + 1, blockSize);
    }
    else
    {
        processTAC(currentLineNo + 1, blockSize);
    }
}

void parser::convertToTAC() 
{
    string tac_line = "";
    struct tac_entry newEntry;
    processTAC(0, local_tokens.size());

    // For debug
    string test_tac_line = "";
    for (int i = 0; i < local_tokens.size(); i++) {
        test_tac_line += local_tokens[i].lexeme;
    }
    this->test_tac.push_back(test_tac_line);
}

void parser::generateTAC() {
    for (int i = 0; i < this->test_tac.size(); i++)
    {
        cout << test_tac[i] << endl;
    }


    for (int i = 0; i < this->tac.size(); i++) {
        cout << tac[i].lineNo << ". " << tac[i].tac_code << endl;
    }
}

void parser::emptyStack() 
{
    convertToTAC();
    printLocalStack();
    this->local_tokens.clear();
    // exit(1);
}

void parser::cleanTAC() 
{
    vector<tac_entry> cleanedTAC = vector<tac_entry>();
    vector<tac_entry> if_TACs = vector<tac_entry>();

    cout << endl << endl;
    cout << "Cleaned TAC:" << endl;

    // i-1 is "what I have seen" from the main vector (total size traversed)
    // size of cleanedTAC is everything that I have seen and inserted.
    // the difference of the two sizes will be the line no diff

    for (int i = 0; i < this->tac.size(); i++)
    {
        if (tac[i].tac_code.find("if") != string::npos)
        {
            string newTac = "goto ";
            
            // find the first bas or another if statement
            int relIndex = -1;

            for (int j = i + 1; j < this->tac.size(); j++)
            {
                if (tac[j].tac_code.find("if") != string::npos || tac[j].tac_code.find("bas") != string::npos || tac[j].tac_code.find("wphir") != string::npos)
                {
                    relIndex = j;
                    break;
                }
            }

            int lineDiff = cleanedTAC.size() - (i - 1);

            newTac += to_string(tac[relIndex].lineNo + lineDiff);
            
            cleanedTAC.push_back(tac_entry(this->tac[i].tac_code, (cleanedTAC.size() + 1)));
            cleanedTAC.push_back(tac_entry(newTac, (cleanedTAC.size() + 1)));
        }
        else if (tac[i].tac_code.find("loop") != string::npos) 
        {
            string newTac = "goto ";
            
            // find the first bas or another if statement
            int relIndex = -1;

            for (int j = i + 1; j < this->tac.size(); j++)
            {
                if (tac[j].tac_code.find("if") != string::npos || tac[j].tac_code.find("bas") != string::npos)
                {
                    relIndex = j;
                    break;
                }
            }

            int lineDiff = cleanedTAC.size() - (i - 1);

            newTac += to_string(tac[relIndex].lineNo + lineDiff);
            
            cleanedTAC.push_back(tac_entry(this->tac[i].tac_code, (cleanedTAC.size() + 1)));
            cleanedTAC.push_back(tac_entry(newTac, (cleanedTAC.size() + 1)));
        }
        else 
        {
            int lineDiff = cleanedTAC.size() - (i - 1);
            if (tac[i].tac_code != "wphir" && tac[i].tac_code != "bas")
            {
                cleanedTAC.push_back(tac_entry(this->tac[i].tac_code, (cleanedTAC.size() + 1)));
            }
        }
        
    }

    // Clean this further.
    for (int i = 0; i < cleanedTAC.size(); i++)
    {
        if (cleanedTAC[i].tac_code.find("if") != string::npos || cleanedTAC[i].tac_code.find("loop") != string::npos)
        {
            cleanedTAC[i].tac_code += " " + to_string(cleanedTAC[i+1].lineNo + 1);
        }        
    }

    this->tac = cleanedTAC;

    // Debug
    for (int i = 0; i < tac.size(); i++)
    {
        cout << tac[i].lineNo << ". " << tac[i].tac_code << endl;
    }
}

void parser::checkLoopSyntax()
{
  //  cout << "IN LOOP SYNTAX" << endl;
    if (expect(TokenType::KEYWORD).lexeme == "tak")
    {
        if (expect(TokenType::SPECIAL_CHARACTER).lexeme == "(")
        {
            checkConditions();

            if (expect(TokenType::SPECIAL_CHARACTER).lexeme == ")")
            {
                if (expect(TokenType::KEYWORD).lexeme == "karo")
                {
                    while (true)
                    {
                       // _lexer.getNextToken();
                        if (!actionsMenu())
                        {
                          //  cout << "Breaking.." << endl;
                            break;
                        }
                    }
                }
            }

           // cout << "BROKEN FROM ACTION MENU: " << _lexer.getCurrentToken().lexeme << endl;

         //   cout << "Before entering: " << _lexer.getCurrentToken().lexeme << endl;

            if (_lexer.peek(1).lexeme == "bas")
            {
                expect(TokenType::KEYWORD);
             //   cout << "INNER BAS ENCOUNTERED!" << endl;
                if (expect(TokenType::KEYWORD).lexeme != "karo")
                {
                    syntax_error();
                }
            }
            else {
                syntax_error();
            }
        }
    }
}

void parser::writeTAC()
{
    ofstream tac_file = ofstream("TAC.tac");
    if (tac_file.is_open())
    {
        for (int i = 0; i < this->tac.size(); i++)
        {
            tac_file << string(to_string(tac[i].lineNo) + ". " + tac[i].tac_code + "\n");
        }
        tac_file.close();
    }
}

void parser::run_parser()
{
    // _lexer.printTokens();
    // exit(0);


    // token t;
    // t = _lexer.getNextToken();
    bool mainFuncFound = false; // Set when markazi is found!


    while (_lexer.peek(1).lexeme == "kaam")
    {
        // t.Print();
        // Check the function condition
        if (expect(TokenType::KEYWORD).lexeme == "kaam")
        {
            checkFunctionSyntax(); // This function checks that the correct syntax for variable declaration is followed
        }
    }

    generateTAC();
    cleanTAC();
    writeTAC();
    printSymbolTable();

}

void parser::addSymbolTableEntry(symbol_table_entry entry)
{
    _symbol_table.push_back(entry);
}

//this function is for sample purposes only
/*
bool parser::statements()
{
    //statements-- > COLON LPAREN start RPAREN
    if (_lexer.peek(1).tokenType == TokenType::COLON)
    {
        expect(TokenType::COLON);
        if (_lexer.peek(1).tokenType == TokenType::LPAREN)
        {
            expect(TokenType::LPAREN);
            start();
            if (_lexer.peek(1).tokenType == TokenType::RPAREN)
            {
                expect(TokenType::RPAREN);
                return true;
            }
        }
    }
    return false;
}*/ 