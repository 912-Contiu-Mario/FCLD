#include "Scanner.h"
using namespace std;


Scanner::Scanner(std::string programPath): symbolTable(100), programFile(programPath)
{
	initTokens();

	finiteAutomataIdentifier= FA("FA-identifier.in");
	finiteAutomataInteger= FA("FA-integer.in");
}


void Scanner::initTokens() {
	ifstream tokensFile("token.txt");
	string token;
	int index = 1;
	while (getline(tokensFile, token)) {
		if (token == "\\n") {
			token = "\n";
		}
		if (index <= 11) {
			this->tokens.insert({ token,{"operator", index}});
		}
		else if (index <= 22) {
			this->tokens.insert({ token,{"separator", index} });
		}
		else {
			this->tokens.insert({ token,{"reserved word", index} });
		}
		index++;
	}

	for (auto pair : this->tokens) {
		cout << "Token: " << pair.first << " -> type: " << pair.second.first << ", code: " << pair.second.second << endl;
	}
}

int Scanner::getTokenCode(std::string token)
{
	auto res = tokens.find(token);
	if (res != tokens.end())
		return res->second.second;
	if (this->isIdentifier(token))
		return 37;
	if (this->isConstant(token)) {
		return 38;
	}
	//throw exception
}


bool Scanner::processToken(std::string tokenToProcess)
{
	currToken = tokenToProcess;

	if (isSeparatorOperatorReservedWord(tokenToProcess)) {
		genPIF(tokenToProcess, { -1, -1 }, this->getTokenCode(tokenToProcess));
	}
	
	//it means it's an identifier or constant
	else {
		//check if it's identifier or constant
		if (isIdentifier(tokenToProcess)) {
			if(!symbolTable.exists(tokenToProcess)){
				symbolTable.insert(tokenToProcess);
			}
			
			tuple<int, int> pos = symbolTable.searchElem(tokenToProcess);
			genPIF(tokenToProcess, pos, getTokenCode(tokenToProcess));
			
		}
		else if (isConstant(tokenToProcess)) {
			if (!symbolTable.exists(tokenToProcess)) {
				symbolTable.insert(tokenToProcess);
			}
			tuple<int, int> pos = symbolTable.searchElem(tokenToProcess);
			genPIF(tokenToProcess, pos, getTokenCode(tokenToProcess));
			
		}
		else {
			string msg = "Line " + to_string(currentLineNum) + ": " + currToken + " is lexically incorrect";
			throw LexicalException(msg);
		}

	}
	return false;
}

void Scanner::genPIF(std::string token, tuple<int, int> pos, int code)
{
	if (token == "\t") {
		token = "TAB";
	}
	if(token == " "){
		token = "SPACE";
	}
	if (token == "\n") {
		token = "NEW_LINE";
	}
	PIF.push_back({ token, pos, code });
}

bool Scanner::isSeparatorOperatorReservedWord(std::string token)
{
	return tokens.find(token) != tokens.end();
}

bool Scanner::isSeparator(std::string token)
{
	auto res = tokens.find(token);
	return res != tokens.end() && res->second.first == "separator";
}

bool Scanner::isOperator(std::string token)
{
	auto res = tokens.find(token);
	return res != tokens.end() && res->second.first == "operator";
}

bool Scanner::isReservedWord(std::string token)
{
	auto res = tokens.find(token);
	return res != tokens.end() && res->second.first == "reserved word";
}

bool Scanner::isConstant(std::string token)
{
   std::regex integerRegex(R"(^[-+]?[0-9]+$)");
   std::regex booleanRegex(R"(^(true|false)$)");
   std::regex stringRegex(R"(^\".*\"$)");


   if (finiteAutomataInteger.checkIfConsistent(token)) {
	   return true;
   }
   if (regex_match(token, integerRegex)) {
	   return true;
   }
   if (regex_match(token, booleanRegex)) {
	   return true;
   }
   if (regex_match(token, stringRegex)) {
	   return true;
   }
   return false;
}


bool Scanner::isIdentifier(std::string token)
{

	return finiteAutomataIdentifier.checkIfConsistent(token);
	std::regex identifierRegex(R"(^[a-zA-Z][a-zA-Z0-9]*$)");

	if (regex_match(token, identifierRegex)) {
		return true;
	}
	return false;
}

void Scanner::generatePIFFile()
{
	ofstream file("PIF.out");

	auto it = this->PIF.begin();
	while (it != PIF.end()) {
		file << "(" << get<0>(*it) << ")" << "->(" << get<0>(get<1>(*it)) << "," << get<1>(get<1>(*it)) << ")" << " | " <<get<2>(*it) << endl;
		it++;
	}
}

void Scanner::generateSTFile()

{
	ofstream file("STF.out");
	file << symbolTable.display();
}



void Scanner::scan()
{
	if (!programFile) {
		//throw exception here
	}
	char currCharacter;
	string buffer;
	currentLineNum = 1;
	currentCharNumPerLine = 1;
	while (programFile.get(currCharacter)) {
		if (currCharacter == '\t') {
			continue;
		}
		string currStringChar(1, currCharacter);

		if (currCharacter == '<') {
			programFile.get(currCharacter);
			if (currCharacter == '<') {
				if (!buffer.empty())
				{
					processToken(buffer);
					buffer.clear();
				}
				processToken("<<");
			}
		}

		else if (currCharacter == '>') {
			programFile.get(currCharacter);
			if (currCharacter == '>') {
				if (!buffer.empty())
				{
					processToken(buffer);
					buffer.clear();
				}
				processToken(">>");
			}
		}

		else if (isSeparator(currStringChar)) {
			if (currCharacter == '\n') {
				currentLineNum++;
				currentCharNumPerLine = 1;
			}
			if (!buffer.empty())
			{
				processToken(buffer);
				buffer.clear();
			}
			processToken(currStringChar);
		}
		else {
			buffer += currCharacter;
		}
		currentCharNumPerLine++;
	}

}
