#include "graph.h"
#include "DataManager.h"
#include <iostream>

Graph::Graph() : head(nullptr), vertexLookup(211)
{
}

Graph::~Graph()
{
    clear();
}

void Graph::clear()
{
    VertexNode *currVertex = head;

    // Loop all Vertex nodes
    while (currVertex)
    {
        EdgeNode *currEdge = currVertex->adjHead;

        // Delete all edges connected to this Vertex
        while (currEdge)
        {
            EdgeNode *nextEdge = currEdge->next;
            delete currEdge;
            currEdge = nextEdge;
        }

        VertexNode *nextVertex = currVertex->next;
        delete currVertex;
        currVertex = nextVertex;
    }

    head = nullptr;
    vertexLookup.clear();
}

VertexNode *Graph::findVertexNode(const char *name) const
{
    return static_cast<VertexNode *>(vertexLookup.find(name));
}

void Graph::addVertex(const char *name)
{
    if (findVertexNode(name))
        return;

    VertexNode *newVertex = new VertexNode(name);
    newVertex->next = head;
    head = newVertex;
    vertexLookup.insert(name, newVertex);
}

void Graph::addEdge(const char *from, const char *to, int w)
{
    VertexNode *fromVertex = findVertexNode(from);
    if (!fromVertex)
    {
        addVertex(from);
        fromVertex = findVertexNode(from);
    }

    VertexNode *toVertex = findVertexNode(to);
    if (!toVertex)
    {
        addVertex(to);
        toVertex = findVertexNode(to);
    }

    EdgeNode *newEdge1 = new EdgeNode(to, w);

    if (!fromVertex->adjHead)
        fromVertex->adjHead = newEdge1;
    else
    {
        EdgeNode *temp = fromVertex->adjHead;
        while (temp->next)
            temp = temp->next;

        temp->next = newEdge1;
    }

    // (Because this is an undirected graph)
    EdgeNode *newEdge2 = new EdgeNode(from, w);

    if (!toVertex->adjHead)
        toVertex->adjHead = newEdge2;
    else
    {
        EdgeNode *temp = toVertex->adjHead;
        while (temp->next)
            temp = temp->next;
        temp->next = newEdge2;
    }
}

void Graph::display() const
{
    VertexNode *Vertex = head;

    while (Vertex)
    {
        std::cout << Vertex->name << " -> ";

        EdgeNode *edge = Vertex->adjHead;
        while (edge)
        {
            std::cout << edge->to << " (" << edge->weight << ")";

            if (edge->next)
                std::cout << " -> ";

            edge = edge->next;
        }

        std::cout << std::endl;
        Vertex = Vertex->next;
    }
}

void Graph::readDataset(const char *filename)
{
    DataManager::loadText(filename, *this);
}

bool Graph::loadFromText(const char *filename)
{
    return DataManager::loadText(filename, *this);
}

bool Graph::loadFromBinary(const char *filename)
{
    return DataManager::loadBinary(filename, *this);
}

bool Graph::saveToText(const char *filename) const
{
    return DataManager::saveText(filename, *this);
}

bool Graph::saveToBinary(const char *filename) const
{
    return DataManager::saveBinary(filename, *this);
}

int Graph::getNumOfNodes() const
{
    int count = 0;
    VertexNode *temp = head;
    while (temp)
    {
        count++;
        temp = temp->next;
    }
    return count;
}

int Graph::getNeighborCount(const char *vertexName) const
{
    VertexNode *vertex = findVertexNode(vertexName);
    if (!vertex)
        return 0;

    int count = 0;
    EdgeNode *curr = vertex->adjHead;
    while (curr)
    {
        count++;
        curr = curr->next;
    }
    return count;
}