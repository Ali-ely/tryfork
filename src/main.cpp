#include "DataManager.h"
#include "ShortestPath.h"
#include "graph.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <limits>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using std::optional;
using std::string;
using std::unordered_map;
using std::vector;

namespace
{
    // ----- ANSI Colors -----
    const string RESET = "\033[0m";
    const string RED = "\033[31m";
    const string GREEN = "\033[32m";
    const string YELLOW = "\033[33m";
    const string BLUE = "\033[34m";
    const string CYAN = "\033[36m";
    const string BOLD = "\033[1m";

    struct ComputedPath
    {
        float distance = 0.0f;
        vector<string> nodes;
    };

    struct CategoryOption
    {
        int menuIndex;
        string label;
        string key;
    };

    const vector<CategoryOption> CATEGORY_MENU = {
        {1, "Food", "food"},
        {2, "Drinks", "drinks"},
        {3, "Supermarket", "supermarket"},
        {4, "Prayer Rooms", "prayer"}};

    const unordered_map<string, vector<string>> CATEGORY_TARGETS = {
        {"food", {"Laroma", "Bartlett Plaza", "Pepsi Entrance"}},
        {"drinks", {"Bartlett Plaza", "Pepsi Entrance"}},
        {"supermarket", {"Campus Center", "Garden Entrance"}},
        {"prayer", {"Moataz Al Alfi Hall", "Bassily Auditorium"}}};

    string toLower(string value)
    {
        std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c)
                       { return static_cast<char>(std::tolower(c)); });
        return value;
    }

    vector<string> collectLocations(const Graph &graph)
    {
        vector<string> names;
        VertexNode *vertex = graph.getHead();
        while (vertex)
        {
            names.emplace_back(vertex->name);
            vertex = vertex->next;
        }
        std::sort(names.begin(), names.end());
        names.erase(std::unique(names.begin(), names.end()), names.end());
        return names;
    }

    vector<string> findMatchingLocations(const string &query, const vector<string> &allLocations)
    {
        vector<string> matches;
        const string needle = toLower(query);
        for (const string &location : allLocations)
        {
            if (toLower(location).find(needle) != string::npos)
            {
                matches.push_back(location);
            }
        }
        return matches;
    }

    void clearScreen()
    {
#ifdef _WIN32
        std::system("CLS");
#else
        std::system("clear");
#endif
    }

    optional<int> readIntegerChoice(int minValue, int maxValue)
    {
        string line;
        if (!std::getline(std::cin, line))
            return std::nullopt;

        try
        {
            int value = std::stoi(line);
            if (value >= minValue && value <= maxValue)
                return value;
        }
        catch (...)
        {
        }

        return optional<int>();
    }

    string disambiguate(const vector<string> &options)
    {
        if (options.empty())
            return "";
        if (options.size() == 1)
            return options.front();

        while (true)
        {
            std::cout << YELLOW << "Did you mean:" << RESET << "\n";
            for (size_t i = 0; i < options.size(); ++i)
            {
                std::cout << "  " << (i + 1) << ") " << options[i] << "\n";
            }
            std::cout << BLUE << "Choose (1-" << options.size() << "): " << RESET;

            auto choice = readIntegerChoice(1, static_cast<int>(options.size()));
            if (choice)
                return options[static_cast<size_t>(*choice - 1)];

            std::cout << RED << "Invalid choice. Try again." << RESET << "\n";
        }
    }

    optional<ComputedPath> computePath(Graph &graph, const string &from, const string &to)
    {
        ShortestPath solver;
        PathResult raw;
        const bool reachable = solver.compute(graph, from.c_str(), to.c_str(), raw);
        optional<ComputedPath> result;
        if (reachable)
        {
            ComputedPath path;
            path.distance = raw.totalDistance;
            path.nodes.reserve(raw.nodeCount);
            for (int i = 0; i < raw.nodeCount; ++i)
            {
                path.nodes.emplace_back(raw.nodes[i]);
            }
            result = std::move(path);
        }
        solver.release(raw);
        return result;
    }

    string formatPathOutput(const ComputedPath &path)
    {
        std::ostringstream oss;
        oss << BOLD << CYAN << "Path:" << RESET << "\n";
        for (size_t i = 0; i < path.nodes.size(); ++i)
        {
            oss << path.nodes[i];
            if (i + 1 < path.nodes.size())
                oss << " -> ";
        }
        oss << "\n\n"
            << GREEN << "Estimated Distance: " << RESET
            << std::fixed << std::setprecision(1) << path.distance << " m\n";
        return oss.str();
    }

    optional<std::pair<string, ComputedPath>> findNearest(Graph &graph, const string &from, const vector<string> &candidates)
    {
        optional<std::pair<string, ComputedPath>> best;
        for (const string &candidate : candidates)
        {
            if (candidate.empty())
                continue;

            auto path = computePath(graph, from, candidate);
            if (!path)
                continue;

            if (!best || path->distance < best->second.distance)
            {
                best = std::make_pair(candidate, *path);
            }
        }
        return best;
    }

    string promptForLocation(const string &prompt, const vector<string> &allLocations)
    {
        while (true)
        {
            std::cout << BLUE << prompt << "\n> " << RESET;
            string input;
            if (!std::getline(std::cin, input))
                return "";

            auto matches = findMatchingLocations(input, allLocations);
            if (matches.empty())
            {
                std::cout << RED << "No matching locations found. Try again." << RESET << "\n";
                continue;
            }

            return disambiguate(matches);
        }
    }

    void waitForKey()
    {
        std::cout << GREEN << "Press Enter to continue..." << RESET;
        std::string dummy;
        std::getline(std::cin, dummy);
    }

    const CategoryOption *findCategoryOption(int menuIndex)
    {
        for (const auto &option : CATEGORY_MENU)
        {
            if (option.menuIndex == menuIndex)
                return &option;
        }
        return nullptr;
    }

    string resolveDatasetPath(int argc, char **argv)
    {
        if (argc > 1)
        {
            return std::filesystem::absolute(argv[1]).string();
        }

        const std::filesystem::path exeDir = std::filesystem::current_path();
        const std::filesystem::path sourceDir = std::filesystem::path(__FILE__).parent_path();
        const std::filesystem::path repoRoot = sourceDir.parent_path();

        vector<std::filesystem::path> baseDirs = {exeDir};
        if (!exeDir.empty())
        {
            baseDirs.push_back(exeDir.parent_path());
            baseDirs.push_back(exeDir.parent_path().parent_path());
        }
        baseDirs.push_back(repoRoot);
        baseDirs.push_back(repoRoot.parent_path());

        const vector<string> relativeNames = {
            "auc_graph_with_names.json",
            "data/auc_graph_with_names.json"};

        for (const auto &base : baseDirs)
        {
            if (base.empty())
                continue;

            for (const auto &name : relativeNames)
            {
                const auto candidate = base / name;
                if (std::filesystem::exists(candidate))
                {
                    return std::filesystem::absolute(candidate).string();
                }
            }
        }

        return {};
    }
}

