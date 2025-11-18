#include "searchShortestPath.h"
#include <iostream>
#include <cstring>

// void shortestPath(Graph& graph, const char* start, const char* destination)
// {
//     int numNodes = graph.getNumOfNodes();

//     int* distances = new int [numNodes];  //to save the wieghts
//     bool* visited = new bool [numNodes];  //to chck if visited before
//     // char (*previous) [64] = new char [numNodes][64]; //to keep track of the path
//     char (*cityName) [64] = new char [numNodes][64]; //to find the index of the node in the arrays

//     VertexNode* temp = graph.getHead();
//     int index = 0;

//     while (temp)     //getting the data from the graph
//     {
//         snprintf(cityName[index], sizeof(cityName[index]), "%s", temp->name);
//         temp = temp->next;
//         index++;
//     }

//     for (int i =0; i < numNodes; i++)
//     {
//         distances[i] = INT_MAX;
//         visited [i] = false;
//     }

//     int startIndex = -1;
//     for (int i =0; i < numNodes; i++)
//     {
//         if (strcmp (cityName[i], start) == 0)  //to compare names safely
//         {
//             startIndex = i;
//             break;
//         }
//     }
    
//     if (startIndex == -1)
//     {
//         cout << "city doesn't exist or something went wrong" << endl;
//         return;
//     }

//     MinHeap <heapNode> heap(numNodes); 

//     heapNode st;
//     snprintf(st.cityName, sizeof(st.cityName), "%s", start); //to copy the name safely
//     st.distance = 0;
//     heap.insert(st);

//     while (! heap.isEmpty())
//     {
        

//     }





// }

