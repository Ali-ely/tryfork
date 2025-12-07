//
//  ShortestPath.hpp
//  TestMap
//

#ifndef ShortestPath_hpp
#define ShortestPath_hpp

#include "Graph.hpp"
#include "CoreTypes.h"

struct PathResult {
    double totalDistance;
    PathNode *head; // linked list of path node ids in order (start -> ... -> dest)
    bool reachable;
    PathResult() : totalDistance(0.0), head(nullptr), reachable(false) {}
};

class ShortestPath {
public:
    // Compute shortest path from 'start' to 'destination'.
    // Returns true if reachable and fills result.head as a linked list of node IDs.
    bool compute(Graph &graph, const char *start, const char *destination, PathResult &result);
};

#endif /* ShortestPath_hpp */
