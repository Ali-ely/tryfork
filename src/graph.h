#ifndef GRAPH_H
#define GRAPH_H

#include <cstring>
#include <cstdio>

struct EdgeNode
{
    char to[64];
    int weight;
    EdgeNode *next;

    EdgeNode(const char *t, int w, EdgeNode *n = nullptr)
    {
        // strncpy(to, t, 63);
        // to[63] = '\0';
        
        //this way is supposed to be better in copying the string safely and place \0 in the correct location
        snprintf(to, sizeof(to), "%s", t);  // %s is called format string (copy the string argument)
        weight = w;
        next = n;
    }
};

// this is the list of the edgeNodes above
struct VertexNode
{
    char name[64];
    EdgeNode *adjHead; // Pointer to the head of this vertex's adjacency list (its neighbors)
    VertexNode *next;

    VertexNode(const char *n)
    {
        // strncpy(name, n, 63);
        // name[63] = '\0';
        snprintf (name, sizeof(name), "%s", n);
        adjHead = nullptr;
        next = nullptr;
    }
};

class Graph
{
private:
    VertexNode *head;

    // function to find a vertex node by name
    VertexNode *findVertexNode(const char *name) const;

public:
    Graph();
    ~Graph();

    void clear();                                          // Delete all vertices and edges
    void addVertex(const char *name);                      // Add a new vertex if it doesn’t exist
    void addEdge(const char *from, const char *to, int w); // Add connection between two vertices
    void display() const;
    void readDataset(const char *filename);
};

#endif
