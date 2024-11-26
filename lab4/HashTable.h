#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <sstream> 


class Node {
public:
    std::string val;
    Node* next;
    Node* prev;
    Node(const std::string& key);
};



class HashTable {
private:
    std::vector<Node*> table;
    int capacity;

    int hashFunction(const std::string& key);

public:
    HashTable(int size);

    void insert(const std::string& key);
    void insert(int key);
    void remove(const std::string& key);
    std::pair<int, int> searchElem(const std::string& key);
    std::string display();
    bool exists(const std::string& key);

};

