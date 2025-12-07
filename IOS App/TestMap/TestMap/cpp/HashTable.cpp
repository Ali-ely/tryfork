//
//  HashTable.cpp
//  TestMap
//

#include "HashTable.hpp"
#include <cstring>
#include <new>

HashTable::HashTable(int initialCapacity, float loadFactor)
: buckets(nullptr), capacity(initialCapacity), count(0), maxLoadFactor(loadFactor)
{
    if (capacity < 11) capacity = 11;
    buckets = new EntryNode*[capacity];
    for (int i = 0; i < capacity; ++i) buckets[i] = nullptr;
}

HashTable::~HashTable() {
    clear();
    delete[] buckets;
    buckets = nullptr;
    capacity = 0;
    count = 0;
}

unsigned long HashTable::hashKey(const std::string &key) const {
    unsigned long hash = 5381;
    for (size_t i = 0; i < key.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(key[i]);
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash % static_cast<unsigned long>(capacity);
}

void HashTable::rehashInternal(int newCapacity) {
    if (newCapacity < 11) newCapacity = 11;
    EntryNode **oldBuckets = buckets;
    int oldCapacity = capacity;

    buckets = new EntryNode*[newCapacity];
    for (int i = 0; i < newCapacity; ++i) buckets[i] = nullptr;

    capacity = newCapacity;
    count = 0;

    for (int i = 0; i < oldCapacity; ++i) {
        EntryNode *node = oldBuckets[i];
        while (node) {
            insert(node->key.c_str(), node->value);
            EntryNode *tmp = node;
            node = node->next;
            delete tmp;
        }
    }
    delete[] oldBuckets;
}

bool HashTable::insert(const char *keyC, void *value) {
    if (!keyC) return false;
    std::string key(keyC);

    // rehash if load factor exceeded
    float lf = (float)count / (float)capacity;
    if (lf >= maxLoadFactor) {
        rehashInternal(capacity * 2 + 1);
    }

    unsigned long hash = hashKey(key);
    int index = static_cast<int>(hash);

    EntryNode *node = buckets[index];
    while (node) {
        if (node->key == key) {
            node->value = value;
            return true;
        }
        node = node->next;
    }

    // insert at head
    EntryNode *n = new EntryNode(key, value, buckets[index]);
    buckets[index] = n;
    ++count;
    return true;
}

void *HashTable::find(const char *keyC) const {
    if (!keyC) return nullptr;
    std::string key(keyC);
    unsigned long hash = hashKey(key);
    int index = static_cast<int>(hash);

    EntryNode *node = buckets[index];
    while (node) {
        if (node->key == key) return node->value;
        node = node->next;
    }
    return nullptr;
}

void HashTable::clear() {
    if (!buckets) return;
    for (int i = 0; i < capacity; ++i) {
        EntryNode *node = buckets[i];
        while (node) {
            EntryNode *tmp = node;
            node = node->next;
            delete tmp;
        }
        buckets[i] = nullptr;
    }
    count = 0;
}
