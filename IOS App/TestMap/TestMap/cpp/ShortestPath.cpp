//
//  ShortestPath.cpp
//  TestMap
//

#include "ShortestPath.hpp"
#include "DijkstraMinHeap.hpp"
#include "HashTable.hpp"
#include <limits>
#include <new>
#include <cstring>

// Helper: count vertices via Graph API (you have Graph::getNumOfNodes())
static int countVertices(Graph &graph) {
    return graph.getNumOfNodes();
}

// Helper: reverse a PathNode linked list; returns new head (caller owns)
static PathNode* reversePathNodeList(PathNode *head) {
    PathNode *prev = nullptr;
    PathNode *cur = head;
    while (cur) {
        PathNode *next = cur->next;
        cur->next = prev;
        prev = cur;
        cur = next;
    }
    return prev;
}

// Create a PathNode list from an array of indices (idByIndex) in order
static PathNode* buildPathNodeListFromIndices(const std::string *idByIndex, int *indices, int length) {
    if (length <= 0) return nullptr;
    PathNode *head = nullptr;
    PathNode *tail = nullptr;
    for (int i = 0; i < length; ++i) {
        PathNode *n = new PathNode(idByIndex[indices[i]]);
        if (!head) {
            head = n;
            tail = n;
        } else {
            tail->next = n;
            tail = n;
        }
    }
    return head;
}

