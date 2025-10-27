#include <gtest/gtest.h>
#include "../src/graph.h"
#include <fstream>
using namespace std;
class GraphTest : public ::testing::Test
{
protected:
    Graph *graph;

    void SetUp() override
    {
        graph = new Graph();
    }

    void TearDown() override
    {
        delete graph;
    }
};

TEST_F(GraphTest, AddVertex)
{
    graph->addVertex("Cairo");

    testing::internal::CaptureStdout();
    graph->display();
    string output = testing::internal::GetCapturedStdout();

    EXPECT_TRUE(output.find("Cairo") != string::npos);
}

TEST_F(GraphTest, AddEdge)
{
    graph->addEdge("Cairo", "Alexandria", 100);

    testing::internal::CaptureStdout();
    graph->display();
    string output = testing::internal::GetCapturedStdout();

    EXPECT_TRUE(output.find("Alexandria (100)") != string::npos);
}

TEST_F(GraphTest, GraphIsUndirected)
{
    graph->addEdge("Cairo", "Alexandria", 100);

    testing::internal::CaptureStdout();
    graph->display();
    string output = testing::internal::GetCapturedStdout();

    EXPECT_TRUE(output.find("Cairo ->") != string::npos);
    EXPECT_TRUE(output.find("Alexandria ->") != string::npos);
}

TEST_F(GraphTest, Clear)
{
    graph->addEdge("Cairo", "Alexandria", 100);
    graph->clear();

    testing::internal::CaptureStdout();
    graph->display();
    string output = testing::internal::GetCapturedStdout();

    EXPECT_TRUE(output.empty());
}

TEST_F(GraphTest, ReadDataset)
{
    graph->readDataset("../../data/city_connections_dataset.txt");

    testing::internal::CaptureStdout();
    graph->display();
    string output = testing::internal::GetCapturedStdout();

    EXPECT_FALSE(output.empty());
}
