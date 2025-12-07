#include <gtest/gtest.h>

#include "../src/DataManager.h"
#include "../src/ShortestPath.h"

#include <filesystem>
#include <fstream>
#include <cstring>

namespace
{
    std::string makeTempFile(const std::string &suffix)
    {
        auto base = std::filesystem::temp_directory_path();
        auto path = base / std::filesystem::path("pathvista_" + suffix);
        return path.string();
    }

    void cleanupFile(const std::string &path)
    {
        std::filesystem::remove(path);
    }
}

// ==================== loadText Tests ====================

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
    EXPECT_EQ(graph.getNumOfNodes(), 3);

    cleanupFile(tempPath);
}

TEST(DataManagerTest, LoadTextFileNonexistent)
{
    Graph graph;
    EXPECT_FALSE(DataManager::loadText("nonexistent_file_12345.txt", graph));
}

TEST(DataManagerTest, LoadTextFileEmptyFile)
{
    std::string tempPath = makeTempFile("empty.txt");
    {
        std::ofstream output(tempPath);
        // Write nothing
    }

    Graph graph;
    EXPECT_TRUE(DataManager::loadText(tempPath.c_str(), graph));
    EXPECT_EQ(graph.getNumOfNodes(), 0);

    cleanupFile(tempPath);
}

TEST(DataManagerTest, LoadTextFileHeaderOnly)
{
    std::string tempPath = makeTempFile("header_only.txt");
    {
        std::ofstream output(tempPath);
        output << "City1\tCity2\tDistance (km)\n";
    }

    Graph graph;
    EXPECT_TRUE(DataManager::loadText(tempPath.c_str(), graph));
    EXPECT_EQ(graph.getNumOfNodes(), 0);

    cleanupFile(tempPath);
}

TEST(DataManagerTest, LoadTextFileSkipsInvalidLines)
{
    std::string tempPath = makeTempFile("invalid_lines.txt");
    {
        std::ofstream output(tempPath);
        output << "City1\tCity2\tDistance (km)\n";
        output << "A\tB\t10\n";
        output << "Invalid line without tabs\n";
        output << "\n"; // Empty line
        output << "C\tD\t20\n";
        output << "E\tF\tnotanumber\n"; // Invalid distance
    }

    Graph graph;
    EXPECT_TRUE(DataManager::loadText(tempPath.c_str(), graph));
    // Only A-B and C-D should be added
    EXPECT_EQ(graph.getNumOfNodes(), 4);
    EXPECT_EQ(graph.getNeighborCount("A"), 1);
    EXPECT_EQ(graph.getNeighborCount("C"), 1);

    cleanupFile(tempPath);
}

TEST(DataManagerTest, LoadTextFileWithExtraWhitespace)
{
    std::string tempPath = makeTempFile("whitespace.txt");
    {
        std::ofstream output(tempPath);
        output << "City1\tCity2\tDistance (km)\n";
        output << "  A  \t  B  \t  10  \n";
    }

    Graph graph;
    EXPECT_TRUE(DataManager::loadText(tempPath.c_str(), graph));
    EXPECT_EQ(graph.getNumOfNodes(), 2);
    EXPECT_EQ(graph.getNeighborCount("A"), 1);
    EXPECT_EQ(graph.getNeighborCount("B"), 1);

    cleanupFile(tempPath);
}

// ==================== Binary Round-Trip Tests ====================

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
    EXPECT_EQ(loaded.getNumOfNodes(), original.getNumOfNodes());

    cleanupFile(tempPath);
}

TEST(DataManagerTest, BinaryRoundTripEmptyGraph)
{
    std::string tempPath = makeTempFile("empty_graph.bin");

    Graph original;
    ASSERT_TRUE(DataManager::saveBinary(tempPath.c_str(), original));

    Graph loaded;
    ASSERT_TRUE(DataManager::loadBinary(tempPath.c_str(), loaded));

    EXPECT_EQ(loaded.getNumOfNodes(), 0);

    cleanupFile(tempPath);
}

