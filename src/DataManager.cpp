#include "DataManager.h"

#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>

namespace
{
#pragma pack(push, 1)
    struct BinaryHeader
    {
        char magic[8];
        uint32_t version;
        uint32_t vertexCount;
        uint32_t edgeCount;
    };

    struct BinaryEdge
    {
        char from[64];
        char to[64];
        int weight;
    };
#pragma pack(pop)

    constexpr char MAGIC_VALUE[8] = {'P', 'V', 'G', 'R', 'A', 'P', 'H', '\0'};
    constexpr uint32_t BINARY_VERSION = 1;

    void trimInPlace(std::string &value)
    {
        size_t start = 0;
        while (start < value.size() && std::isspace(static_cast<unsigned char>(value[start])))
            ++start;

        size_t end = value.size();
        while (end > start && std::isspace(static_cast<unsigned char>(value[end - 1])))
            --end;

        value = value.substr(start, end - start);
    }

    bool shouldEmitEdge(const char *from, const char *to)
    {
        return std::strcmp(from, to) < 0;
    }

    uint32_t countVertices(const Graph &graph)
    {
        uint32_t count = 0;
        VertexNode *curr = graph.getHead();
        while (curr)
        {
            ++count;
            curr = curr->next;
        }
        return count;
    }

    uint32_t countUniqueEdges(const Graph &graph)
    {
        uint32_t count = 0;
        VertexNode *vertex = graph.getHead();
        while (vertex)
        {
            EdgeNode *edge = vertex->adjHead;
            while (edge)
            {
                if (shouldEmitEdge(vertex->name, edge->to))
                    ++count;
                edge = edge->next;
            }
            vertex = vertex->next;
        }
        return count;
    }

    void writeEdge(BinaryEdge &record, const char *from, const char *to, int weight)
    {
        std::snprintf(record.from, sizeof(record.from), "%s", from);
        std::snprintf(record.to, sizeof(record.to), "%s", to);
        record.weight = weight;
    }
}

bool DataManager::loadText(const char *filename, Graph &graph)
{
    std::ifstream input(filename);
    if (!input.is_open())
        return false;

    graph.clear();

    std::string line;
    bool firstLine = true;
    while (std::getline(input, line))
    {
        if (line.empty())
            continue;

        std::stringstream ss(line);
        std::string from, to, distanceStr;
        if (!std::getline(ss, from, '\t'))
            continue;
        if (!std::getline(ss, to, '\t'))
            continue;
        if (!std::getline(ss, distanceStr))
            continue;

        trimInPlace(from);
        trimInPlace(to);
        trimInPlace(distanceStr);

        if (firstLine && distanceStr.find("Distance") != std::string::npos)
        {
            firstLine = false;
            continue;
        }
        firstLine = false;

        if (from.empty() || to.empty() || distanceStr.empty())
            continue;

        int distance = 0;
        try
        {
            distance = std::stoi(distanceStr);
        }
        catch (...)
        {
            continue;
        }

        graph.addEdge(from.c_str(), to.c_str(), distance);
    }

    return true;
}

bool DataManager::loadBinary(const char *filename, Graph &graph)
{
    std::ifstream input(filename, std::ios::binary);
    if (!input.is_open())
        return false;

    BinaryHeader header{};
    if (!input.read(reinterpret_cast<char *>(&header), sizeof(header)))
        return false;

    if (std::memcmp(header.magic, MAGIC_VALUE, sizeof(header.magic)) != 0)
        return false;
    if (header.version != BINARY_VERSION)
        return false;

    graph.clear();

    for (uint32_t i = 0; i < header.edgeCount; ++i)
    {
        BinaryEdge record{};
        if (!input.read(reinterpret_cast<char *>(&record), sizeof(record)))
            return false;

        graph.addEdge(record.from, record.to, record.weight);
    }

    return true;
}

bool DataManager::saveText(const char *filename, const Graph &graph)
{
    std::ofstream output(filename);
    if (!output.is_open())
        return false;

    output << "City1\tCity2\tDistance (km)\n";

    VertexNode *vertex = graph.getHead();
    while (vertex)
    {
        EdgeNode *edge = vertex->adjHead;
        while (edge)
        {
            if (shouldEmitEdge(vertex->name, edge->to))
            {
                output << vertex->name << '\t'
                       << edge->to << '\t'
                       << static_cast<int>(edge->weight) << '\n';
            }
            edge = edge->next;
        }
        vertex = vertex->next;
    }

    return true;
}

