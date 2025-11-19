#include "HashTable.h"
#include <cstring>
#include <cstdio>

HashTable::HashTable(int initialCapacity, float loadFactor)
    : table(nullptr), capacity(initialCapacity), count(0), maxLoadFactor(loadFactor)
{
    if (capacity < 11) // minimum capacity
        capacity = 11;
    table = new Entry[capacity];
}

HashTable::~HashTable()
{
    delete[] table;
}

// Hash function using DJB2 algorithm
int HashTable::hashKey(const char *key) const
{
    unsigned long hash = 5381;
    for (const char *c = key; *c != '\0'; ++c)
    {
        hash = (33 * hash) + *c;
    }
    return static_cast<int>(hash % capacity);
}

// Linear probing
int HashTable::probeIndex(int hash, int attempt) const
{
    return (hash + attempt) % capacity;
}

bool HashTable::shouldResize() const
{
    return (float)count / capacity >= maxLoadFactor;
}

void HashTable::rehashInternal(int newCapacity)
{
    Entry *oldTable = table;
    int oldCapacity = capacity;

    capacity = newCapacity;
    table = new Entry[capacity];
    count = 0;

    for (int i = 0; i < oldCapacity; ++i)
    {
        if (oldTable[i].occupied && !oldTable[i].deleted)
        {
            insert(oldTable[i].key, oldTable[i].value);
        }
    }

    delete[] oldTable;
}

void HashTable::rehash(int newCapacity)
{
    if (newCapacity <= capacity)
        return;
    rehashInternal(newCapacity);
}

bool HashTable::insert(const char *key, void *value)
{
    if (shouldResize())
    {
        rehashInternal(capacity * 2 + 1);
    }

    int hash = hashKey(key);
    for (int attempt = 0; attempt < capacity; ++attempt)
    {
        int index = probeIndex(hash, attempt);

        if (!table[index].occupied || table[index].deleted)
        {
            snprintf(table[index].key, sizeof(table[index].key), "%s", key);
            table[index].value = value;
            table[index].occupied = true;
            table[index].deleted = false;
            count++;
            return true;
        }

        if (!table[index].deleted && std::strcmp(table[index].key, key) == 0)
        {
            table[index].value = value;
            return true;
        }
    }
    return false;
}

int HashTable::findSlot(const char *key) const
{
    int hash = hashKey(key);
    for (int attempt = 0; attempt < capacity; ++attempt)
    {
        int index = probeIndex(hash, attempt);

        if (!table[index].occupied && !table[index].deleted)
        {
            return -1;
        }

        if (table[index].occupied && !table[index].deleted && std::strcmp(table[index].key, key) == 0)
        {
            return index;
        }
    }
    return -1;
}

void *HashTable::find(const char *key) const
{
    int slot = findSlot(key);
    if (slot == -1)
        return nullptr;
    return table[slot].value;
}

bool HashTable::remove(const char *key)
{
    int slot = findSlot(key);
    if (slot == -1)
        return false;

    table[slot].deleted = true;
    table[slot].occupied = false;
    table[slot].value = nullptr;
    count--;
    return true;
}

void HashTable::clear()
{
    delete[] table;
    table = new Entry[capacity];
    count = 0;
}
