#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>
#include <fstream>

class Grammar {
private:
    std::set<std::string> nonTerminals;  // Set of non-terminal symbols
    std::set<std::string> terminals;     // Set of terminal symbols
    std::string startSymbol;             // Starting symbol of the grammar
    std::map<std::string, std::vector<std::vector<std::string>>> productions;  // Productions map

    // Helper functions
    bool isTerminal(const std::string& symbol) const;
    bool isNonTerminal(const std::string& symbol) const;
    std::vector<std::string> splitProduction(const std::string& production) const;

public:
    Grammar();

    // Required operations
    bool readFromFile(const std::string& filename);
    void printNonTerminals() const;
    void printTerminals() const;
    void printProductions() const;
    void printProductionsFor(const std::string& nonTerminal) const;
    bool checkCFG() const;

    // Getters
    const std::set<std::string>& getNonTerminals() const { return nonTerminals; }
    const std::set<std::string>& getTerminals() const { return terminals; }
    const std::string& getStartSymbol() const { return startSymbol; }
    const std::map<std::string, std::vector<std::vector<std::string>>>& getProductions() const {
        return productions;
    }
};