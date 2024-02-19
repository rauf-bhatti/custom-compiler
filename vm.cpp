#include <fstream>
#include <vector>
#include <iostream>
#include <string>
#include <cctype>
#include <cstring>
#include <sstream>
using namespace std;

class vm {
private:
	vector< vector<string> > tac_lines;
	vector< vector<string> > symbol_table;
	int callLineNo;

	vector<string> processTacStatement(string tacLine)
	{
		vector<string> toReturn;
		string temp;

		int c = 0;

		while (tacLine[c] != '.') 
		{
			temp += tacLine[c];
			c++;
		}

		toReturn.push_back(temp);

		c += 2;
		temp = "";

		while (tacLine[c] != '\0')
		{
			if (tacLine[c] != ' ')
			{
				temp += tacLine[c];
			}
			else 
			{
				toReturn.push_back(temp);
				temp = "";
			}

			c++;
		}

		toReturn.push_back(temp);

		return toReturn;
	}

	void print2DVector(vector< vector<string> > _2dVec) 
	{
		for (int i = 0; i < _2dVec.size(); i++)
		{
			vector<string> innerLine = _2dVec[i];
			for (int j = 0; j < innerLine.size(); j++) 
			{
				cout << innerLine[j] << endl;
			}
			cout << endl;
		}
	}

	void cleanLines()
	{
		for (int i = 0; i < tac_lines.size(); i++)
		{
			vector<string> innerLine = tac_lines[i];

			if (innerLine[1] == "out")
			{
				vector<string> newInnerLine;

				if (innerLine[2][0] == '\"')
				{
					newInnerLine.push_back(innerLine[0]);
					newInnerLine.push_back(innerLine[1]);

					string line = "";

					for (int j = 2; j < innerLine.size(); j++)
					{
						line += innerLine[j] + " ";
					}

					newInnerLine.push_back(line);
				}

				tac_lines[i] = newInnerLine;
			}
			else if (innerLine[1] == "call")
			{
				vector<string> newInnerLine;
				//cout << "Here" << endl;
				
				for (int j = 2; j < innerLine.size(); j++)
				{
					int index = innerLine[j].find(',');

					if (index != -1)
					{
						innerLine[j].erase(index);
					}
				}
				tac_lines[i] = innerLine;
			}
		}
	}


	vector<string> processSymbolTableStatement(string line)
	{
		vector<string> toReturn;
		string entry;

		for (int i = 0; i < line.size(); i++)
		{
			if (line[i] == ' ' && line[i + 1] != ' ')
			{
				toReturn.push_back(entry);
				entry = "";
			}
			else
			{
				entry += line[i];
			}
		}
		toReturn.push_back(entry);

		return toReturn;
	}

	void loadSymbolTable()
	{

		ifstream symbol_table_file("symbol_table.txt");

		if (!symbol_table_file)
		{
			cout << "Symbol Table file not found...";
			exit(1);
		}
		else 
		{
			std::string line;
            while (std::getline(symbol_table_file, line))
            {
				symbol_table.push_back(processSymbolTableStatement(line));
                
            }
            symbol_table_file.close();

		}
	}
	
	void cleanSymbolTable() 
	{
		for (int i = 1; i < this->symbol_table.size(); i++)
		{
			for (int j = 0; j < this->symbol_table[i].size(); j++)
			{
				string line = "";

				for (int k = 0; k < this->symbol_table[i][j].size(); k++)
				{
					if (this->symbol_table[i][j][k] != ' ')
						line += this->symbol_table[i][j][k];
				}

				this->symbol_table[i][j] = line;
			}
		}
	}

	int locateFunction(string functionName)
	{
		for (int i = 0; i < this->symbol_table.size(); i++)
		{
			if (symbol_table[i][0] == functionName)
			{
				int n = stoi(symbol_table[i][4]);
				return n;
			}
		}

		return -1;
	}

	int lookupVariableAddress(string variable)
	{
		for (int i = 0; i < this->symbol_table.size(); i++)
		{
			if (symbol_table[i][0] == variable)
			{
				return stoi(symbol_table[i][4]);
			}
		}

		return -1;
	}

	int lookupVariableValue(string variable)
	{
		for (int i = 0; i < this->symbol_table.size(); i++)
		{
			if (symbol_table[i][0] == variable)
			{
				return stoi(symbol_table[i][3]);
			}
		}

		return 0;
	}