bool DataManager::saveBinary(const char *filename, const Graph &graph)
{
    std::ofstream output(filename, std::ios::binary);
    if (!output.is_open())
        return false;

    BinaryHeader header{};
    std::memcpy(header.magic, MAGIC_VALUE, sizeof(header.magic));
    header.version = BINARY_VERSION;
    header.vertexCount = countVertices(graph);
    header.edgeCount = countUniqueEdges(graph);

    output.write(reinterpret_cast<const char *>(&header), sizeof(header));

    VertexNode *vertex = graph.getHead();
    while (vertex)
    {
        EdgeNode *edge = vertex->adjHead;
        while (edge)
        {
            if (shouldEmitEdge(vertex->name, edge->to))
            {
                BinaryEdge record;
                writeEdge(record, vertex->name, edge->to, static_cast<int>(edge->weight));
                output.write(reinterpret_cast<const char *>(&record), sizeof(record));
            }
            edge = edge->next;
        }
        vertex = vertex->next;
    }

    return true;
}

bool DataManager::saveResultAsJSON(const char *filename, const PathResult &result)
{
    std::ofstream output(filename);
    if (!output.is_open())
        return false;
    output << "{\n";
    output << "  \"totalDistance\": " << result.totalDistance << ",\n";
    output << "  \"reachable\": " << (result.reachable ? "true" : "false") << ",\n";
    output << "  \"nodeCount\": " << result.nodeCount << ",\n";
    output << "  \"nodes\": [\n";
    for (int i = 0; i < result.nodeCount; ++i)
    {
        output << "    \"" << result.nodes[i] << "\"";
        if (i + 1 < result.nodeCount)
            output << ",";
        output << "\n";
    }
    output << "  ]\n";
    output << "}\n";
    return true;
}

bool DataManager::loadJSON(const char *filename, Graph &graph)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        return false;
    }

    graph.clear();

    // Simple manual parsing
    char line[1024];
    char currentId[64] = "";
    char currentName[128] = "";
    bool inNodes = false;
    bool inEdges = false;

    // Maps for ID -> Name
    HashTable idToNameMap(2000);

    // Pass 1: Read Nodes and build ID->Name map
    while (fgets(line, sizeof(line), fp))
    {
        std::string s(line);
        if (s.find("\"nodes\":") != std::string::npos) { inNodes = true; inEdges = false; continue; }
        if (s.find("\"edges\":") != std::string::npos) { inNodes = false; inEdges = true; continue; }

        if (inNodes)
        {
            if (s.find("\"id\":") != std::string::npos)
            {
                sscanf(line, "      \"id\": \"%[^\"]\"", currentId);
                // Default name is ID
                std::strcpy(currentName, currentId);
            }
            else if (s.find("\"name\":") != std::string::npos)
            {
                sscanf(line, "      \"name\": \"%[^\"]\"", currentName);
            }
            else if (s.find("},") != std::string::npos || (s.find("}") != std::string::npos && !inEdges))
            {
                // End of node object
                if (std::strlen(currentId) > 0)
                {
                    // Store in map. We need to allocate memory for the name because HashTable stores void*
                    char *nameCopy = new char[std::strlen(currentName) + 1];
                    std::strcpy(nameCopy, currentName);
                    idToNameMap.insert(currentId, nameCopy);

                    // Add to graph
                    graph.addVertex(currentName);

                    // Reset
                    currentId[0] = '\0';
                    currentName[0] = '\0';
                }
            }
        }
    }

    // Pass 2: Read Edges (Rewind file)
    fseek(fp, 0, SEEK_SET);
    inNodes = false;
    inEdges = false;
    char fromId[64] = "";
    char toId[64] = "";
    double weight = 0.0;

    while (fgets(line, sizeof(line), fp))
    {
        std::string s(line);
        if (s.find("\"nodes\":") != std::string::npos) { inNodes = true; inEdges = false; continue; }
        if (s.find("\"edges\":") != std::string::npos) { inNodes = false; inEdges = true; continue; }

        if (inEdges)
        {
            if (s.find("\"from\":") != std::string::npos)
            {
                sscanf(line, "      \"from\": \"%[^\"]\"", fromId);
            }
            else if (s.find("\"to\":") != std::string::npos)
            {
                sscanf(line, "      \"to\": \"%[^\"]\"", toId);
            }
            else if (s.find("\"weight\":") != std::string::npos)
            {
                sscanf(line, "      \"weight\": %lf", &weight);
            }
            else if (s.find("},") != std::string::npos || (s.find("}") != std::string::npos && inEdges))
            {
                // End of edge object
                if (std::strlen(fromId) > 0 && std::strlen(toId) > 0)
                {
                    // Resolve names
                    char *fromName = (char*)idToNameMap.find(fromId);
                    char *toName = (char*)idToNameMap.find(toId);

                    if (fromName && toName)
                    {
                        graph.addEdge(fromName, toName, (int)weight);
                    }

                    // Reset
                    fromId[0] = '\0';
                    toId[0] = '\0';
                    weight = 0.0;
                }
            }
        }
    }
    fclose(fp);
    return true;
}