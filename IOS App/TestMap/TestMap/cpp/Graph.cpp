//
//  Graph.cpp
//  TestMap
//


#include "Graph.hpp"
#include <cstring>
#include <cmath>

Graph::Graph() : head(nullptr), vertexLookup(2000) {}

Graph::~Graph() {
    clear();
}

void Graph::clear() {
    VertexNode *currVertex = head;
    while (currVertex) {
        VertexNode *nextVertex = currVertex->next;
        delete currVertex;
        currVertex = nextVertex;
    }
    head = nullptr;
    vertexLookup.clear();
}

VertexNode *Graph::findVertexNode(const char *id) const {
    if (!id) return nullptr;
    return static_cast<VertexNode *>(vertexLookup.find(id));
}

void Graph::addVertex(const char *idC, const char *nameC, double lat, double lon) {
    if (!idC) return;
    std::string id(idC);
    if (findVertexNode(id.c_str())) return; // prevent duplicates

    std::string name = nameC ? std::string(nameC) : id;
    VertexNode *newVertex = new VertexNode(id, name, lat, lon);

    newVertex->next = head;
    head = newVertex;

    vertexLookup.insert(id.c_str(), newVertex);
}

// Helper: clone linked list geomHead (returns head of new list). Caller should free.
static GeomNode* cloneGeomList(const GeomNode *src) {
    if (!src) return nullptr;
    GeomNode *newHead = new GeomNode(src->lat, src->lon, nullptr);
    GeomNode *tail = newHead;
    const GeomNode *it = src->next;
    while (it) {
        tail->next = new GeomNode(it->lat, it->lon, nullptr);
        tail = tail->next;
        it = it->next;
    }
    return newHead;
}

// Helper: reverse a geometry linked list and return new head (caller owns result)
static GeomNode* reverseGeomList(const GeomNode *src) {
    GeomNode *result = nullptr;
    const GeomNode *it = src;
    while (it) {
        GeomNode *n = new GeomNode(it->lat, it->lon, result);
        result = n;
        it = it->next;
    }
    return result;
}

void Graph::addEdge(const std::string &fromId, const std::string &toId, double w, const std::string &edgeId, GeomNode *geomHead) {
    VertexNode *fromVertex = findVertexNode(fromId.c_str());
    VertexNode *toVertex = findVertexNode(toId.c_str());

    if (!fromVertex || !toVertex) {
        // If geomHead was allocated by caller, free it to avoid leak
        GeomNode *g = geomHead;
        while (g) { GeomNode *tmp = g; g = g->next; delete tmp; }
        return;
    }

    // Add U -> V
    EdgeNode *newEdge1 = new EdgeNode(toId, w, edgeId);
    newEdge1->geometryHead = cloneGeomList(geomHead);
    newEdge1->next = fromVertex->adjHead;
    fromVertex->adjHead = newEdge1;

    // Add V -> U (reverse geometry)
    EdgeNode *newEdge2 = new EdgeNode(fromId, w, edgeId);
    if (geomHead) {
        newEdge2->geometryHead = reverseGeomList(geomHead);
    } else {
        newEdge2->geometryHead = nullptr;
    }
    newEdge2->next = toVertex->adjHead;
    toVertex->adjHead = newEdge2;

    // free caller-owned geomHead (we cloned it)
    GeomNode *g = geomHead;
    while (g) { GeomNode *tmp = g; g = g->next; delete tmp; }
}

int Graph::getNumOfNodes() const {
    int count = 0;
    VertexNode *temp = head;
    while (temp) {
        ++count;
        temp = temp->next;
    }
    return count;
}

const GeomNode* Graph::getEdgeGeometry(const char* from, const char* to) const {
    if (!from || !to) return nullptr;
    VertexNode *v = findVertexNode(from);
    if (!v) return nullptr;

    EdgeNode *e = v->adjHead;
    while (e) {
        if (e->to == to) {
            return e->geometryHead;
        }
        e = e->next;
    }
    return nullptr;
}

// Helper: toLower without algorithm
static std::string toLower(const std::string &s) {
    std::string out;
    out.reserve(s.size());
    for (size_t i = 0; i < s.size(); ++i) {
        char c = s[i];
        if (c >= 'A' && c <= 'Z') c = char(c - 'A' + 'a');
        out.push_back(c);
    }
    return out;
}

std::string Graph::findIdByName(const std::string &queryName) const {
    if (queryName.empty()) return "";

    std::string queryLower = toLower(queryName);

    VertexNode *curr = head;
    while(curr) {
        std::string nodeNameLower = toLower(curr->name);
        if (nodeNameLower == queryLower) return curr->id;

        std::string nodeIdLower = toLower(curr->id);
        if (nodeIdLower == queryLower) return curr->id;

        curr = curr->next;
    }
    return ""; // not found
}
