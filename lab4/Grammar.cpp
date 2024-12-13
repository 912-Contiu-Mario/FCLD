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


    nonTerminals.clear();
    terminals.clear();
    productions.clear();

    string line;
    int section = 0;  // 0: non-terminals, 1: terminals, 2: start symbol, 3: productions

    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;  // comment

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
            lhs.erase(remove_if(lhs.begin(), lhs.end(), ::isspace), lhs.end());//scoate spatiile

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
    // Ensure start symbol is a valid non-terminal
    if (startSymbol.empty() || nonTerminals.find(startSymbol) == nonTerminals.end()) {
        cerr << "Invalid grammar: Start symbol is missing or not a non-terminal." << endl;
        return false;
    }

    // Validate all productions
    for (const auto& entry : productions) {
        const string& lhs = entry.first; // LHS of the production
        const vector<vector<string>>& productionList = entry.second; // RHS productions

        // Ensure LHS is a valid single non-terminal
        if (nonTerminals.find(lhs) == nonTerminals.end()) {
            cerr << "Invalid grammar: LHS of production (" << lhs << ") is not a non-terminal." << endl;
            return false;
        }

        // Ensure LHS contains exactly one symbol (already implied by the map structure in your code)
        if (lhs.find(' ') != string::npos) {
            cerr << "Invalid grammar: LHS of production (" << lhs
                << ") contains more than one symbol." << endl;
            return false;
        }

        // Ensure LHS is not a terminal
        if (terminals.find(lhs) != terminals.end()) {
            cerr << "Invalid grammar: LHS of production (" << lhs << ") cannot be a terminal." << endl;
            return false;
        }

        // Check each RHS in the production list
        for (const auto& rhs : productionList) {
            for (const auto& symbol : rhs) {
                // Ensure each symbol in RHS is either a terminal or a non-terminal
                if (!isTerminal(symbol) && !isNonTerminal(symbol)) {
                    cerr << "Invalid grammar: Symbol '" << symbol
                        << "' in production is neither a terminal nor a non-terminal." << endl;
                    return false;
                }
            }
        }
    }

    // Ensure every non-terminal has at least one production
    for (const auto& nonTerminal : nonTerminals) {
        if (productions.find(nonTerminal) == productions.end()) {
            cerr << "Invalid grammar: Non-terminal '" << nonTerminal
                << "' has no productions." << endl;
            return false;
        }
    }

    // Grammar is valid
    return true;
}


