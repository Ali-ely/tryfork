#include "MinHeap.h"

template <typename T>
int MinHeap<T>::parent(int i) { return (i) / 2; }

template <typename T>
int MinHeap<T>::left(int i) { return 2 * i; }

template <typename T>
int MinHeap<T>::right(int i) { return 2 * i + 1; }

template <typename T>
int MinHeap<T>::last() const { return N; }

template <typename T>
void MinHeap<T>::upheap(int i)
{
    while (i != 0 && arr[parent(i)] > arr[i])
    {
        swap(arr[i], arr[parent(i)]);
        i = parent(i);
    }
}

template <typename T>
void MinHeap<T>::downheap(int i)
{
    int smallest = i;
    int l = left(i);
    int r = right(i);

    if (l < N && arr[l] < arr[smallest])
        smallest = l;
    if (r < N && arr[r] < arr[smallest])
        smallest = r;

    if (smallest != i)
    {
        swap(arr[i], arr[smallest]);
        downheap(smallest);
    }
}

template <typename T>
MinHeap<T>::MinHeap(int cap, T min)
{
    capacity = cap;
    N = 0;
    minimum = min;
    arr = new T[capacity+1];
    arr[0] = minimum;
}

template <typename T>
MinHeap<T>::~MinHeap()
{
    delete[] arr;
}

template <typename T>
bool MinHeap<T>::isEmpty() const { return N == 0; }

template <typename T>
bool MinHeap<T>::isFull() const { return N == capacity; }

template <typename T>
void MinHeap<T>::insert(const T &key)
{
    if (isFull())
        return;
    arr[++N] = key;
    upheap(last());
}

template <typename T>
T MinHeap<T>::getMin()
{
    if (isEmpty())
        return T();
    return arr[1];
}

template <typename T>
T MinHeap<T>::extractMin()
{
    if (isEmpty())
        return T();
    if (N == 1)
    {
        N--;
        return arr[1];
    }
    T root = move(arr[1]);
    arr[1] = move(arr[last()]);
    N--;
    downheap(1);
    return root;
}

template <typename T>
void MinHeap<T>::printHeap()
{
    for (int i = 0; i < N; i++)
        cout << arr[i] << " ";
    cout << endl;
}