	void updateValueInSymbolTable(string variable, int value)
	{
		for (int i = 0; i < this->symbol_table.size(); i++)
		{
			if (symbol_table[i][0] == variable)
			{
				symbol_table[i][3] = to_string(value);
			}
		}
	}

	int findIndexOfRO(vector<string> l_quad)
	{
		for (int i = 0; i < l_quad.size(); i++)
		{
			if (l_quad[i] == "<" || l_quad[i] == ">" || l_quad[i] == ">="
				|| l_quad[i] == "<=" || l_quad[i] == "!=")
			{
				return i;
			}
		}
		return -1;
	}

	int findIndexOfAO(vector<string> l_quad, int start, int end)
	{
		for (int i = start; i < end; i++)
		{
			if (l_quad[i] == "-" || l_quad[i] == "+" || l_quad[i] == "*"
				|| l_quad[i] == "/")
			{
				return i;
			}
		}

		return -1;
	}

	bool checkIfVar(char c)
	{
		if (int(c) >= 48 && int(c) <= 57)
		{
			return false;
		}

		return true;
	}

	int getExpressionResult(vector<string> quad, int start, int end)
	{
		int indexAO = findIndexOfAO(quad, start, end);
		int expr_result = -99, var1Addr, var1Val, var2Addr, var2Val;

		if (indexAO != -1)
		{
			if (checkIfVar(quad[indexAO - 1][0]))
			{
				var1Addr = lookupVariableAddress(quad[indexAO - 1]);
				var1Val = lookupVariableValue(quad[indexAO - 1]);

				// cout << var1Addr << " " << var1Val << endl;

				if (checkIfVar(quad[indexAO + 1][0]))
				{
					var2Addr = lookupVariableAddress(quad[indexAO + 1]);
					var2Val = lookupVariableValue(quad[indexAO + 1]);
				}
				else
				{
					var2Val = stoi(quad[indexAO + 1]);
					// cout << "Quad: " << quad[indexAO + 1] << endl;
				}
			}

			string AO = quad[indexAO];

			if (AO == "+")
			{
				expr_result = var1Val + var2Val;
			}
			else if (AO == "-")
			{
				expr_result = var1Val - var2Val;
			}
			return expr_result;
		} 
		else 
		{
			if (checkIfVar(quad[start][0]))
			{
				int varAddr = lookupVariableAddress(quad[start]);
				int varValue = lookupVariableValue(quad[start]);

				return varValue;
			}
			else
			{
				return stoi(quad[start]);
			}
		}
	}

	bool evaluateConditions(vector<string> quad)
	{
		int indexRO = findIndexOfRO(quad);
		int res = getExpressionResult(quad, 0, indexRO);
		int res2 = getExpressionResult(quad, indexRO + 1, quad.size());

		// cout << "res: " << res << endl;
		// cout << "res2: " << res2 << endl;

		bool resultFlag = false;
		string RO = quad[indexRO];

		if (RO == "<")
		{
			// cout << "I'm HERE!" << endl;
			// cout << "RES: " << res << endl;
			// cout << "Res2: " << res2 << endl;
			if (res < res2)
			{
				resultFlag = true;
			}
		}
		else if (RO == "=")
		{
			if (res == res2)
			{
				resultFlag = true;
			}
		}
		else if (RO == ">")
		{
			if (res > res2)
			{
				resultFlag = true;
			}
		}
		else if (RO == ">=")
		{
			if (res >= res2)
			{
				resultFlag = true;
			}
		}
		else if (RO == "<=")
		{
			if (res <= res2)
			{
				resultFlag = true;
			}
		}


		// cout << "result flag: " << resultFlag << endl;
		return resultFlag;
	}


public:

	vm(const char filename[])
    {
		this->callLineNo = -99;

        ifstream fin(filename);

        if (!fin) 
        {
            cout << "File not found!";
            exit(1);
        }
        else 
        {
            std::string line;
            while (std::getline(fin, line))
            {
				tac_lines.push_back(processTacStatement(line));
               // std::cout << line << std::endl;
            }
            fin.close();
        }
		cleanLines();
		loadSymbolTable();
		cleanSymbolTable();
    }

