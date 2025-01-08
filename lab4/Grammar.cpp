#include "Grammar.h"
using namespace std;

bool Grammar::addToSet(set<string>& targetSet, const set<string>& sourceSet)
{
	size_t oldSize = targetSet.size();
	targetSet.insert(sourceSet.begin(), sourceSet.end());
	return targetSet.size() > oldSize;

}

Grammar::Grammar() : startSymbol("") {}

bool Grammar::isTerminal(const string& symbol) const {
	return terminals.find(symbol) != terminals.end();
}

bool Grammar::isNonTerminal(const string& symbol) const {
	return nonTerminals.find(symbol) != nonTerminals.end();
}

vector<string> Grammar::splitProduction(const string& production) const {
	vector<string> symbols;
	istringstream iss(production);
	string symbol;

	while (iss >> symbol) {
		symbols.push_back(symbol);
	}

	return symbols;
}

bool Grammar::readFromFile(const string& filename) {
	ifstream file(filename);
	if (!file.is_open()) {

		//handle err
		return false;
	}

	nonTerminals.clear();
	terminals.clear();
	productions.clear();

	string line;
	// 0 non-terminals, 1 terminals, 2 start symbol, 3 productions
	int section = 0;
	int lineNum = 0;
	while (getline(file, line)) {
		if (line.empty() || line[0] == '#') continue;

		if (line == "$") {
			section++;
			continue;
		}

		switch (section) {
		case 0:
			nonTerminals.insert(line);
			break;

		case 1:
			terminals.insert(line);
			break;

		case 2:
			if (nonTerminals.find(line) != nonTerminals.end()) {
				startSymbol = line;
			}
			else {
				cerr << "Line " << lineNum << " Start symbol must be non-terminal" << endl;
				return false;
			}
			break;

		case 3:
		{
			size_t arrowPos = line.find("->");
			if (arrowPos == string::npos) {
				cerr << "Line " << lineNum << " Incorrect production format" << endl;
				return false;
			}

			string lhs = line.substr(0, arrowPos);

			vector<string> lhsSymbols = splitProduction(lhs);
			if (lhsSymbols.size() != 1 || nonTerminals.find(lhsSymbols[0]) == nonTerminals.end()) {
				isCFG = false;
			}

			for (string lhsSymbol : lhsSymbols) {
				if (terminals.find(lhsSymbol) == terminals.end() && nonTerminals.find(lhsSymbol) == nonTerminals.end()) {
					cerr << "Line " << lineNum << " Non defined symbol on lhs" << endl;
					return false;
				}
			}

			string rhs = line.substr(arrowPos + 2);
			vector<string> rhsSymbols = splitProduction(rhs);
			for (const string& symbol : rhsSymbols) {
				if (nonTerminals.find(symbol) == nonTerminals.end() && terminals.find(symbol) == terminals.end()) {
					cerr << "Line " << lineNum << " Non defined symbol(s) on rhs" << endl;
					return false;
				}
			}

			//if we get here lhsSymbols will have size 1 surely.
			productions[lhsSymbols[0]].push_back(splitProduction(rhs));
		}
		break;
		}

		lineNum++;

	}

	file.close();
	return true;
}

void Grammar::printNonTerminals() const {
	cout << "Non-terminals: ";
	for (const auto& nt : nonTerminals) {
		cout << nt << " ";
	}
	cout << endl;
}

void Grammar::printTerminals() const {
	cout << "Terminals: ";
	for (const auto& t : terminals) {
		cout << t << " ";
	}
	cout << endl;
}

void Grammar::printProductions() const {
	cout << "Productions:" << endl;
	for (auto it = productions.begin(); it != productions.end(); ++it) {
		const string& currentNonTerminal = it->first;
		const vector<vector<string>>& productionList = it->second;

		for (auto prodIt = productionList.begin(); prodIt != productionList.end(); ++prodIt) {
			cout << currentNonTerminal << " -> ";
			for (auto symbolIt = prodIt->begin(); symbolIt != prodIt->end(); ++symbolIt) {
				cout << *symbolIt << " ";
			}
			cout << endl;
		}
	}
}

