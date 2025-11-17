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

    T temp = std::move(arr[i]);

    while (i > 0 && arr[parent(i)] > temp)
    {
        arr[i] = std::move(arr[parent(i)]);
        i = parent(i);
    }

    arr[i] = std::move(temp);
}

template <typename T>
void MinHeap<T>::downheap(int i)
{
    T temp = std::move(arr[i]);
    int j = left(i);

    while (j <= N)
    {
        int r = j+1;

        if (r <= N && arr[j] > arr[r])   j = r;

        if (temp <= arr[j]) break;

        arr[i] = std::move(arr[j]);
        i = j;
        j = left(i);
    }
    arr[i] = std::move(temp);
}

template <typename T>
void MinHeap<T>::resize()
{
    int newCapacity = capacity * 2;
    T* newArr = new T[newCapacity + 1];
    for (int i = 0; i <= N; i++)
        newArr[i] = std::move(arr[i]);
    delete[] arr;
    arr = newArr;
    capacity = newCapacity;
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
    if (isFull()) resize();
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
    for (int i = 1; i <= N; i++)
        cout << arr[i] << " ";
    cout << endl;
}