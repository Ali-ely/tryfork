#include <gtest/gtest.h>

#include "../src/DataManager.h"

#include <filesystem>
#include <fstream>

namespace
{
    std::string makeTempFile(const std::string &suffix)
    {
        auto base = std::filesystem::temp_directory_path();
        auto path = base / std::filesystem::path("pathvista_" + suffix);
        return path.string();
    }
}

TEST(DataManagerTest, LoadTextFile)
{
    std::string tempPath = makeTempFile("sample.txt");
    {
        std::ofstream output(tempPath);
        output << "City1\tCity2\tDistance (km)\n";
        output << "A\tB\t10\n";
        output << "B\tC\t5\n";
    }

    Graph graph;
    EXPECT_TRUE(DataManager::loadText(tempPath.c_str(), graph));
    EXPECT_EQ(graph.getNeighborCount("A"), 1);
    EXPECT_EQ(graph.getNeighborCount("B"), 2);
    EXPECT_EQ(graph.getNeighborCount("C"), 1);

    std::filesystem::remove(tempPath);
}

TEST(DataManagerTest, BinaryRoundTrip)
{
    std::string tempPath = makeTempFile("graph.bin");

    Graph original;
    original.addEdge("A", "B", 10);
    original.addEdge("B", "C", 5);

    ASSERT_TRUE(DataManager::saveBinary(tempPath.c_str(), original));

    Graph loaded;
    ASSERT_TRUE(DataManager::loadBinary(tempPath.c_str(), loaded));

    EXPECT_EQ(loaded.getNeighborCount("A"), original.getNeighborCount("A"));
    EXPECT_EQ(loaded.getNeighborCount("B"), original.getNeighborCount("B"));
    EXPECT_EQ(loaded.getNeighborCount("C"), original.getNeighborCount("C"));

    std::filesystem::remove(tempPath);
}
