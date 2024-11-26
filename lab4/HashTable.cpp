#include "HashTable.h"

Node::Node(const std::string& key) : val(key), next(nullptr), prev(nullptr) {}

HashTable::HashTable(int size) {
    capacity = size;
    table.resize(size, nullptr);
}

int HashTable::hashFunction(const std::string& key) {
    int hashValue = 0;
    for (char ch : key) {
        hashValue += ch;
    }
    return hashValue % capacity;
}

void HashTable::insert(const std::string& key) {
    int index = hashFunction(key);
    Node* newNode = new Node(key);

    if (table[index] == nullptr) {
        table[index] = newNode;
    }
    else {
        Node* curr = table[index];
        while (curr->next != NULL) {
            curr = curr->next;
        }
        newNode->prev = curr;
        newNode->next = NULL;
        curr->next = newNode;
    }
}

void HashTable::insert(int key) {
    std::string stringKey = std::to_string(key);
    insert(stringKey);
}

void HashTable::remove(const std::string& key) {
    int index = hashFunction(key);
    Node* curr = table[index];

    while (curr != nullptr) {
        if (curr->val == key) {
            if (curr->prev != nullptr) {
                curr->prev->next = curr->next;
            }
            else {
                table[index] = curr->next;
            }

            if (curr->next != nullptr) {
                curr->next->prev = curr->prev;
            }

            delete curr;
            return;
        }
        curr = curr->next;
    }

    std::cout << "Key '" << key << "' not found in the hash table." << std::endl;
}

std::pair<int, int> HashTable::searchElem(const std::string& key) {
    int index = hashFunction(key);
    Node* curr = table[index];
    int position = 0;

    while (curr != nullptr) {
        if (curr->val == key) {
            return { index, position };
        }
        curr = curr->next;
        position++;
    }

    return { -1, -1 };
}


bool HashTable::exists(const std::string& key) {
    int index = hashFunction(key);
    Node* curr = table[index];
    int position = 0;

    while (curr != nullptr) {
        if (curr->val == key) {
            return true;
        }
        curr = curr->next;
        position++;
    }

    return false;
}

std::string HashTable::display() {
    std::ostringstream oss; 

    for (int i = 0; i < capacity; i++) {
        oss << "Bucket " << i << ": "; 
        Node* curr = table[i];

        while (curr != nullptr) {
            oss << curr->val << " <-> "; 
            curr = curr->next; 
        }
        oss << "NULL" << std::endl; 
    }

    return oss.str(); 
}
