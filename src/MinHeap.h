#ifndef PATHVISTA_MINHEAP_H
#define PATHVISTA_MINHEAP_H

#include <iostream>
using namespace std;

template <typename T>
class MinHeap
{
private:
    T *arr;
    T minimum;
    int capacity;
    int N;

    static int parent(int i);

    static int left(int i);

    static int right(int i);
    int last() const;

    void upheap(int i);
    void downheap(int i);

    void resize();

public:
    explicit MinHeap(int cap, T min);
    ~MinHeap();

    bool isEmpty() const;
    bool isFull() const;

    void insert(const T &key);
    T getMin();
    T extractMin();
    void decreaseKey(int, const T &);
    int findIndex(const T &key) const;
    void printHeap();
};

#endif // PATHVISTA_MINHEAP_H