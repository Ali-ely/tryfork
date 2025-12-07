#include <gtest/gtest.h>
#include "../src/graph.h"
#include <fstream>
#include <filesystem>

using namespace std;

namespace
{
    std::string makeTempFile(const std::string &suffix)
    {
        auto base = std::filesystem::temp_directory_path();
        auto path = base / std::filesystem::path("pathvista_graph_" + suffix);
        return path.string();
    }

    void cleanupFile(const std::string &path)
    {
        std::filesystem::remove(path);
    }
}

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
    // Create a temporary dataset file for testing
    // DataManager::loadText expects tab-separated values
    std::string tempPath = makeTempFile("dataset.txt");
    {
        std::ofstream output(tempPath);
        output << "City1\tCity2\tDistance (km)\n";
        output << "Cairo\tAlexandria\t220\n";
        output << "Cairo\tGiza\t20\n";
        output << "Alexandria\tGiza\t200\n";
    }

    graph->readDataset(tempPath.c_str());

    testing::internal::CaptureStdout();
    graph->display();
    string output = testing::internal::GetCapturedStdout();

    EXPECT_FALSE(output.empty());
    EXPECT_TRUE(output.find("Cairo") != string::npos);
    EXPECT_TRUE(output.find("Alexandria") != string::npos);
    EXPECT_TRUE(output.find("Giza") != string::npos);

    cleanupFile(tempPath);
}
