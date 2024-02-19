#ifndef _PARSER_H_
#define _PARSER_H_
#include "lexer.h"
#include <iomanip>
//for future assignments leave it as it is

struct symbol_table_entry 
{
    string id_name;
    string type;
    string datatype;
    string initialValue;
    string address;
};

struct tac_entry 
{
    string tac_code;
    int lineNo;

    tac_entry() {}

    tac_entry(string tac_code, int lineNo) 
    {
        this->tac_code = tac_code;
        this->lineNo = lineNo;
    }
};

class parser
{
    lexer _lexer;
    vector<symbol_table_entry> _symbol_table = vector<symbol_table_entry>();
    int line_number = 0;
    vector<token> local_tokens = vector<token>();
    vector<tac_entry> tac = vector<tac_entry>();
    vector<string> test_tac = vector<string>();
    int newVarCount = 0;
    int address = 0;

    bool checkIdentifierDatatypePair(bool, bool);
    void checkFunctionDeclaration();
    void checkFunctionSyntax();


    void checkVariableDeclaration(bool);

    void checkStringSyntax();
    void checkOutputSyntax();

    void checkReturnSyntax();

    bool actionsMenu();
    
    void checkExpression();
    
    void checkConditions();
    void checkIfControl();
    void checkElseIfControl();
    void checkElseControl();
    void checkIfBlockSyntax();

    void checkLoopSyntax();

    void checkInputVariableDeclaration();
    void checkInputWithOutputSyntax();
    void checkInputSyntax();

    void checkFunctionCallSyntax();

    void addSymbolTableEntry(symbol_table_entry entry);
    void printSymbolTable();
    void addEntryInitialValue(string);
    void addTACVariables(string, string);
    void printParseTree(vector<token> arr);

    void printLocalStack();
    void emptyStack();
    void generateTAC(); // Generates TAC uptil what it has in the local stack!
    void convertToTAC();

    void writeTAC();
    void processTAC(int, int);
    void cleanTAC();

public:
    void syntax_error();
    token expect(TokenType expected_type);
    parser(const char filename[]);
    void readAndPrintAllInput();
    void resetPointer();
    void run_parser();
    /*Terminal functions goes here use peek and expect*/
    /*use TokenType:: for token names for example
        expect(TokenType::ASSIGN);   //example function call
      */
	//all your parser function goes here
};
#endif
