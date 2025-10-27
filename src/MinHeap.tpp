#include "MinHeap.h"

template <typename T>
int MinHeap<T>::parent(int i) { return (i - 1) / 2; }

template <typename T>
int MinHeap<T>::left(int i) { return 2 * i + 1; }

template <typename T>
int MinHeap<T>::right(int i) { return 2 * i + 2; }

template <typename T>
int MinHeap<T>::last() const { return size - 1; }

template <typename T>
void MinHeap<T>::heapifyUp(int i)
{
    while (i != 0 && arr[parent(i)] > arr[i])
    {
        swap(arr[i], arr[parent(i)]);
        i = parent(i);
    }
}

template <typename T>
void MinHeap<T>::heapifyDown(int i)
{
    int smallest = i;
    int l = left(i);
    int r = right(i);

    if (l < size && arr[l] < arr[smallest])
        smallest = l;
    if (r < size && arr[r] < arr[smallest])
        smallest = r;

    if (smallest != i)
    {
        swap(arr[i], arr[smallest]);
        heapifyDown(smallest);
    }
}

template <typename T>
MinHeap<T>::MinHeap(int cap)
{
    capacity = cap;
    size = 0;
    arr = new T[cap];
}

template <typename T>
MinHeap<T>::~MinHeap()
{
    delete[] arr;
}

template <typename T>
bool MinHeap<T>::isEmpty() const { return size == 0; }

template <typename T>
bool MinHeap<T>::isFull() const { return size == capacity; }

template <typename T>
void MinHeap<T>::insert(const T &key)
{
    if (isFull())
        return;
    arr[size] = key;
    size++;
    heapifyUp(last());
}

template <typename T>
T MinHeap<T>::getMin()
{
    if (isEmpty())
        return T();
    return arr[0];
}

template <typename T>
T MinHeap<T>::extractMin()
{
    if (isEmpty())
        return T();
    if (size == 1)
    {
        size--;
        return arr[0];
    }
    T root = arr[0];
    arr[0] = arr[last()];
    size--;
    heapifyDown(0);
    return root;
}

template <typename T>
void MinHeap<T>::printHeap()
{
    for (int i = 0; i < size; i++)
        cout << arr[i] << " ";
    cout << endl;
}