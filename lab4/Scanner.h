#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <regex>
#include "HashTable.h"
#include "FiniteAutomata.h"
#include "LexicalException.cpp"

class Scanner {
public:
	Scanner(std::string programPath);
	Scanner(std::ifstream programFile);
	void scan();
	void generateSTFile();
	void generatePIFFile();

private:
	std::ifstream programFile;
	HashTable symbolTable;
	FA finiteAutomataInteger;
	FA finiteAutomataIdentifier;
	std::vector<std::tuple<std::string, std::tuple<int,int>, int>> PIF;


	std::unordered_map<std::string, std::pair<std::string, int>> tokens;
	int currentLineNum;
	int currentCharNumPerLine;
	std::string currToken;

	bool processToken(std::string tokenToProcess);
	void genPIF(std::string token, std::tuple<int, int> pos, int code);
	bool isSeparatorOperatorReservedWord(std::string token);
	bool isSeparator(std::string token);
	bool isOperator(std::string token);
	bool isReservedWord(std::string token);
	bool isConstant(std::string token);
	bool isIdentifier(std::string token);
	void initTokens();
	std::string getTokenType(std::string token);
	int getTokenCode(std::string token);

};