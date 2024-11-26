#include "Tokenize.h"

using namespace std;


vector<string> tokenize(string toTokenize, char del) {
	stringstream stream(toTokenize);

	string token;
	vector<string> tokens;
	while (getline(stream, token, del)) {
		tokens.push_back(token);
	}

	return tokens;
}

string removeFromString(const std::string str, char toRem) {
	std::string result = str;  // Create a copy of the input string
	result.erase(std::remove(result.begin(), result.end(), toRem), result.end());
	return result;
}
