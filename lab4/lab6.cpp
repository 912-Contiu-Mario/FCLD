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
    cout << "Is context free: " << grammar.checkCFG()<<endl<<endl;
    grammar.computeFirst();
    grammar.printFirstSets();
    cout << endl;

    grammar.computeFollow();
    grammar.printFollowSets();
    return 0;
}