int main(int argc, char **argv)
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    Graph campusGraph;

    const string datasetPath = resolveDatasetPath(argc, argv);
    if (datasetPath.empty() || !DataManager::loadJSON(datasetPath.c_str(), campusGraph))
    {
        std::cerr << "Failed to load campus graph from '" << (datasetPath.empty() ? string("<unspecified>") : datasetPath) << "'." << std::endl;
        return 1;
    }

    vector<string> allLocations = collectLocations(campusGraph);
    if (allLocations.empty())
    {
        std::cerr << "The loaded graph does not contain any locations." << std::endl;
        return 1;
    }

    string currentLocation;

    while (currentLocation.empty())
    {
        clearScreen();
        std::cout << CYAN << BOLD
                  << "====================================\n"
                  << "        AUC Campus Navigator        \n"
                  << "====================================\n"
                  << RESET;

        currentLocation = promptForLocation("Where are you now? (nearest class/room/spot)", allLocations);
        if (currentLocation.empty())
            return 0;
    }

    while (true)
    {
        clearScreen();
        std::cout << CYAN << BOLD << "====================================\n"
                  << RESET;
        std::cout << BOLD << "Current Location: " << YELLOW << currentLocation << RESET << "\n";
        std::cout << CYAN << BOLD << "====================================\n"
                  << RESET;
        std::cout << GREEN
                  << "1) Go To...\n"
                  << "2) Change My Current Location\n"
                  << "3) Where is the nearest...\n"
                  << "4) Exit\n"
                  << RESET << "> ";

        auto choice = readIntegerChoice(1, 4);
        if (!choice)
        {
            std::cout << RED << "Invalid choice. Try again." << RESET << "\n";
            continue;
        }

        if (*choice == 1)
        {
            string destination = promptForLocation("Where would you like to go?", allLocations);
            if (destination.empty())
                break;

            clearScreen();
            std::cout << CYAN << BOLD << "----- Shortest Path -----" << RESET << "\n";
            auto path = computePath(campusGraph, currentLocation, destination);
            if (!path)
            {
                std::cout << RED << "No path could be found between these locations." << RESET << "\n";
            }
            else
            {
                std::cout << formatPathOutput(*path);
            }
            waitForKey();
        }
        else if (*choice == 2)
        {
            string newLocation = promptForLocation("Enter your new location:", allLocations);
            if (newLocation.empty())
                break;
            currentLocation = newLocation;
        }
        else if (*choice == 3)
        {
            std::cout << GREEN;
            for (const auto &option : CATEGORY_MENU)
            {
                std::cout << option.menuIndex << ") " << option.label << "\n";
            }
            std::cout << RESET << "> ";

            auto categoryChoice = readIntegerChoice(1, static_cast<int>(CATEGORY_MENU.size()));
            if (!categoryChoice)
            {
                std::cout << RED << "Invalid selection." << RESET << "\n";
                waitForKey();
                continue;
            }

            const CategoryOption *category = findCategoryOption(*categoryChoice);
            if (!category)
            {
                std::cout << RED << "Category not available." << RESET << "\n";
                waitForKey();
                continue;
            }

            auto targetsIt = CATEGORY_TARGETS.find(category->key);
            if (targetsIt == CATEGORY_TARGETS.end())
            {
                std::cout << RED << "No targets configured for this category." << RESET << "\n";
                waitForKey();
                continue;
            }

            auto nearest = findNearest(campusGraph, currentLocation, targetsIt->second);
            if (!nearest)
            {
                std::cout << RED << "Could not find any reachable locations for this category." << RESET << "\n";
            }
            else
            {
                std::cout << CYAN << BOLD << "----- Nearest Location -----" << RESET << "\n";
                std::cout << CYAN << "From: " << RESET << currentLocation << "\n";
                std::cout << CYAN << "Nearest " << category->label << ": " << RESET << nearest->first << "\n\n";
                std::cout << formatPathOutput(nearest->second);
            }
            waitForKey();
        }
        else if (*choice == 4)
        {
            std::cout << GREEN << "Goodbye!" << RESET << "\n";
            break;
        }
    }

    return 0;
}
