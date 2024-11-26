#include "Scanner.h"
using namespace std;


int main() {
    try {
        Scanner scanner("p1err.txt");
        scanner.scan();
        cout << "Lexically correct" << endl;
        scanner.generatePIFFile();
        scanner.generateSTFile();
    }
    catch (const LexicalException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
   
}