	void run() 
	{
		cout << "Now running..." << endl;
		//cout << locateFunction("markazi") << endl;
		// cout << this->tac_lines.size() << endl;

		int ar = locateFunction("markazi");
		// cout << activeRow << endl;
		vector<string> quad;

		int ds[100];

		while (ar <= this->tac_lines.size())
		{
			quad = tac_lines[ar - 1];

			// for (int i = 0; i < quad.size(); i++) {
			// 	cout << quad[i] << endl;
			// }
			
			// cout << endl;

			// cout << "Line currently executing: " << endl;
			// for (int i = 0; i < quad.size(); i++)
			// {
			// 	cout << quad[i] << " ";
			// }
			// cout << endl << endl;

			if (quad[1] == "out")
			{
				for (int i = 1; i < quad[2].size() - 2; i++)
				{
					cout << quad[2][i];
				}

				cout << endl;
			}
			else if (quad[1] == "call")
			{
				int func_addr = locateFunction(quad[2]);
				// cout << quad[2] << endl;
				callLineNo = ar;
				ar = func_addr;
				continue;
			}
			else if (quad[1] == "param")
			{

			}
			else if (quad[1] == "if")
			{	
				// cout << "In if" << endl;
				// for (int i = 1; i < quad.size(); i++)
				// {
				// 	cout << quad[i] << endl;
				// }
				bool resultFlag = evaluateConditions(quad);
				// cout << "Outside Result Flag: " << resultFlag << endl;

				if (resultFlag)
				{
					int quad_size = quad.size() - 1;
					int jump_to = stoi(quad[quad_size]);

					// cout << "JUMP TO: " << jump_to << endl;
					ar = jump_to;
					continue;
				}
				else 
				{
					// cout << "False" << endl;
					ar++;
					// vector<string> temp = tac_lines[ar -1];
					// for (int i = 0; i < temp.size(); i++) {
					// 	cout << temp[i] << endl;
					// }
					// cout << endl;
					continue;
				}
			}
			else if (quad[1] == "loop")
			{
				// for (int i = 1; i < quad.size(); i++)
				// {
				// 	cout << quad[i] << endl;
				// }

				bool resultFlag = evaluateConditions(quad);
				if (resultFlag)
				{
					int quad_size = quad.size() - 1;
					// cout << quad[quad_size] << endl;
					int jump_to = stoi(quad[quad_size]);

					// cout << "JUMP TO: " << jump_to << endl;
					ar = jump_to;
					continue;
				}
				else 
				{
					ar++;
					continue;
				}
			}
			else if (quad[1] == "goto")
			{
				// cout << "VAL:" << quad[2] << endl;
				ar = stoi(quad[2]);
				continue;
			}
			else if (quad[1] == "ret")
			{
				cout << "Ending execution.." << endl;
				exit(1);
			}
			else if (quad[1] == "in")
			{
				int addr = lookupVariableAddress(quad[2]);

				cin >> ds[addr];
			}
			else 
			{
				// cout << quad[1] << endl;

				if (int(quad[3][0]) >= 48 && int(quad[3][0]) <= 57)
				{
					int addr = lookupVariableAddress(quad[1]);
					int val = stoi(quad[3]);
					
					ds[addr] = val;
					// cout << "DS at: " << addr << ", value: " << ds[addr] << endl;
				}
				else 
				{
					if (quad.size() < 6)
					{
						int addr = lookupVariableAddress(quad[1]);
						ds[addr] = lookupVariableValue(quad[3]);

						// cout << "DS at: " << addr << ", value: " << ds[addr] << endl;
					}
					else 
					{
						int addr = lookupVariableAddress(quad[1]);

						string OP = quad[4];
						int res;

						int var1 = lookupVariableValue(quad[3]);
						int var2 = lookupVariableValue(quad[5]);

						// cout << "OP: " << OP << endl;
						// cout << quad[3] << " " << quad[5] << endl;

						if (OP == "+")
						{
							res = var1 + var2;
							// cout << "res: " << res << endl;
						}
						else if (OP == "-")
						{
							res = var1 - var2;
						}
						else if (OP == "*")
						{
							res = var1 * var2;
						}
						else if (OP == "/")
						{
							res = var1 / var2;
						}

						ds[addr] = res;
						// cout << "DS at: " << addr << ", value: " << ds[addr] << endl;
						updateValueInSymbolTable(quad[1], res);
					}
				}
			}
			ar++;
		}
	}	
};


int main(int argc, char* argv[])
{
	//asking for file name as command line arguments
	if (argc == 2)
	{
        vm _vm = vm(argv[1]);
		_vm.run();
	}
	else if (argc > 2)
	{ //argument limit exceeds
		cout << "Too many arguments" << endl;
	}
	else //if file name is'nt given
	{
		cout << "Please provide a file name" << endl;
	}
	return 0;
}
