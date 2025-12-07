//
//  HashTable.hpp
//  TestMap
//

#ifndef HashTable_hpp
#define HashTable_hpp

#include <string>

class HashTable {
private:
    // separate chaining entry node
    struct EntryNode {
        std::string key;
        void *value;
        EntryNode *next;
        EntryNode(const std::string &k = std::string(), void *v = nullptr, EntryNode *n = nullptr) : key(k), value(v), next(n) {}
    };

    EntryNode **buckets; // array of bucket heads (linked lists)
    int capacity;
    int count;
    float maxLoadFactor;

    unsigned long hashKey(const std::string &key) const;
    void rehashInternal(int newCapacity);

public:
    explicit HashTable(int initialCapacity = 211, float loadFactor = 0.7f);
    ~HashTable();

    bool insert(const char *key, void *value);
    void *find(const char *key) const;
    void clear();
};

#endif /* HashTable_hpp */
