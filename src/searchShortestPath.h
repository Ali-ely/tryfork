#ifndef searchShortestPath_H
#define searchShortestPath_H

#include "MinHeap.h"
#include "graph.h"
struct heapNode
{
    char cityName[64];
    int distance;

    bool operator<(const heapNode& other) const {
        return distance < other.distance;
    }
    bool operator>(const heapNode& other) const {
        return distance > other.distance;
    }
};

void shortestPath(Graph& graph, const char* start, const char* destination);

#endif