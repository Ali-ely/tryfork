#include "ShortestPath.h"
#include "HashTable.h"
#include "MinHeap.h"
#include "MinHeap.tpp"
#include <cstdio>
#include <cstring>
#include <cfloat>

namespace
{
    struct HeapNode
    {
        int vertexIndex;
        float distance;

        HeapNode() : vertexIndex(-1), distance(0.0f) {}
        HeapNode(int idx, float dist) : vertexIndex(idx), distance(dist) {}

        bool operator<(const HeapNode &other) const { return distance < other.distance; }
        bool operator>(const HeapNode &other) const { return distance > other.distance; }
        // bool operator<=(const HeapNode &other) const { return distance <= other.distance; }
        bool operator==(const HeapNode &other) const { return vertexIndex == other.vertexIndex; }
    };

    int getIndex(HashTable &map, const char *name)
    {
        void *value = map.find(name);
        if (!value)
            return -1;
        return *static_cast<int *>(value);
    }
}

void ShortestPath::resetResult(PathResult &result)
{
    release(result);
}

void ShortestPath::release(PathResult &result)
{
    if (result.nodes)
    {
        for (int i = 0; i < result.nodeCount; ++i)
        {
            delete[] result.nodes[i];
        }
        delete[] result.nodes;
    }
    result.nodes = nullptr;
    result.nodeCount = 0;
    result.totalDistance = 0.0f;
    result.reachable = false;
}

bool ShortestPath::compute(Graph &graph, const char *start, const char *destination, PathResult &result)
{
    // Diskstra Algorithm
    resetResult(result);

    int numNodes = graph.getNumOfNodes();
    if (numNodes == 0)
        return false;

    VertexNode **vertices = new VertexNode *[numNodes];
    // int *indexStorage = new int[numNodes];
    HashTable indexMap(numNodes * 2 + 1);

    VertexNode *curr = graph.getHead();
    int idx = 0;
    while (curr && idx < numNodes)
    {
        vertices[idx] = curr;
        // indexStorage[idx] = idx;
        // indexMap.insert(curr->name, &indexStorage[idx]);
        int index = idx;
        indexMap.insert(curr->name, &index);
        curr = curr->next;
        idx++;
    }

    int startIndex = getIndex(indexMap, start);
    int destIndex = getIndex(indexMap, destination);
    if (startIndex == -1 || destIndex == -1)
    {
        delete[] vertices;
        // delete[] indexStorage;
        return false;
    }

    float *distances = new float[numNodes];
    bool *visited = new bool[numNodes];
    bool *inHeap = new bool[numNodes];
    int *previous = new int[numNodes];

    const float INF = FLT_MAX;
    for (int i = 0; i < numNodes; ++i)
    {
        distances[i] = INF;
        visited[i] = false;
        inHeap[i] = false;
        previous[i] = -1;
    }

    MinHeap<HeapNode> heap(numNodes, HeapNode(-1, -1.0f));
    distances[startIndex] = 0.0f;
    heap.insert(HeapNode(startIndex, 0.0f));
    inHeap[startIndex] = true;

    while (!heap.isEmpty())
    {
        HeapNode node = heap.extractMin();
        int u = node.vertexIndex;

        if (u < 0 || visited[u])
            continue;

        visited[u] = true;
        inHeap[u] = false;
        if (u == destIndex)
            break;

        EdgeNode *edge = vertices[u]->adjHead;
        while (edge)
        {
            int v = getIndex(indexMap, edge->to);
            if (v != -1 && !visited[v])
            {
                float newDist = distances[u] + edge->weight;
                if (newDist < distances[v])
                {
                    distances[v] = newDist;
                    previous[v] = u;
                    if (inHeap[v])
                    {
                        int position = heap.findIndex(HeapNode(v, newDist));
                        // if (position != -1)
                        // {
                        heap.decreaseKey(position, HeapNode(v, newDist));
                        // }
                        // else
                        // {
                        // heap.insert(HeapNode(v, newDist));
                        // inHeap[v] = true;
                        // }
                    }
                    else
                    {
                        heap.insert(HeapNode(v, newDist));
                        inHeap[v] = true;
                    }
                }
            }
            edge = edge->next;
        }
    }

    bool reachable = visited[destIndex] && distances[destIndex] < INF;
    if (reachable)
    {
        int pathLength = 0; // Number of nodes between start and destination
        for (int v = destIndex; v != -1; v = previous[v])
            pathLength++;

        result.nodes = new char *[pathLength];
        result.nodeCount = pathLength;
        int pos = pathLength - 1;
        for (int v = destIndex; v != -1; v = previous[v])
        {
            result.nodes[pos] = new char[64];
            std::snprintf(result.nodes[pos], 64, "%s", vertices[v]->name);
            pos--;
        }
        result.totalDistance = distances[destIndex];
        result.reachable = true;
    }

    delete[] vertices;
    // delete[] indexStorage;
    delete[] distances;
    delete[] visited;
    delete[] inHeap;
    delete[] previous;

    return reachable;
}
