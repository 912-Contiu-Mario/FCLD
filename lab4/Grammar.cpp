#include "Grammar.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <set>
using namespace std;

bool Grammar::addToSet(std::set<std::string>& targetSet, const std::set<std::string>& sourceSet)
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
                cerr << "Line "<< lineNum <<" Start symbol must be non-terminal" << endl;
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

            for(string lhsSymbol: lhsSymbols) {
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
            productions[lhs].push_back(splitProduction(rhs));
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
    for (auto it = productions.begin();it != productions.end(); ++it) {
        const string& currentNonTerminal = it->first;
        const vector<vector<string>>& productionList = it->second;

        for (auto prodIt = productionList.begin();prodIt != productionList.end(); ++prodIt) {
            cout << currentNonTerminal << " -> ";
            for (auto symbolIt = prodIt->begin();symbolIt != prodIt->end(); ++symbolIt) {
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
    for (auto prodIt = productionList.begin();prodIt != productionList.end(); ++prodIt) {
        cout << nonTerminal << " -> ";
        for (auto symbolIt = prodIt->begin();symbolIt != prodIt->end(); ++symbolIt) {
            cout << *symbolIt << " ";
        }
        cout << endl;
    }
}

bool Grammar::checkCFG() const {
    
    return isCFG;
}


std::set<std::string> Grammar::getFirst(const std::string& symbol) {
	std::set<std::string> firstSet;
    if (isTerminal(symbol)) {
		firstSet.insert(symbol);
	}
    else {
        for (const std::vector<std::string>& production : productions[symbol]) {
			bool hasEpsilon = true;
            for (const std::string& prodSymbol : production) {
                if (isTerminal(prodSymbol)) {
					firstSet.insert(prodSymbol);
					hasEpsilon = false;
					break;
				}
                else {
					std::set<std::string> firstProd = getFirst(prodSymbol);
                    if (!firstProd.count("$")) {
						hasEpsilon = false;
					}
					addToSet(firstSet, firstProd);
				}
			}
            if (hasEpsilon) {
				firstSet.insert("$");
			}
		}
	}
	return firstSet;
}

std::set<std::string> Grammar::getFollow(const std::string& symbol) {
	std::set<std::string> followSet;
    if (symbol == startSymbol) {
		followSet.insert("$");
	}
    for (const std::pair<std::string, std::vector<std::vector<std::string>>>& production : productions) {
        for (const std::vector<std::string>& prod : production.second) {
            for (size_t i = 0; i < prod.size(); i++) {
                if (prod[i] == symbol) {
                    if (i == prod.size() - 1) {
                        if (production.first != symbol) {
							addToSet(followSet, getFollow(production.first));
						}
					}
                    else {
						bool hasEpsilon = true;
                        for (size_t j = i + 1; j < prod.size(); j++) {
                            if (isTerminal(prod[j])) {
								followSet.insert(prod[j]);
								hasEpsilon = false;
								break;
							}
                            else {
								std::set<std::string> firstProd = getFirst(prod[j]);
                                if (!firstProd.count("$")) {
									hasEpsilon = false;
								}
								addToSet(followSet, firstProd);
							}
						}
                        if (hasEpsilon && production.first != symbol) {
							addToSet(followSet, getFollow(production.first));
						}
					}
				}
			}
		}
	}
	return followSet;
}

void Grammar::computeFirst() {
    firstSets.clear();
    for (const string& terminal : terminals) {
		firstSets[terminal].insert(terminal);
	}

    for (const string& nonTerminal : nonTerminals) {
		firstSets[nonTerminal] = getFirst(nonTerminal);
	}
}

void Grammar::computeFollow() {
	followSets.clear();
    for (const string& nonTerminal : nonTerminals) {
		followSets[nonTerminal] = getFollow(nonTerminal);
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
