#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <cstddef>

class HashTable
{
private:
    struct Entry
    {
        char key[64];
        void *value;
        bool occupied;
        bool deleted;
        Entry() : value(nullptr), occupied(false), deleted(false)
        {
            key[0] = '\0';
        }
    };

    Entry *table;
    int capacity;
    int count;
    float maxLoadFactor;

    int hashKey(const char *key) const;
    int probeIndex(int hash, int attempt) const;
    int findSlot(const char *key) const;
    bool shouldResize() const;
    void rehashInternal(int newCapacity);

public:
    explicit HashTable(int initialCapacity = 211, float loadFactor = 0.7f);
    ~HashTable();

    bool insert(const char *key, void *value);
    void *find(const char *key) const;
    bool remove(const char *key);
    void clear();
    void rehash(int newCapacity);
    int size() const { return count; }
    int getCapacity() const { return capacity; }
};

#endif
