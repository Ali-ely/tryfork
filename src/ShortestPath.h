#ifndef SHORTESTPATH_H
#define SHORTESTPATH_H

#include "graph.h"

struct PathResult
{
	float totalDistance;
	char **nodes;
	int nodeCount;
	bool reachable;

	PathResult()
		: totalDistance(0.0f), nodes(nullptr), nodeCount(0), reachable(false)
	{
	}
};

class ShortestPath
{
public:
	ShortestPath() = default;
	~ShortestPath() = default;

	bool compute(Graph &graph, const char *start, const char *destination, PathResult &result);
	void release(PathResult &result);

private:
	void resetResult(PathResult &result);
};

#endif