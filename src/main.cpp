#include "graph.h"
#include "MinHeap.h"
#include "MinHeap.tpp"
#include <iostream>
#include <string>

using namespace std;

void testMinHeap()
{
    cout << "\n=== Testing MinHeap ===" << endl;
    cout << "----------------------" << endl;

    // Test with integers
    cout << "\n1. Testing MinHeap with integers:" << endl;
    MinHeap<int> intHeap(10);

    cout << "Inserting: 50, 30, 70, 10, 40, 60, 80" << endl;
    intHeap.insert(50);
    intHeap.insert(30);
    intHeap.insert(70);
    intHeap.insert(10);
    intHeap.insert(40);
    intHeap.insert(60);
    intHeap.insert(80);

    cout << "Heap contents: ";
    intHeap.printHeap();

    cout << "Minimum element: " << intHeap.getMin() << endl;

    cout << "\nExtracting elements in sorted order:" << endl;
    while (!intHeap.isEmpty())
    {
        cout << intHeap.extractMin() << " ";
    }
    cout << endl;

    // Test with doubles
    cout << "\n2. Testing MinHeap with doubles:" << endl;
    MinHeap<double> doubleHeap(5);

    cout << "Inserting: 3.14, 2.71, 1.41, 9.87, 5.55" << endl;
    doubleHeap.insert(3.14);
    doubleHeap.insert(2.71);
    doubleHeap.insert(1.41);
    doubleHeap.insert(9.87);
    doubleHeap.insert(5.55);

    cout << "Heap contents: ";
    doubleHeap.printHeap();

    cout << "Extracting minimum: " << doubleHeap.extractMin() << endl;
    cout << "New minimum: " << doubleHeap.getMin() << endl;
}

void testGraph()
{
    cout << "\n=== Testing Graph ===" << endl;
    cout << "---------------------" << endl;

    Graph graph;

    // Test 1: Manual graph creation
    cout << "\n1. Creating a small test graph manually:" << endl;
    graph.addVertex("Cairo");
    graph.addVertex("Alexandria");
    graph.addVertex("Giza");
    graph.addVertex("Luxor");

    graph.addEdge("Cairo", "Alexandria", 220);
    graph.addEdge("Cairo", "Giza", 20);
    graph.addEdge("Cairo", "Luxor", 670);
    graph.addEdge("Alexandria", "Giza", 200);

    cout << "\nGraph structure:" << endl;
    graph.display();

    // Test 2: Clear and reload
    cout << "\n2. Testing clear function..." << endl;
    graph.clear();
    cout << "Graph after clear:" << endl;
    graph.display();
    cout << "(Graph is empty)" << endl;

    // Test 3: Load real dataset
    cout << "\n3. Loading real dataset from file..." << endl;
    graph.readDataset("../../../../data/city_connections_dataset.txt");

    cout << "\nLoaded graph structure:" << endl;
    graph.display();
}

int main()
{
    cout << "========================================" << endl;
    cout << "   PathVista - Graph & MinHeap Testing" << endl;
    cout << "========================================" << endl;

    try
    {
        // Test MinHeap
        testMinHeap();

        // Test Graph
        testGraph();

        cout << "\n========================================" << endl;
        cout << "   All tests completed successfully!" << endl;
        cout << "========================================" << endl;
    }
    catch (const exception &e)
    {
        cerr << "\nError: " << e.what() << endl;
        return 1;
    }

    return 0;
}
