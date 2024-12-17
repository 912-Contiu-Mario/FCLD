#include <string>
#include <vector>
#include <set>
#include <map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

class Grammar {
private:
    std::set<std::string> nonTerminals;
    std::set<std::string> terminals;
    std::string startSymbol;
    std::map<std::string, std::vector<std::vector<std::string>>> productions;
    bool isTerminal(const std::string& symbol) const;
    bool isNonTerminal(const std::string& symbol) const;
    std::vector<std::string> splitProduction(const std::string& production) const;
    bool isCFG = true;

    std::map<std::string, std::set<std::string>> firstSets;  // To store First sets
    std::map<std::string, std::set<std::string>> followSets; // To store Follow sets

    // Helper function to add elements to a set and check if it was modified
    bool addToSet(std::set<std::string>& targetSet, const std::set<std::string>& sourceSet);
public:
    Grammar();
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


    void computeFirst();
    void computeFollow();

    void printFirstSets() const;
    void printFollowSets() const;
};