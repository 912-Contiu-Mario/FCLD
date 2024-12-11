#include "Grammar.h"
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std;

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
        cerr << "Error: Could not open file " << filename << endl;
        return false;
    }

    // Clear existing data
    nonTerminals.clear();
    terminals.clear();
    productions.clear();

    string line;
    int section = 0;  // 0: non-terminals, 1: terminals, 2: start symbol, 3: productions

    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;  // Skip empty lines and comments

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
                cerr << "Error: Start symbol must be a non-terminal" << endl;
                return false;
            }
            break;

        case 3:
        {
            size_t arrowPos = line.find("->");
            if (arrowPos == string::npos) {
                cerr << "Error: Invalid production format" << endl;
                return false;
            }

            string lhs = line.substr(0, arrowPos);
            lhs.erase(remove_if(lhs.begin(), lhs.end(), ::isspace), lhs.end());

            if (nonTerminals.find(lhs) == nonTerminals.end()) {
                cerr << "Error: LHS of production must be a non-terminal" << endl;
                return false;
            }

            string rhs = line.substr(arrowPos + 2);
            productions[lhs].push_back(splitProduction(rhs));
        }
        break;
        }
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
    for (map<string, vector<vector<string>>>::const_iterator it = productions.begin();
        it != productions.end(); ++it) {
        const string& currentNonTerminal = it->first;
        const vector<vector<string>>& productionList = it->second;

        for (vector<vector<string>>::const_iterator prodIt = productionList.begin();
            prodIt != productionList.end(); ++prodIt) {
            cout << currentNonTerminal << " -> ";
            for (vector<string>::const_iterator symbolIt = prodIt->begin();
                symbolIt != prodIt->end(); ++symbolIt) {
                cout << *symbolIt << " ";
            }
            cout << endl;
        }
    }
}

void Grammar::printProductionsFor(const string& nonTerminal) const {
    map<string, vector<vector<string>>>::const_iterator it = productions.find(nonTerminal);
    if (it == productions.end()) {
        cout << "No productions for " << nonTerminal << endl;
        return;
    }

    cout << "Productions for " << nonTerminal << ":" << endl;
    const vector<vector<string>>& productionList = it->second;
    for (vector<vector<string>>::const_iterator prodIt = productionList.begin();
        prodIt != productionList.end(); ++prodIt) {
        cout << nonTerminal << " -> ";
        for (vector<string>::const_iterator symbolIt = prodIt->begin();
            symbolIt != prodIt->end(); ++symbolIt) {
            cout << *symbolIt << " ";
        }
        cout << endl;
    }
}

bool Grammar::checkCFG() const {
    // Check 1: Start symbol must be a non-terminal
    if (nonTerminals.find(startSymbol) == nonTerminals.end()) {
        return false;
    }

    // Check 2: LHS of each production must be a single non-terminal
    for (map<string, vector<vector<string>>>::const_iterator it = productions.begin();
        it != productions.end(); ++it) {
        if (nonTerminals.find(it->first) == nonTerminals.end()) {
            return false;
        }
    }

    // Check 3: RHS symbols must be either terminals or non-terminals
    for (map<string, vector<vector<string>>>::const_iterator it = productions.begin();
        it != productions.end(); ++it) {
        const vector<vector<string>>& productionList = it->second;
        for (vector<vector<string>>::const_iterator prodIt = productionList.begin();
            prodIt != productionList.end(); ++prodIt) {
            for (vector<string>::const_iterator symbolIt = prodIt->begin();
                symbolIt != prodIt->end(); ++symbolIt) {
                if (!isTerminal(*symbolIt) && !isNonTerminal(*symbolIt)) {
                    return false;
                }
            }
        }
    }

    // Check 4: Every non-terminal must have at least one production
    for (set<string>::const_iterator it = nonTerminals.begin();
        it != nonTerminals.end(); ++it) {
        map<string, vector<vector<string>>>::const_iterator prodIt = productions.find(*it);
        if (prodIt == productions.end() || prodIt->second.empty()) {
            return false;
        }
    }

    return true;
}