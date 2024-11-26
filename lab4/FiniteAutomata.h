// FiniteAutomata.h
#pragma once
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <iostream>
#include "Tokenize.h"

class FA {
private:
   void init(std::string filepath);
    std::unordered_set<std::string> states;
    std::unordered_map<std::string, std::string> alphabetMap;
    std::string initialState;
    std::unordered_set<std::string> finalStates;
    std::map<std::string, std::map<std::string, std::string>> transitions;
    std::string currentState;
    bool isInside(char character, std::string toSearchIn);
    std::string expand(std::string expand);


public:
    FA(std::string filepath);
    FA();
    bool checkIfConsistent(std::string toCheck);
    void displayStates() const;
    void displayAlphabet() const;
    void displayTransitions() const;
    void displayInitialState() const;
    void displayFinalStates() const;
};