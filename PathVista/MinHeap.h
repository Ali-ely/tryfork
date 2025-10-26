//
// Created by Adham Abotarboush on 26/10/2025.
//

#ifndef PATHVISTA_MINHEAP_H
#define PATHVISTA_MINHEAP_H

#include <iostream>
using namespace std;

template <typename T>
class MinHeap {
private:
    T* arr;
    int capacity;
    int size;

    static int parent(int i);

    static int left(int i);

    static int right(int i);
    int last() const;

    void heapifyUp(int i);
    void heapifyDown(int i);

public:
    explicit MinHeap(int cap);
    ~MinHeap();

    bool isEmpty() const;
    bool isFull() const;

    void insert(const T& key);
    T getMin();
    T extractMin();
    void printHeap();
};

#endif //PATHVISTA_MINHEAP_H