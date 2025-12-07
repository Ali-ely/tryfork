//
//  DijkstraMinHeap.hpp
//  TestMap
//

#ifndef DijkstraMinHeap_hpp
#define DijkstraMinHeap_hpp

#include <limits>

struct HeapNode {
    int vertexIndex;
    double distance;
    HeapNode(int v = -1, double d = std::numeric_limits<double>::infinity()) : vertexIndex(v), distance(d) {}
};

class DijkstraMinHeap {
private:
    HeapNode *arr; // 1-based index array
    int *pos;      // maps vertexIndex -> position in heap, -1 if not present
    int capacity;  // max heap elements
    int size;
    int maxVertices;

    void swapNodes(int i, int j);
    void upheap(int i);
    void downheap(int i);

public:
    // heapCapacity: max elements, maxVertexIndex: largest vertex index used (pos sized to maxVertexIndex+1)
    explicit DijkstraMinHeap(int heapCapacity, int maxVertexIndex);
    ~DijkstraMinHeap();

    bool isEmpty() const;
    void insert(int vertexIndex, double distance);
    HeapNode extractMin();
    void decreaseKey(int vertexIndex, double newDist);
    bool isInHeap(int vertexIndex) const;
};

#endif /* DijkstraMinHeap_hpp */
