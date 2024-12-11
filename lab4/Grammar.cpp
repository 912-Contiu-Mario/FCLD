#include "Grammar.h"
#include <iostream>
#include <sstream>
#include <algorithm>

Grammar::Grammar() : startSymbol("") {}

bool Grammar::isTerminal(const std::string& symbol) const {
    return terminals.find(symbol) != terminals.end();
}

bool Grammar::isNonTerminal(const std::string& symbol) const {
    return nonTerminals.find(symbol) != nonTerminals.end();
}

std::vector<std::string> Grammar::splitProduction(const std::string& production) const {
    std::vector<std::string> symbols;
    std::istringstream iss(production);
    std::string symbol;

    while (iss >> symbol) {
        symbols.push_back(symbol);
    }

    return symbols;
}

bool Grammar::readFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return false;
    }

    // Clear existing data
    nonTerminals.clear();
    terminals.clear();
    productions.clear();

    std::string line;
    int section = 0;  // 0: non-terminals, 1: terminals, 2: start symbol, 3: productions

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;  // Skip empty lines and comments

        if (line == "$") {
            section++;
            continue;
        }

        switch (section) {
        case 0:  // Non-terminals
            nonTerminals.insert(line);
            break;

        case 1:  // Terminals
            terminals.insert(line);
            break;

        case 2:  // Start symbol
            if (nonTerminals.find(line) != nonTerminals.end()) {
                startSymbol = line;
            }
            else {
                std::cerr << "Error: Start symbol must be a non-terminal" << std::endl;
                return false;
            }
            break;

        case 3:  // Productions
        {
            size_t arrowPos = line.find("->");
            if (arrowPos == std::string::npos) {
                std::cerr << "Error: Invalid production format" << std::endl;
                return false;
            }

            std::string lhs = line.substr(0, arrowPos);
            lhs.erase(std::remove_if(lhs.begin(), lhs.end(), ::isspace), lhs.end());

            if (nonTerminals.find(lhs) == nonTerminals.end()) {
                std::cerr << "Error: LHS of production must be a non-terminal" << std::endl;
                return false;
            }

            std::string rhs = line.substr(arrowPos + 2);
            productions[lhs].push_back(splitProduction(rhs));
        }
        break;
        }
    }

    file.close();
    return true;
}

void Grammar::printNonTerminals() const {
    std::cout << "Non-terminals: ";
    for (const auto& nt : nonTerminals) {
        std::cout << nt << " ";
    }
    std::cout << std::endl;
}

void Grammar::printTerminals() const {
    std::cout << "Terminals: ";
    for (const auto& t : terminals) {
        std::cout << t << " ";
    }
    std::cout << std::endl;
}

void Grammar::printProductions() const {
    std::cout << "Productions:" << std::endl;
    for (std::map<std::string, std::vector<std::vector<std::string>>>::const_iterator it = productions.begin();
        it != productions.end(); ++it) {
        const std::string& currentNonTerminal = it->first;
        const std::vector<std::vector<std::string>>& productionList = it->second;

        for (std::vector<std::vector<std::string>>::const_iterator prodIt = productionList.begin();
            prodIt != productionList.end(); ++prodIt) {
            std::cout << currentNonTerminal << " -> ";
            for (std::vector<std::string>::const_iterator symbolIt = prodIt->begin();
                symbolIt != prodIt->end(); ++symbolIt) {
                std::cout << *symbolIt << " ";
            }
            std::cout << std::endl;
        }
    }
}

void Grammar::printProductionsFor(const std::string& nonTerminal) const {
    std::map<std::string, std::vector<std::vector<std::string>>>::const_iterator it = productions.find(nonTerminal);
    if (it == productions.end()) {
        std::cout << "No productions for " << nonTerminal << std::endl;
        return;
    }

    std::cout << "Productions for " << nonTerminal << ":" << std::endl;
    const std::vector<std::vector<std::string>>& productionList = it->second;
    for (std::vector<std::vector<std::string>>::const_iterator prodIt = productionList.begin();
        prodIt != productionList.end(); ++prodIt) {
        std::cout << nonTerminal << " -> ";
        for (std::vector<std::string>::const_iterator symbolIt = prodIt->begin();
            symbolIt != prodIt->end(); ++symbolIt) {
            std::cout << *symbolIt << " ";
        }
        std::cout << std::endl;
    }
}

bool Grammar::checkCFG() const {
    // Check 1: Start symbol must be a non-terminal
    if (nonTerminals.find(startSymbol) == nonTerminals.end()) {
        return false;
    }

    // Check 2: LHS of each production must be a single non-terminal
    for (std::map<std::string, std::vector<std::vector<std::string>>>::const_iterator it = productions.begin();
        it != productions.end(); ++it) {
        if (nonTerminals.find(it->first) == nonTerminals.end()) {
            return false;
        }
    }

    // Check 3: RHS symbols must be either terminals or non-terminals
    for (std::map<std::string, std::vector<std::vector<std::string>>>::const_iterator it = productions.begin();
        it != productions.end(); ++it) {
        const std::vector<std::vector<std::string>>& productionList = it->second;
        for (std::vector<std::vector<std::string>>::const_iterator prodIt = productionList.begin();
            prodIt != productionList.end(); ++prodIt) {
            for (std::vector<std::string>::const_iterator symbolIt = prodIt->begin();
                symbolIt != prodIt->end(); ++symbolIt) {
                if (!isTerminal(*symbolIt) && !isNonTerminal(*symbolIt)) {
                    return false;
                }
            }
        }
    }

    // Check 4: Every non-terminal must have at least one production
    for (std::set<std::string>::const_iterator it = nonTerminals.begin();
        it != nonTerminals.end(); ++it) {
        std::map<std::string, std::vector<std::vector<std::string>>>::const_iterator prodIt = productions.find(*it);
        if (prodIt == productions.end() || prodIt->second.empty()) {
            return false;
        }
    }

    return true;
}