void Grammar::printProductionsFor(const string& nonTerminal) const {
	auto it = productions.find(nonTerminal);
	if (it == productions.end()) {
		cout << "No productions for " << nonTerminal << endl;
		return;
	}

	cout << "Productions for " << nonTerminal << ":" << endl;
	const vector<vector<string>>& productionList = it->second;
	for (auto prodIt = productionList.begin(); prodIt != productionList.end(); ++prodIt) {
		cout << nonTerminal << " -> ";
		for (auto symbolIt = prodIt->begin(); symbolIt != prodIt->end(); ++symbolIt) {
			cout << *symbolIt << " ";
		}
		cout << endl;
	}
}

bool Grammar::checkCFG() const {
	return isCFG;
}

void Grammar::computeFirst() {
	firstSets.clear();

	for (const string& terminal : terminals) {
		firstSets[terminal].insert(terminal);
	}

	for (const string& nonTerminal : nonTerminals) {
		firstSets[nonTerminal] = set<string>();
	}

	bool changed = true;

	while (changed) {
		changed = false;

		for (const auto& productionPair : productions) {
			const string& A = productionPair.first;
			const auto& productionList = productionPair.second;

			for (const auto& production : productionList) {
				bool epsilonInAll = true;

				for (const string& symbol : production) {
					size_t beforeSize = firstSets[A].size();

					for (const string& symFirst : firstSets[symbol]) {
						if (symFirst != "e") {
							firstSets[A].insert(symFirst);
						}
					}

					if (firstSets[A].size() > beforeSize) {
						changed = true;
					}

					if (firstSets[symbol].find("e") == firstSets[symbol].end()) {
						epsilonInAll = false;
						break;
					}
				}

				if (epsilonInAll) {
					if (firstSets[A].find("e") == firstSets[A].end()) {
						firstSets[A].insert("e");
						changed = true;
					}
				}
			}
		}
	}
}


void Grammar::computeFollow() {
	followSets.clear();

	followSets[startSymbol].insert("e");

	bool changed = true;

	while (changed) {
		changed = false;

		for (const auto& productionPair : productions) {
			const string& A = productionPair.first;
			const vector<vector<string>>& productionList = productionPair.second;

			for (const auto& production : productionList) {
				for (size_t i = 0; i < production.size(); ++i) {
					const string& B = production[i];

					if (isNonTerminal(B)) {
						set<string> firstBeta;
						bool epsilonInBeta = true;

						for (size_t j = i + 1; j < production.size(); ++j) {
							const string& symbol = production[j];
							const set<string>& firstSetSymbol = firstSets[symbol];

							for (const string& symFirst : firstSetSymbol) {
								if (symFirst != "$") {
									firstBeta.insert(symFirst);
								}
							}

							if (firstSetSymbol.find("$") == firstSetSymbol.end()) {
								epsilonInBeta = false;
								break;
							}
						}

						if (i + 1 < production.size()) {
							size_t beforeSize = followSets[B].size();
							bool addedFirst = addToSet(followSets[B], firstBeta);
							if (addedFirst) {
								changed = true;
							}

							if (epsilonInBeta) {
								size_t beforeSizeFollow = followSets[B].size();
								bool addedFollow = addToSet(followSets[B], followSets[A]);
								if (addedFollow) {
									changed = true;
								}
							}
						}
						else {
							size_t beforeSizeFollow = followSets[B].size();
							bool addedFollow = addToSet(followSets[B], followSets[A]);
							if (addedFollow) {
								changed = true;
							}
						}
					}
				}
			}
		}
	}
}

void Grammar::printFirstSets() const {
	cout << "First sets:" << endl;
	for (const auto& firstSet : firstSets) {
		cout << firstSet.first << ": ";
		for (const string& symbol : firstSet.second) {
			cout << symbol << " ";
		}
		cout << endl;
	}
}

void Grammar::printFollowSets() const {
	cout << "Follow sets:" << endl;
	for (const auto& followSet : followSets) {
		cout << followSet.first << ": ";
		for (const string& symbol : followSet.second) {
			cout << symbol << " ";
		}
		cout << endl;
	}
}
