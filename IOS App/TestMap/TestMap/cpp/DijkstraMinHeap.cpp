//
//  DijkstraMinHeap.cpp
//  TestMap
//

#include "DijkstraMinHeap.hpp"
#include <limits>
#include <new>

DijkstraMinHeap::DijkstraMinHeap(int heapCapacity, int maxVertexIndex)
: arr(nullptr), pos(nullptr), capacity(heapCapacity), size(0), maxVertices(maxVertexIndex)
{
    if (capacity < 1) capacity = 1;
    // allocate 1-based array: capacity + 1
    arr = new HeapNode[capacity + 1];
    pos = new int[maxVertices + 1];
    for (int i = 0; i <= maxVertices; ++i) pos[i] = -1;
}

DijkstraMinHeap::~DijkstraMinHeap() {
    delete[] arr;
    delete[] pos;
}

bool DijkstraMinHeap::isEmpty() const {
    return size == 0;
}

bool DijkstraMinHeap::isInHeap(int vertexIndex) const {
    if (vertexIndex < 0 || vertexIndex > maxVertices) return false;
    int p = pos[vertexIndex];
    return p != -1 && p >= 1 && p <= size;
}

void DijkstraMinHeap::swapNodes(int i, int j) {
    HeapNode tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;

    pos[arr[i].vertexIndex] = i;
    pos[arr[j].vertexIndex] = j;
}

void DijkstraMinHeap::upheap(int i) {
    while (i > 1) {
        int parent = i / 2;
        if (arr[i].distance < arr[parent].distance) {
            swapNodes(i, parent);
            i = parent;
        } else break;
    }
}

void DijkstraMinHeap::downheap(int i) {
    while (true) {
        int smallest = i;
        int left = 2 * i;
        int right = 2 * i + 1;

        if (left <= size && arr[left].distance < arr[smallest].distance) smallest = left;
        if (right <= size && arr[right].distance < arr[smallest].distance) smallest = right;

        if (smallest != i) {
            swapNodes(i, smallest);
            i = smallest;
        } else break;
    }
}

void DijkstraMinHeap::insert(int vertexIndex, double distance) {
    if (size >= capacity) return;
    if (vertexIndex < 0 || vertexIndex > maxVertices) return;

    ++size;
    arr[size].vertexIndex = vertexIndex;
    arr[size].distance = distance;
    pos[vertexIndex] = size;
    upheap(size);
}

HeapNode DijkstraMinHeap::extractMin() {
    if (isEmpty()) return HeapNode(-1, -1.0);

    HeapNode root = arr[1];
    HeapNode last = arr[size];

    arr[1] = last;

    pos[root.vertexIndex] = -1;
    pos[last.vertexIndex] = 1;

    --size;
    if (size >= 1) downheap(1);

    return root;
}

void DijkstraMinHeap::decreaseKey(int vertexIndex, double newDist) {
    if (vertexIndex < 0 || vertexIndex > maxVertices) return;
    int i = pos[vertexIndex];
    if (i == -1 || i > size || i < 1) return;
    if (newDist > arr[i].distance) return;

    arr[i].distance = newDist;
    upheap(i);
}