TEST(DataManagerTest, BinaryRoundTripLargerGraph)
{
    std::string tempPath = makeTempFile("larger_graph.bin");

    Graph original;
    original.addEdge("Cairo", "Alexandria", 220);
    original.addEdge("Cairo", "Giza", 20);
    original.addEdge("Alexandria", "Giza", 200);
    original.addEdge("Luxor", "Aswan", 240);
    original.addEdge("Cairo", "Luxor", 670);

    ASSERT_TRUE(DataManager::saveBinary(tempPath.c_str(), original));

    Graph loaded;
    ASSERT_TRUE(DataManager::loadBinary(tempPath.c_str(), loaded));

    EXPECT_EQ(loaded.getNumOfNodes(), original.getNumOfNodes());
    EXPECT_EQ(loaded.getNeighborCount("Cairo"), original.getNeighborCount("Cairo"));
    EXPECT_EQ(loaded.getNeighborCount("Alexandria"), original.getNeighborCount("Alexandria"));
    EXPECT_EQ(loaded.getNeighborCount("Luxor"), original.getNeighborCount("Luxor"));

    cleanupFile(tempPath);
}

TEST(DataManagerTest, LoadBinaryNonexistent)
{
    Graph graph;
    EXPECT_FALSE(DataManager::loadBinary("nonexistent_file_12345.bin", graph));
}

TEST(DataManagerTest, LoadBinaryInvalidMagic)
{
    std::string tempPath = makeTempFile("invalid_magic.bin");
    {
        std::ofstream output(tempPath, std::ios::binary);
        char garbage[] = "GARBAGE!";
        output.write(garbage, sizeof(garbage));
    }

    Graph graph;
    EXPECT_FALSE(DataManager::loadBinary(tempPath.c_str(), graph));

    cleanupFile(tempPath);
}

// ==================== Text Round-Trip Tests ====================

TEST(DataManagerTest, TextRoundTrip)
{
    std::string tempPath = makeTempFile("text_roundtrip.txt");

    Graph original;
    original.addEdge("X", "Y", 100);
    original.addEdge("Y", "Z", 50);

    ASSERT_TRUE(DataManager::saveText(tempPath.c_str(), original));

    Graph loaded;
    ASSERT_TRUE(DataManager::loadText(tempPath.c_str(), loaded));

    EXPECT_EQ(loaded.getNumOfNodes(), original.getNumOfNodes());
    EXPECT_EQ(loaded.getNeighborCount("X"), original.getNeighborCount("X"));
    EXPECT_EQ(loaded.getNeighborCount("Y"), original.getNeighborCount("Y"));
    EXPECT_EQ(loaded.getNeighborCount("Z"), original.getNeighborCount("Z"));

    cleanupFile(tempPath);
}

TEST(DataManagerTest, SaveTextToInvalidPath)
{
    Graph graph;
    graph.addEdge("A", "B", 10);
    // Trying to save to an invalid directory path
    EXPECT_FALSE(DataManager::saveText("/nonexistent_dir_12345/file.txt", graph));
}

TEST(DataManagerTest, SaveBinaryToInvalidPath)
{
    Graph graph;
    graph.addEdge("A", "B", 10);
    EXPECT_FALSE(DataManager::saveBinary("/nonexistent_dir_12345/file.bin", graph));
}

// ==================== JSON Result Saving Tests ====================

TEST(DataManagerTest, SaveResultAsJSON)
{
    std::string tempPath = makeTempFile("result.json");

    PathResult result;
    result.totalDistance = 150.5f;
    result.reachable = true;
    result.nodeCount = 3;
    result.nodes = new char *[3];
    for (int i = 0; i < 3; ++i)
    {
        result.nodes[i] = new char[64];
    }
    std::strcpy(result.nodes[0], "Start");
    std::strcpy(result.nodes[1], "Middle");
    std::strcpy(result.nodes[2], "End");

    ASSERT_TRUE(DataManager::saveResultAsJSON(tempPath.c_str(), result));

    // Verify file exists and contains expected content
    std::string content;
    {
        std::ifstream input(tempPath);
        ASSERT_TRUE(input.is_open());
        content = std::string((std::istreambuf_iterator<char>(input)),
                              std::istreambuf_iterator<char>());
        input.close();
    }

    EXPECT_NE(content.find("\"totalDistance\":"), std::string::npos);
    EXPECT_NE(content.find("\"reachable\": true"), std::string::npos);
    EXPECT_NE(content.find("\"nodeCount\": 3"), std::string::npos);
    EXPECT_NE(content.find("\"Start\""), std::string::npos);
    EXPECT_NE(content.find("\"Middle\""), std::string::npos);
    EXPECT_NE(content.find("\"End\""), std::string::npos);

    // Cleanup allocated memory
    for (int i = 0; i < 3; ++i)
    {
        delete[] result.nodes[i];
    }
    delete[] result.nodes;

    cleanupFile(tempPath);
}

