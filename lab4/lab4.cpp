#include "Scanner.h"
#include "FiniteAutomata.h"
#include "Grammar.h"
using namespace std;

int main() {

    Grammar grammar;


    grammar.readFromFile("g1.txt");
    grammar.printNonTerminals();
    cout << endl;
    grammar.printTerminals();
    cout << endl;
    grammar.printProductions();
    cout << endl;
    cout << grammar.checkCFG();
    grammar.computeFirst();
    grammar.printFirstSets();

    grammar.computeFollow();
    grammar.printFollowSets();
    return 0;
}