bool ShortestPath::compute(Graph &graph, const char *start, const char *destination, PathResult &result) {
    // initialize result
    result.head = nullptr;
    result.reachable = false;
    result.totalDistance = 0.0;

    if (!start || !destination) return false;

    // 1) Count vertices
    int n = countVertices(graph);
    if (n <= 0) return false;

    // 2) Build idByIndex and nodesByIndex arrays by scanning graph's linked list of vertices
    VertexNode **nodesByIndex = nullptr;
    std::string *idByIndex = nullptr;
    int *indexStorage = nullptr;

    try {
        nodesByIndex = new VertexNode*[n];
        idByIndex = new std::string[n];
        indexStorage = new int[n];
    } catch (const std::bad_alloc &) {
        delete[] nodesByIndex;
        delete[] idByIndex;
        delete[] indexStorage;
        return false;
    }

    // Fill arrays
    VertexNode *it = graph.getHead();
    int idx = 0;
    while (it && idx < n) {
        nodesByIndex[idx] = it;
        idByIndex[idx] = it->id;
        indexStorage[idx] = idx; // pointer storage for HashTable values
        it = it->next;
        ++idx;
    }

    // If count mismatch (defensive)
    if (idx != n) {
        // fallback: adjust n to actual count
        n = idx;
        // Note: in this unlikely case we could shrink arrays or continue,
        // but we'll proceed with current arrays (unused tail entries won't be touched).
    }

    // 3) Build HashTable id -> &indexStorage[i]
    // choose capacity ~ 2*n + 1 for lower collision rate
    int htCap = n * 2 + 1;
    if (htCap < 11) htCap = 11;
    HashTable idIndexMap(htCap);

    for (int i = 0; i < n; ++i) {
        idIndexMap.insert(idByIndex[i].c_str(), &indexStorage[i]);
    }

    // 4) Lookup start and dest indices
    void *vptr = idIndexMap.find(start);
    int startIndex = -1;
    if (vptr) {
        startIndex = *((int *)vptr);
    } else {
        // try resolving by name via Graph helper
        std::string resolvedStart = graph.findIdByName(std::string(start));
        if (!resolvedStart.empty()) {
            vptr = idIndexMap.find(resolvedStart.c_str());
            if (vptr) startIndex = *((int *)vptr);
        }
    }

    vptr = idIndexMap.find(destination);
    int destIndex = -1;
    if (vptr) {
        destIndex = *((int *)vptr);
    } else {
        std::string resolvedDest = graph.findIdByName(std::string(destination));
        if (!resolvedDest.empty()) {
            vptr = idIndexMap.find(resolvedDest.c_str());
            if (vptr) destIndex = *((int *)vptr);
        }
    }

    if (startIndex < 0 || destIndex < 0) {
        // cleanup
        delete[] nodesByIndex;
        delete[] idByIndex;
        delete[] indexStorage;
        return false;
    }

    // 5) Prepare Dijkstra structures
    double *dist = nullptr;
    int *prev = nullptr;
    try {
        dist = new double[n];
        prev = new int[n];
    } catch (const std::bad_alloc &) {
        delete[] nodesByIndex;
        delete[] idByIndex;
        delete[] indexStorage;
        delete[] dist;
        delete[] prev;
        return false;
    }

    const double INF = std::numeric_limits<double>::infinity();
    for (int i = 0; i < n; ++i) {
        dist[i] = INF;
        prev[i] = -1;
    }

    // create heap: heapCapacity = n, maxVertexIndex = n - 1
    DijkstraMinHeap heap(n, n - 1);

    // Insert all nodes into heap with INF, then decreaseKey for start to 0
    for (int i = 0; i < n; ++i) {
        heap.insert(i, INF);
    }
    dist[startIndex] = 0.0;
    heap.decreaseKey(startIndex, 0.0);

    // 6) Dijkstra main loop
    while (!heap.isEmpty()) {
        HeapNode hn = heap.extractMin();
        int u = hn.vertexIndex;
        double d = hn.distance;
        if (d == INF) break; // remaining vertices unreachable
        if (u == destIndex) break; // early exit if reached destination

        // Traverse adjacency edges of nodesByIndex[u]
        VertexNode *vNode = nodesByIndex[u];
        if (!vNode) continue;
        EdgeNode *edge = vNode->adjHead;
        while (edge) {
            // find neighbor index
            void *p = idIndexMap.find(edge->to.c_str());
            if (p) {
                int vIdx = *((int *)p);
                double alt = d + edge->weight;
                if (alt < dist[vIdx]) {
                    dist[vIdx] = alt;
                    prev[vIdx] = u;
                    heap.decreaseKey(vIdx, alt);
                }
            }
            // else: neighbor id not found in mapping (shouldn't happen), skip
            edge = edge->next;
        }
    }

    // 7) Check reachability
    if (dist[destIndex] == INF) {
        // no path
        delete[] nodesByIndex;
        delete[] idByIndex;
        delete[] indexStorage;
        delete[] dist;
        delete[] prev;
        return false;
    }

    // 8) Reconstruct path by walking prev[] from dest -> start
    // We'll collect indices by building a small temporary array of length up to n
    int *revIndices = nullptr;
    try {
        revIndices = new int[n];
    } catch (const std::bad_alloc &) {
        delete[] nodesByIndex;
        delete[] idByIndex;
        delete[] indexStorage;
        delete[] dist;
        delete[] prev;
        return false;
    }

    int pathLen = 0;
    int cur = destIndex;
    while (cur != -1 && pathLen < n) {
        revIndices[pathLen++] = cur;
        if (cur == startIndex) break;
        cur = prev[cur];
    }

    // If we didn't reach start during backtracking, fail-safe
    if (pathLen == 0 || revIndices[pathLen - 1] != startIndex) {
        delete[] nodesByIndex;
        delete[] idByIndex;
        delete[] indexStorage;
        delete[] dist;
        delete[] prev;
        delete[] revIndices;
        return false;
    }

    // Reverse indices into forward order
    int *indices = nullptr;
    try {
        indices = new int[pathLen];
    } catch (const std::bad_alloc &) {
        delete[] nodesByIndex;
        delete[] idByIndex;
        delete[] indexStorage;
        delete[] dist;
        delete[] prev;
        delete[] revIndices;
        return false;
    }

    for (int i = 0; i < pathLen; ++i) {
        indices[i] = revIndices[pathLen - 1 - i];
    }

    // 9) Build PathNode linked list in forward order
    PathNode *pathHead = buildPathNodeListFromIndices(idByIndex, indices, pathLen);

    // 10) Fill result
    result.head = pathHead;
    result.reachable = true;
    result.totalDistance = dist[destIndex];

    // 11) Cleanup temporaries
    delete[] nodesByIndex;
    delete[] idByIndex;
    delete[] indexStorage;
    delete[] dist;
    delete[] prev;
    delete[] revIndices;
    delete[] indices;

    return true;
}
