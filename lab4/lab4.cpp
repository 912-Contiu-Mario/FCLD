#include "Scanner.h"
#include "FiniteAutomata.h"
#include "Grammar.h"
using namespace std;



void runMenu(FA& fa) {
    int choice;
    std::string sequence;

    do {
        cout << "\nFinite Automata Menu:\n";
        cout << "1. Display States\n";
        cout << "2. Display Alphabet\n";
        cout << "3. Display Transitions\n";
        cout << "4. Display Initial State\n";
        cout << "5. Display Final States\n";
        cout << "6. Verify Sequence\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            fa.displayStates();
            break;
        case 2:
            fa.displayAlphabet();
            break;
        case 3:
            fa.displayTransitions();
            break;
        case 4:
            fa.displayInitialState();
            break;
        case 5:
            fa.displayFinalStates();
            break;
        case 6:
            std::cout << "Enter a sequence to verify: ";
            std::cin >> sequence;
            if (fa.checkIfConsistent(sequence)) {
                std::cout << "The sequence is accepted by the FA.\n";
            }
            else {
                std::cout << "The sequence is not accepted by the FA.\n";
            }
            break;
        case 0:
            std::cout << "Exiting program.\n";
            break;
        default:
            std::cout << "Invalid choice. Please try again.\n";
            break;
        }
    } while (choice != 0);
}

int main() {
   //try {
   //     Scanner scanner("p1err.txt");
   //     scanner.scan();
   //     cout << "Lexically correct" << endl;
   //     scanner.generatePIFFile();
   //     scanner.generateSTFile();
   // }
   // catch (const LexicalException& e) {
   //     std::cerr << "Error: " << e.what() << std::endl;
   // }


  /*  FA finiteAutomata("FA-identifier.in");
    runMenu(finiteAutomata);*/

    Grammar grammar;


    grammar.readFromFile("g1.txt");
    grammar.printNonTerminals();
    cout << endl;
    grammar.printTerminals();
    cout << endl;
    grammar.printProductions();
    cout << endl;
    return 0;
}