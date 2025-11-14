#include "graph.h"
#include <iostream>
#include <fstream>
#include <sstream>

Graph::Graph() : head(nullptr)
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
}

VertexNode *Graph::findVertexNode(const char *name) const
{
    VertexNode *temp = head;

    // loop the Vertex list
    while (temp)
    {
        if (strcmp(temp->name, name) == 0)
            return temp;
        temp = temp->next;
    }

    return nullptr;
}

void Graph::addVertex(const char *name)
{
    if (findVertexNode(name))
        return;

    VertexNode *newVertex = new VertexNode(name);

    if (!head)
    {
        head = newVertex;
    }
    else
    {
        VertexNode *temp = head;
        while (temp->next)
            temp = temp->next;

        temp->next = newVertex;
    }
}

void Graph::addEdge(const char *from, const char *to, int w)
{
    // check both cities exist or create if they don't
    addVertex(from);
    addVertex(to);

    VertexNode *fromVertex = findVertexNode(from);
    VertexNode *toVertex = findVertexNode(to);

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
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    std::string line;
    bool firstLine = true;

    while (std::getline(file, line))
    {
        if (line.empty())
            continue; // skip blank lines
        if (firstLine)
        { // skip header line
            firstLine = false;
            continue;
        }

        std::stringstream ss(line);
        std::string vertex1, vertex2;
        int weight;

        ss >> vertex1 >> vertex2 >> weight;

        if (vertex1.empty() || vertex2.empty() || ss.fail())
            continue;

        addEdge(vertex1.c_str(), vertex2.c_str(), weight);
    }

    file.close();
    std::cout << "Dataset successfully readed from: " << filename << std::endl;
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