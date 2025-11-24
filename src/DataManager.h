#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include "graph.h"
#include "ShortestPath.h"

class DataManager
{
public:
    static bool loadText(const char *filename, Graph &graph);
    static bool loadBinary(const char *filename, Graph &graph);
    static bool saveText(const char *filename, const Graph &graph);
    static bool saveBinary(const char *filename, const Graph &graph);
    static bool saveResultAsJSON(const char *filename, const PathResult &result);
    static bool loadJSON(const char *filename, Graph &graph);
};

#endif
