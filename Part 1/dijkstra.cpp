// -----------------------------------
//  Name: Rachel Ellis and Anushka Khare
//  ID: 1618966 and 1617774
//  CMPUT 275
//
//  Assignment Part 1: Trivial Navigation System
// ------------------------------------

#include <unordered_map>
#include "wdigraph.h"
#include "dijkstra.h"
#include "heap.h"
#include <iostream>
using namespace std;

typedef pair<int, int> PII; 
typedef pair<int, long long> PIL;

/*
  Description: Compute the least cost of paths that starts at a specific vertex
              Funcitons from binary heap are used for the implementation

  Arguments: 
    graph (const WDiagraph&): reference to an instance of WDiagraph
    startVertex (int): vertex is considered as the root of the search tree
    tree (unordered map<int, PIL>): stores the two vertices and the cost of
                    their edge and is used to construct the least cost path

Return:
  (void)
*/
void dijkstra(const WDigraph& graph, int startVertex, unordered_map<int, PIL>& tree) {
    //creating a binary heap to store the points being explored
    BinaryHeap <PII, long long> StoringPoints;

    // putting in the starting vertex as the root node
    StoringPoints.insert(PII(startVertex, -1), 0);
    
    // while the heap isn't empty
    while (StoringPoints.size() != 0) {
        
        // return and remove the the min item from the heap
       HeapItem <PII, long long> point = StoringPoints.min();
       StoringPoints.popMin();  

       int first_point = point.item.first;
       int sec_point = point.item.second;
       long long dist = point.key;
       
       // check if the node is not reached 
        if (tree.find(first_point) != tree.end()) {
            continue;
        }

        // append the current node to the reached map
        tree[first_point] = PIL(sec_point, dist); 

        // look at the neighbouring points of the nearest waypoint and add them to the heap
        for (auto iter = graph.neighbours(first_point); iter != graph.endIterator(first_point); iter++) {
            int nbr = *iter; 
            // neighbour's cost will be the previous cost (dist) plus the cost of the new edge
            long long cost = dist + graph.getCost(first_point, nbr); 
            StoringPoints.insert(PII(nbr, first_point), cost);
        }
    }
}