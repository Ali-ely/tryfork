# PathVista

A C++ project implementing Graph and MinHeap data structures for managing city connections and finding optimal paths.

## 📋 Table of Contents

- [Overview](#overview)
- [Project Structure](#project-structure)
- [Data Structures](#data-structures)
  - [Graph](#graph)
  - [MinHeap](#minheap)
- [Big-O Complexity Analysis](#big-o-complexity-analysis)
- [Building the Project](#building-the-project)
- [Running the Project](#running-the-project)
- [Running Tests](#running-tests)

---

## 🎯 Overview

PathVista is designed to manage and analyze city connection networks. It provides:

- **Graph data structure** for representing cities and their connections
- **MinHeap data structure** for efficient priority queue operations
- Support for loading real-world city connection datasets
- Comprehensive test suite using Google Test

---

## 📁 Project Structure

```
PathVista/
├── src/
│   ├── graph.h              # Graph class declaration
│   ├── graph.cpp            # Graph class implementation
│   ├── MinHeap.h            # MinHeap class declaration
│   ├── MinHeap.tpp          # MinHeap template implementation
│   ├── main.cpp             # Main program for testing
│   └── CMakeLists.txt       # CMake build configuration for src
├── test/
│   ├── graph_test.cpp       # Graph unit tests
│   ├── minheap_test.cpp     # MinHeap unit tests
│   └── CMakeLists.txt       # CMake build configuration for tests
├── data/
│   └── city_connections_dataset.txt  # Real-world city connection data
├── CMakeLists.txt           # Root CMake configuration
└── README.md                # This file
```

---

## 🏗️ Data Structures

### Graph

The **Graph** class represents an undirected, weighted graph using an **adjacency list** implementation with linked lists.

#### Structure

- **VertexNode**: Represents a city (vertex)
  - `char name[64]`: City name
  - `EdgeNode* adjHead`: Pointer to adjacency list
  - `VertexNode* next`: Pointer to next vertex

- **EdgeNode**: Represents a connection (edge) between cities
  - `char to[64]`: Destination city name
  - `int weight`: Distance/cost of connection
  - `EdgeNode* next`: Pointer to next edge

#### Public Methods

##### 1. `Graph()`

**Description**: Constructor that initializes an empty graph.

**Big-O Complexity**: `O(1)`

---

##### 2. `~Graph()`

**Description**: Destructor that frees all allocated memory by calling `clear()`.

**Big-O Complexity**: `O(V + E)` where V = number of vertices, E = number of edges

---

##### 3. `void addVertex(const char* name)`

**Description**: Adds a new vertex (city) to the graph if it doesn't already exist.

**Parameters**:

- `name`: Name of the city to add (max 63 characters)

**Big-O Complexity**: `O(V)` where V = number of vertices (due to duplicate check and traversal to end of list)

**Example**:

```cpp
Graph g;
g.addVertex("Cairo");
g.addVertex("Alexandria");
```

---

##### 4. `void addEdge(const char* from, const char* to, int weight)`

**Description**: Adds an undirected edge between two cities with a given weight. Automatically creates vertices if they don't exist.

**Parameters**:

- `from`: Source city name
- `to`: Destination city name
- `weight`: Distance or cost of the connection

**Big-O Complexity**: `O(V + E)` where V = number of vertices, E = number of edges per vertex

- Finding vertices: `O(V)`
- Adding edges to adjacency lists: `O(E)`

**Example**:

```cpp
g.addEdge("Cairo", "Alexandria", 220);
g.addEdge("Cairo", "Giza", 20);
```

---

##### 5. `void display() const`

**Description**: Displays the entire graph structure showing all vertices and their connections.

**Big-O Complexity**: `O(V + E)` where V = number of vertices, E = total number of edges

**Output Format**:

```
Cairo -> Alexandria (220) -> Giza (20)
Alexandria -> Cairo (220)
Giza -> Cairo (20)
```

---

##### 6. `void clear()`

**Description**: Removes all vertices and edges from the graph, freeing all allocated memory.

**Big-O Complexity**: `O(V + E)` where V = number of vertices, E = number of edges

---

##### 7. `void readDataset(const char* filename)`

**Description**: Loads city connections from a file. File format: `City1 City2 Distance` (one per line, first line is header).

**Parameters**:

- `filename`: Path to the dataset file

**Big-O Complexity**: `O(N * (V + E))` where N = number of lines in file, V = vertices, E = edges

- Each line requires adding an edge: `O(V + E)` per line

**File Format**:

```
City1 City2 Distance
Cairo Alexandria 220
Cairo Giza 20
Alexandria Giza 200
```

**Example**:

```cpp
g.readDataset("data/city_connections_dataset.txt");
```

---

### MinHeap

The **MinHeap** class is a template-based implementation of a binary min-heap using an array.

#### Structure

- **Template Type `T`**: Can store any comparable type (int, double, float, etc.)
- **Array-based**: Stored in contiguous memory for cache efficiency
- **Complete Binary Tree**: Satisfies heap property - parent ≤ children

#### Private Methods

##### Helper Functions

- `parent(int i)`: Returns parent index → `O(1)`
- `left(int i)`: Returns left child index → `O(1)`
- `right(int i)`: Returns right child index → `O(1)`
- `last()`: Returns last element index → `O(1)`
- `heapifyUp(int i)`: Restores heap property upward → `O(log n)`
- `heapifyDown(int i)`: Restores heap property downward → `O(log n)`

#### Public Methods

##### 1. `MinHeap(int capacity)`

**Description**: Constructor that creates a heap with specified capacity.

**Parameters**:

- `capacity`: Maximum number of elements the heap can hold

**Big-O Complexity**: `O(n)` where n = capacity (for array allocation)

**Example**:

```cpp
MinHeap<int> heap(100);
MinHeap<double> distances(50);
```

---

##### 2. `~MinHeap()`

**Description**: Destructor that frees the allocated array.

**Big-O Complexity**: `O(1)`

---

##### 3. `bool isEmpty() const`

**Description**: Checks if the heap contains no elements.

**Returns**: `true` if heap is empty, `false` otherwise

**Big-O Complexity**: `O(1)`

**Example**:

```cpp
if (heap.isEmpty()) {
    cout << "Heap is empty" << endl;
}
```

---

##### 4. `bool isFull() const`

**Description**: Checks if the heap has reached its maximum capacity.

**Returns**: `true` if heap is full, `false` otherwise

**Big-O Complexity**: `O(1)`

**Example**:

```cpp
if (!heap.isFull()) {
    heap.insert(42);
}
```

---

##### 5. `void insert(const T& key)`

**Description**: Inserts a new element into the heap and maintains the min-heap property.

**Parameters**:

- `key`: Element to insert

**Big-O Complexity**: `O(log n)` where n = number of elements in heap

- Insert at end: `O(1)`
- Heapify up: `O(log n)` - worst case travels from leaf to root

**Example**:

```cpp
MinHeap<int> heap(10);
heap.insert(50);
heap.insert(30);
heap.insert(70);
heap.insert(10);  // Heap: [10, 30, 70, 50]
```

---

##### 6. `T getMin()`

**Description**: Returns the minimum element (root) without removing it. Returns default value if empty.

**Returns**: Minimum element in the heap

**Big-O Complexity**: `O(1)` - minimum is always at index 0

**Example**:

```cpp
int min = heap.getMin();  // Doesn't remove the element
cout << "Minimum: " << min << endl;
```

---

##### 7. `T extractMin()`

**Description**: Removes and returns the minimum element, then restores heap property.

**Returns**: Minimum element that was removed

**Big-O Complexity**: `O(log n)` where n = number of elements

- Remove root: `O(1)`
- Replace with last element: `O(1)`
- Heapify down: `O(log n)` - worst case travels from root to leaf

**Example**:

```cpp
while (!heap.isEmpty()) {
    int min = heap.extractMin();
    cout << min << " ";  // Prints elements in sorted order
}
```

---

##### 8. `void printHeap()`

**Description**: Prints all elements in the heap array (level-order).

**Big-O Complexity**: `O(n)` where n = number of elements in heap

**Example**:

```cpp
heap.printHeap();  // Output: 10 30 50 70
```

---

## 📊 Big-O Complexity Analysis

### Graph Operations Summary

| Operation | Time Complexity | Space Complexity | Notes |
|-----------|----------------|------------------|-------|
| `Graph()` | O(1) | O(1) | Initialize empty graph |
| `~Graph()` | O(V + E) | O(1) | Delete all vertices and edges |
| `addVertex()` | O(V) | O(1) | Linear search for duplicates |
| `addEdge()` | O(V + E) | O(1) | Find vertices + add edges |
| `display()` | O(V + E) | O(1) | Visit all vertices and edges |
| `clear()` | O(V + E) | O(1) | Delete all nodes |
| `readDataset()` | O(N × (V + E)) | O(1) | N lines, each adds edge |
| `findVertexNode()` | O(V) | O(1) | Linear search (private) |

**Legend**: V = vertices, E = edges, N = number of lines in dataset

---

### MinHeap Operations Summary

| Operation | Time Complexity | Space Complexity | Notes |
|-----------|----------------|------------------|-------|
| `MinHeap(capacity)` | O(n) | O(n) | Allocate array of size n |
| `~MinHeap()` | O(1) | O(1) | Deallocate array |
| `isEmpty()` | O(1) | O(1) | Check size |
| `isFull()` | O(1) | O(1) | Check size vs capacity |
| `insert()` | O(log n) | O(1) | Add + heapify up |
| `getMin()` | O(1) | O(1) | Return root element |
| `extractMin()` | O(log n) | O(1) | Remove root + heapify down |
| `printHeap()` | O(n) | O(1) | Print all elements |
| `heapifyUp()` | O(log n) | O(1) | Bubble up to root (private) |
| `heapifyDown()` | O(log n) | O(1) | Bubble down to leaf (private) |

**Legend**: n = number of elements in heap

---

## 🔨 Building the Project

### Prerequisites

- CMake 3.14 or higher
- C++17 compatible compiler (GCC, Clang, or MSVC)
- Git (for cloning)

### Build Steps

1. **Clone the repository**:

```bash
git clone https://github.com/a7med-yamany/PathVista.git
cd PathVista
```

2. **Create build directory**:

```bash
mkdir build
cd build
```

3. **Configure with CMake**:

```bash
cmake ..
```

4. **Build the project**:

```bash
cmake --build .
```

On Windows, executables will be in:

- Main program: `build/src/Debug/PathVista.exe`
- Tests: `build/test/Debug/graph_tests.exe`

---

## 🚀 Running the Project

### Run the Main Program

**Windows**:

```powershell
.\src\Debug\PathVista.exe
```

**Linux/Mac**:

```bash
./src/PathVista
```

### Expected Output

The program will:

1. Test MinHeap operations with integers and doubles
2. Create a test graph with Egyptian cities
3. Load the real dataset from `data/city_connections_dataset.txt`
4. Display all graph structures

---

## 🧪 Running Tests

The project uses Google Test for unit testing.

### Run All Tests

```bash
cd build
ctest --output-on-failure
```

Or run the test executable directly:

**Windows**:

```powershell
.\test\Debug\graph_tests.exe
```

**Linux/Mac**:

```bash
./test/graph_tests
```

### Test Coverage

- **Graph Tests**: AddVertex, AddEdge, GraphIsUndirected, Clear, ReadDataset
- **MinHeap Tests**: CreateEmptyHeap, Insert, ExtractMin, HeapProperty, HeapFull

---

## 📝 Usage Examples

### Example 1: Creating a Graph

```cpp
#include "graph.h"

Graph cityGraph;

// Add cities
cityGraph.addVertex("Cairo");
cityGraph.addVertex("Alexandria");

// Add connections
cityGraph.addEdge("Cairo", "Alexandria", 220);
cityGraph.addEdge("Cairo", "Giza", 20);

// Display graph
cityGraph.display();
```

### Example 2: Using MinHeap for Priority Queue

```cpp
#include "MinHeap.h"
#include "MinHeap.tpp"

MinHeap<int> distances(100);

// Add distances
distances.insert(50);
distances.insert(30);
distances.insert(70);
distances.insert(10);

// Get shortest distance
int shortest = distances.getMin();  // Returns 10

// Process distances in order
while (!distances.isEmpty()) {
    int dist = distances.extractMin();
    cout << "Distance: " << dist << endl;
}
```

### Example 3: Loading Dataset

```cpp
Graph g;
g.readDataset("data/city_connections_dataset.txt");
g.display();  // Shows all loaded connections
```

---

## 📄 License

This project is created for educational purposes as part of the ADS course at AUC.

---

## 👥 Authors

- Ahmed Yamany ([@a7med-yamany](https://github.com/a7med-yamany))

- Omar Marey ([@OMarey1](https://github.com/OMarey1))

- Tarek Kamel ([@tarek-kamel](https://github.com/tarek-kamel))

- Adham Abotarboush ([@leo7-am](https://github.com/leo7-am))

---

## 🔗 Repository

[https://github.com/a7med-yamany/PathVista](https://github.com/a7med-yamany/PathVista)
