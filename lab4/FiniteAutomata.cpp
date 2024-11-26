#include "FiniteAutomata.h"
#include <fstream>
using namespace std;

void FA::init(string filepath) {
	ifstream faInput(filepath);
	string line;


	while (getline(faInput, line)) {
		vector<string> tokens = tokenize(line, ':');

		// Ensure `tokens` has two elements (label and value)
		if (tokens.size() < 2) continue;

		string label = tokens[0];
		string value = tokens[1];

		if (label == "states") {
			vector<string> stateTokens = tokenize(value, ',');
			for (string state : stateTokens) {
				this->states.insert(state);
			}
		}
		else if (label == "alphabet") {
			vector<string> entries = tokenize(value, ',');
			for (string entry : entries) {
				vector<string> aux = tokenize(entry, '=');

				//ensure that characters are given an alias
				if (aux.size() < 2) continue;

				string val = aux[0];
				string alias = aux[1];

				val = removeFromString(val, '[');
				val = removeFromString(val, ']');
				aux = tokenize(val, '|');
				for (string a : aux) {
					this->alphabetMap.insert({ this->expand(a), alias });
				}
			}
		}
		else if (label == "initial") {
			this->initialState = value;
		}
		else if (label == "final") {
			vector<string> finalStates = tokenize(value, ',');
			for (string state : finalStates) {
				this->finalStates.insert(state);
			}
		}
		else if (label == "transitions") {
			vector<string> transitionTokens = tokenize(value, ',');
			for (const string& transition : transitionTokens) {
				vector<string> parts = tokenize(transition, '=');

				
				if (parts.size() < 2) continue;
				string states = parts[0];
				string inputs = parts[1];
				vector<string> statesVec = tokenize(states, '|');

				//ensure that from and to state are gien
				if (statesVec.size() < 2) continue;

				string fromState = statesVec[0];
				string toState = statesVec[1];

				vector<string> symbols = tokenize(inputs, '.');
				symbols[0] = removeFromString(symbols[0], '[');
				symbols[symbols.size() - 1] = removeFromString(symbols[symbols.size() - 1], ']');

				for (const string& symbol : symbols) {
					this->transitions[fromState][symbol] = toState;
				}
			}
		}
	}
}

string FA::expand(std::string expand)
{
	vector<std::string> parts = tokenize(expand, '.');
	string expanded = "";

	//if it's a range, we expand it
	if (parts.size() == 2) {

		//make sure that the range only made of chars
		if (parts[0].size() > 1 || parts[1].size() > 1)
			throw exception("incorrent FA.in format");
		char curr = parts[0][0];
		//construct range
		while (curr != parts[1][0]) {
			expanded = expanded + curr;
			curr++;
		}
		expanded = expanded + parts[1][0];
		return expanded;
	}
	else if (parts.size() == 1) {
		//there's no expanding needed
		return expand;
	}
	
	throw exception("incorrect FA.in format");
}

FA::FA(string filepath) {
	this->init(filepath);
}

FA::FA()
{
}

bool FA::checkIfConsistent(std::string toCheck)
{
	currentState = initialState;
	for (size_t i = 0; i < toCheck.size(); ++i) {
		char ch = toCheck[i];
		bool foundTransition = false;

		for (const auto& entry : alphabetMap) {
			//find what variable character is part of
			if (entry.first.find(ch) != std::string::npos) {
				
				//find to which state current character leads
				if (this->transitions[currentState][entry.second] != "") {
					currentState = this->transitions[currentState][entry.second];
					foundTransition = true;
				}
				
				//we found type of char we can break
				break;
			}
			

		}
		if (!foundTransition)
			return false;
	}

	//we find if the final state is a final state, if it is, it means the given string is consistent
	if (std::find(finalStates.begin(), finalStates.end(), currentState) != finalStates.end()) {
		return true;
	}
	return false;
}


void FA::displayStates() const {
	std::cout << "States: ";
	for (const auto& state : states) {
		std::cout << state << " ";
	}
	std::cout << std::endl;
}

void FA::displayAlphabet() const {
	std::cout << "Alphabet: ";
	for (const auto& entry : alphabetMap) {
		std::cout << entry.first << " (alias: " << entry.second << ") ";
	}
	std::cout << std::endl;
}

void FA::displayTransitions() const {
	std::cout << "Transitions:\n";
	for (const auto& fromState : transitions) {
		for (const auto& symbol : fromState.second) {
			std::cout << fromState.first << " --" << symbol.first << "--> " << symbol.second << "\n";
		}
	}
}

void FA::displayInitialState() const {
	std::cout << "Initial State: " << initialState << std::endl;
}

void FA::displayFinalStates() const {
	std::cout << "Final States: ";
	for (const auto& state : finalStates) {
		std::cout << state << " ";
	}
	std::cout << std::endl;
}