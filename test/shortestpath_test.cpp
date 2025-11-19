#include <gtest/gtest.h>

#include "../src/ShortestPath.h"

class ShortestPathTest : public ::testing::Test
{
protected:
    Graph graph;
    ShortestPath shortestPath;
    PathResult result;

    void TearDown() override
    {
        shortestPath.release(result);
    }
};

TEST_F(ShortestPathTest, FindsDirectPath)
{
    graph.addEdge("A", "B", 5);
    graph.addEdge("B", "C", 7);

    bool found = shortestPath.compute(graph, "A", "C", result);

    ASSERT_TRUE(found);
    EXPECT_TRUE(result.reachable);
    ASSERT_EQ(result.nodeCount, 3);
    EXPECT_STREQ(result.nodes[0], "A");
    EXPECT_STREQ(result.nodes[1], "B");
    EXPECT_STREQ(result.nodes[2], "C");
    EXPECT_FLOAT_EQ(result.totalDistance, 12.0f);
}

TEST_F(ShortestPathTest, ReturnsFalseForMissingVertex)
{
    graph.addEdge("A", "B", 5);

    bool found = shortestPath.compute(graph, "A", "Z", result);

    EXPECT_FALSE(found);
    EXPECT_FALSE(result.reachable);
    EXPECT_EQ(result.nodeCount, 0);
}

TEST_F(ShortestPathTest, HandlesDisconnectedGraph)
{
    graph.addEdge("A", "B", 5);
    graph.addEdge("C", "D", 3);

    bool found = shortestPath.compute(graph, "A", "D", result);

    EXPECT_FALSE(found);
    EXPECT_FALSE(result.reachable);
    EXPECT_EQ(result.nodeCount, 0);
}