TEST(DataManagerTest, SaveResultAsJSONUnreachable)
{
    std::string tempPath = makeTempFile("unreachable.json");

    PathResult result;
    result.totalDistance = 0.0f;
    result.reachable = false;
    result.nodeCount = 0;
    result.nodes = nullptr;

    ASSERT_TRUE(DataManager::saveResultAsJSON(tempPath.c_str(), result));

    std::string content;
    {
        std::ifstream input(tempPath);
        ASSERT_TRUE(input.is_open());
        content = std::string((std::istreambuf_iterator<char>(input)),
                              std::istreambuf_iterator<char>());
        input.close();
    }

    EXPECT_NE(content.find("\"reachable\": false"), std::string::npos);
    EXPECT_NE(content.find("\"nodeCount\": 0"), std::string::npos);

    cleanupFile(tempPath);
}

TEST(DataManagerTest, SaveResultAsJSONInvalidPath)
{
    PathResult result;
    result.totalDistance = 0.0f;
    result.reachable = false;
    result.nodeCount = 0;
    result.nodes = nullptr;

    EXPECT_FALSE(DataManager::saveResultAsJSON("/nonexistent_dir_12345/result.json", result));
}

// ==================== Graph Clear Before Load Tests ====================

TEST(DataManagerTest, LoadTextClearsPreviousData)
{
    std::string tempPath = makeTempFile("clear_test.txt");
    {
        std::ofstream output(tempPath);
        output << "City1\tCity2\tDistance (km)\n";
        output << "NewA\tNewB\t100\n";
    }

    Graph graph;
    graph.addEdge("OldX", "OldY", 50);
    EXPECT_EQ(graph.getNumOfNodes(), 2);

    EXPECT_TRUE(DataManager::loadText(tempPath.c_str(), graph));
    
    // Old data should be cleared
    EXPECT_EQ(graph.getNeighborCount("OldX"), 0);
    EXPECT_EQ(graph.getNeighborCount("OldY"), 0);
    
    // New data should be present
    EXPECT_EQ(graph.getNumOfNodes(), 2);
    EXPECT_EQ(graph.getNeighborCount("NewA"), 1);
    EXPECT_EQ(graph.getNeighborCount("NewB"), 1);

    cleanupFile(tempPath);
}

TEST(DataManagerTest, LoadBinaryClearsPreviousData)
{
    std::string tempPath = makeTempFile("clear_binary_test.bin");

    Graph toSave;
    toSave.addEdge("NewA", "NewB", 100);
    ASSERT_TRUE(DataManager::saveBinary(tempPath.c_str(), toSave));

    Graph graph;
    graph.addEdge("OldX", "OldY", 50);
    EXPECT_EQ(graph.getNumOfNodes(), 2);

    EXPECT_TRUE(DataManager::loadBinary(tempPath.c_str(), graph));
    
    // Old data should be cleared
    EXPECT_EQ(graph.getNeighborCount("OldX"), 0);
    EXPECT_EQ(graph.getNeighborCount("OldY"), 0);
    
    // New data should be present
    EXPECT_EQ(graph.getNumOfNodes(), 2);
    EXPECT_EQ(graph.getNeighborCount("NewA"), 1);
    EXPECT_EQ(graph.getNeighborCount("NewB"), 1);

    cleanupFile(tempPath);
}
