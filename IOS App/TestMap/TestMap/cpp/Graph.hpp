//
//  Graph.hpp
//  TestMap
//

#ifndef Graph_hpp
#define Graph_hpp

#include "CoreTypes.h"
#include "HashTable.hpp"
#include <string>

struct EdgeNode {
    std::string to; // destination id
    double weight;
    std::string edgeId;
    GeomNode *geometryHead; // singly-linked geometry list (may be nullptr)
    EdgeNode *next;

    EdgeNode(const std::string &t, double w, const std::string &eid, EdgeNode *n = nullptr)
    : to(t), weight(w), edgeId(eid), geometryHead(nullptr), next(n) {}
    ~EdgeNode() {
        // free geometry list
        GeomNode *g = geometryHead;
        while (g) {
            GeomNode *tmp = g;
            g = g->next;
            delete tmp;
        }
    }
};

struct VertexNode {
    std::string id;   // Unique key (e.g., "r263")
    std::string name; // Display name
    double lat;
    double lon;
    EdgeNode *adjHead;
    VertexNode *next;

    VertexNode(const std::string &uniqueId, const std::string &displayName, double l, double ln)
    : id(uniqueId), name(displayName), lat(l), lon(ln), adjHead(nullptr), next(nullptr) {}
    ~VertexNode() {
        // delete adjacency edges
        EdgeNode *e = adjHead;
        while (e) {
            EdgeNode *tmp = e;
            e = e->next;
            delete tmp;
        }
    }
};

class Graph {
private:
    VertexNode *head;
    HashTable vertexLookup; // maps id -> VertexNode*

public:
    Graph();
    ~Graph();

    VertexNode *getHead() const { return head; }
    VertexNode *findVertexNode(const char *id) const; // lookup by ID

    void clear();
    void addVertex(const char *id, const char *name, double lat, double lon);
    void addEdge(const std::string &fromId, const std::string &toId, double w, const std::string &edgeId, GeomNode *geomHead);

    int getNumOfNodes() const;
    const GeomNode* getEdgeGeometry(const char* fromId, const char* toId) const;

    // Helper to find ID if we only have the Name
    std::string findIdByName(const std::string &queryName) const;
};

#endif /* Graph_hpp */
